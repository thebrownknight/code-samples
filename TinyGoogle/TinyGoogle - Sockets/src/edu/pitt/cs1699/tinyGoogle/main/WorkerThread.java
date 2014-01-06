import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.TreeMap;
import java.util.Map;

/**
 * Worker.java
 * @author Nikhil Venkatesh 
 * @version 1.0.0
 *
 */
public class WorkerThread extends Thread {
	private int port;
	private WorkerServer _server;
	private ServerSocket serverSock;
	ObjectInputStream input;
	ObjectOutputStream output;
	ArrayList<TreeMap<String, Integer>> freqs = new ArrayList<TreeMap<String, Integer>>();
	private ArrayList<Object> params;
	private ArrayList<String> result;
	public WorkerThread(int portnum) {
		port = portnum;
	}
	public WorkerThread(WorkerServer server) {
		_server = server;
		this.start();
	}
	/**
	 * for debug use only (for now, maybe this would work wonders later)
	 * @param params
	 */
	public WorkerThread(ArrayList<Object> params) {
		this.params = params;
	}
	
	public void initialize() {
		
	}
	
	public ArrayList<String> getResult() {
		return result;
	}
	/**
	 * 
	 * @param params - Contains parameters for the map task. Should contain 4 values. 1st is filename. 2nd is directory. 3rd is block. 4th is block size.
	 * @return
	 */
	public boolean map(ArrayList<Object> params) {
		if(params.size() != 4 || !(params.get(0) instanceof String) || !(params.get(1) instanceof String) || !(params.get(2) instanceof Integer) || !(params.get(3) instanceof Integer)) 
			return false;
		// parse parameters
		String filename = (String)(params.get(1));
		String path = (String)(params.get(0));
		int block = (Integer)(params.get(2));
		int block_size = (Integer)(params.get(3));
	
		File f = new File(path+"/"+filename);
		System.out.println("FILE LENGTH: " + f.length());
		FileInputStream fis = null;
		FileWriter fw = null;
		
		try {
			// open source file for reading
			fis = new FileInputStream(f);
			// init buffer
			byte [] buf = new byte[block_size];
			// advance to appropriate block
			fis.skip(block*block_size);
			// read appropriate block
			fis.read(buf, 0, block_size);
			// convert to a string
			String line = new String(buf);
			System.out.println("Line being checked: " + line);
			// initialize frequency maps
			for(int i=0; i<Split.num_splits; i++) {
				freqs.add(new TreeMap<String, Integer>());
			}
			// split the file into words. remove everything that is not a letter or -
			String [] words = line.split("[\\W+]");
			Map<String, Integer> temp = null;
			
			for(String word : words) {
				// for appropriate string comparisons
				word = word.toUpperCase();
				//System.out.println("word being checked: " + word);
				// find which bin the word belongs in
				Split sp = Split.getSplit(word);
				if(sp != null) {
					// add the string to the map with count = 1 or replace the entry with the count incremented
					temp = freqs.get(sp.num());
					Integer tempInt = temp.get(word); 
					int count = tempInt == null ? 0: tempInt;
					temp.put(word, count + 1);
				}
			}
			// write to the files used by the reducers. one file per split
			for(int i=0; i<Split.num_splits; i++) {
				// use utils to guarantee standard naming conventions
				String outFileName = Utils.getMappedPath(path, filename, Split.getSplit(i).tag(), block);
				fw = new FileWriter(outFileName);
				for(Map.Entry<String, Integer> e : freqs.get(i).entrySet()) {
					// write each value pair
					fw.write(e.getKey()+","+e.getValue()+"\n");
				}
				if(fw != null)
				try {
					fw.close();
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
		finally {
			if(fis != null)
				try {
					fis.close();
				} catch (IOException e) {
					e.printStackTrace();
				}	
			if(fw != null)
				try {
					fw.close();
				} catch (IOException e) {
					e.printStackTrace();
				}
		}
		return true;
	}
	
	/**
	 * 
	 * @param params - Contains parameters for the reduce task. Should contain 4 values. 1st is filename. 2nd is directory. 3rd is Split. 4th is num blocks.
	 * @return
	 */
	public boolean reduce(ArrayList<Object> params) {
		if(params.size() != 4 || !(params.get(0) instanceof String) || !(params.get(1) instanceof String) || !(params.get(2) instanceof Integer) || !(params.get(3) instanceof Integer))
			return false;
		// parse parameters
		String filename = (String)(params.get(1));
		String path = (String)(params.get(0));
		int split = (Integer)(params.get(2));
		int num_blocks = (Integer)(params.get(3));
		
		// initialize frequency map
		TreeMap<String, Integer> summap = new TreeMap<String, Integer>();
		
		FileReader fr = null;
		BufferedReader br = null;
		
		// for each block, read all files into frequency map with the same split
		for(int i=0; i<num_blocks; i++) {
			try {
				// use utils to guarantee standard naming conventions
				String inPath = Utils.getMappedPath(path, filename, Split.getSplit(split).tag(), i);
				
				File f = new File(inPath);
				fr = new FileReader(f);
				br = new BufferedReader(fr);
				
				String temp = null;
				String[] vals;
				
				// read each <word, frequency> pair line by line
				while((temp = br.readLine()) != null) {
					// separate pairs into the key and value
					vals = temp.split(",");
					// dummy-proofing it
					if(vals.length == 2) {
						// add to the map with frequency or replace the entry with the frequency added to the running total
						Integer tempInt = summap.get(vals[0]); 
						int count = tempInt == null ? 0: tempInt;
						summap.put(vals[0], count + Integer.parseInt(vals[1]));
					}
				}
			} catch (FileNotFoundException e) {
				e.printStackTrace();
			} catch (IOException e) {
				e.printStackTrace();
			} finally {
				if(fr != null)
					try {
						fr.close();
					} catch (IOException e1) {
						e1.printStackTrace();
					}
				if(br != null)
					try {
						br.close();
					} catch (IOException e1) {
						e1.printStackTrace();
					}
			}
		}
		// write to the reduced file
		FileWriter fw = null;
		try {
			// use utils to guarantee standard naming conventions
			fw = new FileWriter(Utils.getReducedPath(path, filename, Split.getSplit(split).tag()));
			for(Map.Entry<String, Integer> e : summap.entrySet()) {
				// write each value pair
				fw.write(e.getKey()+","+e.getValue()+"\n");
			}
		}
		catch(IOException e) {
			e.printStackTrace();
		} finally {
			if(fw != null)
			try {
				fw.close();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
		return true;
	}
	
	/**
	 * 
	 * @param params - Contains parameters for the searchmasterindex task. Should contain 2 values. 1st is word list. 2nd is split.
	 * @return
	 */
	public ArrayList<String> search(ArrayList<Object> params) {
		if(params.size() != 2 || !(params.get(0) instanceof ArrayList) || !(params.get(1) instanceof Integer))
			return null;
		// parse parameters
		ArrayList<String> words = (ArrayList<String>)(params.get(0));
		ArrayList<String> docs = new ArrayList<String>();
		int split = (Integer)(params.get(1));
		
		// read in master index file
			FileReader fr = null;
			BufferedReader br = null;
			try {
				// use utils to guarantee standard naming conventions
				String inPath = Utils.getMasterPath(Split.getSplit(split).tag());
				//System.out.println("INPATH: " + inPath);
				File f = new File(inPath);
				if(!f.exists()) {
					f.createNewFile();
				}
				fr = new FileReader(f);
				br = new BufferedReader(fr);
				
				String temp = null;
				String[] vals;
				
				// read each <word, frequency> pair line by line
				while((temp = br.readLine()) != null) {
					// separate pairs into the key and value
					vals = temp.split("[|]");
					// dummy-proofing it
					if(vals.length == 2) {
						for (String s : words) {
							if (vals[0].equalsIgnoreCase(s)){
								docs.add(temp);
								System.out.println(temp);
							}
						}
					}
				}
			} catch (FileNotFoundException e) {
				e.printStackTrace();
			} catch (IOException e) {
				e.printStackTrace();
			} finally {
				if(fr != null)
					try {
						fr.close();
					} catch (IOException e1) {
						e1.printStackTrace();
					}
				if(br != null)
					try {
						br.close();
					} catch (IOException e1) {
						e1.printStackTrace();
					}
			}
		if(docs.size() == 0)
			return null;
		return docs;
	}
	
	/**
	 * 
	 * @param params - Contains parameters for the addtomasterindex task. Should contain 3 values. 1st is filename. 2nd is directory. 3rd is Split.
	 * @return
	 */
	public boolean index(ArrayList<Object> params) {
		if(params.size() != 3 || !(params.get(0) instanceof String) || !(params.get(1) instanceof String) || !(params.get(2) instanceof Integer))
			return false;
		// parse parameters
		String filename = (String)(params.get(1));
		String path = (String)(params.get(0));
		int split = (Integer)(params.get(2));
		
		TreeMap<String, TreeMap<String, Integer>> masterIndexMap = new TreeMap<String, TreeMap<String, Integer>>();
		// read in master index file
		FileReader fr = null;
		BufferedReader br = null;
		try {
			// use utils to guarantee standard naming conventions
			String inPath = Utils.getMasterPath(Split.getSplit(split).tag());
			File f = new File(inPath);
			if(!f.exists()) {
				f.createNewFile();
			}
			fr = new FileReader(f);
			br = new BufferedReader(fr);
			
			String temp = null;
			String[] vals, pairs, tuple;
			TreeMap<String, Integer> tempMap = null;
			
			// read each <word, frequency> pair line by line
			while((temp = br.readLine()) != null) {
				// separate pairs into the key and value
				vals = temp.split("[|]");
				// dummy-proofing it
				if(vals.length == 2) {
					pairs = vals[1].split(";");
					// build the map of <document, frequency> value pairs
					tempMap = new TreeMap<String, Integer>();
					for(String pair : pairs) {
						tuple = pair.split(",");
						if(tuple.length == 2) {
							tempMap.put(tuple[0], Integer.parseInt(tuple[1]));
						}
					}
					// add to the map with term and <document, frequency> pair
					masterIndexMap.put(vals[0], tempMap);
				}
			}
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		} finally {
			if(fr != null)
				try {
					fr.close();
				} catch (IOException e1) {
					e1.printStackTrace();
				}
			if(br != null)
				try {
					br.close();
				} catch (IOException e1) {
					e1.printStackTrace();
				}
		}
		// read in newly counted files
		try {
			// use utils to guarantee standard naming conventions
			String inPath = Utils.getReducedPath(path, filename, Split.getSplit(split).tag());
			
			File f = new File(inPath);
			fr = new FileReader(f);
			br = new BufferedReader(fr);
			
			String temp = null;
			String[] vals;
			TreeMap<String, Integer> tempMap;
			
			// read each <word, frequency> pair line by line
			while((temp = br.readLine()) != null) {
				// separate pairs into the key and value
				vals = temp.split(",");
				// dummy-proofing it
				if(vals.length == 2) {
					// add to the master index map with frequency
					tempMap = masterIndexMap.get(vals[0]); 
					if(tempMap == null) {
						tempMap = new TreeMap<String, Integer>();
						masterIndexMap.put(vals[0], null);
					}
					// add document and frequency
					tempMap.put(filename, Integer.parseInt(vals[1]));
					masterIndexMap.put(vals[0], tempMap);
				}
			}
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		} finally {
			if(fr != null)
				try {
					fr.close();
				} catch (IOException e1) {
					e1.printStackTrace();
				}
			if(br != null)
				try {
					br.close();
				} catch (IOException e1) {
					e1.printStackTrace();
				}
		}
		
		FileWriter fw = null;
		try {
			// use utils to guarantee standard naming conventions
			fw = new FileWriter(Utils.getMasterPath(Split.getSplit(split).tag()));
			for(Map.Entry<String, TreeMap<String, Integer>> e : masterIndexMap.entrySet()) {
				// write the key (term)
				fw.write(e.getKey()+"|");
				// print all of the <document, frequency> terms
				for(Map.Entry<String, Integer> i : e.getValue().entrySet()) {
					// write the <document, frequency> pair
					fw.write(i.getKey()+","+i.getValue()+";");
				}
				fw.write("\n");
			}
		}
		catch(IOException e) {
			e.printStackTrace();
		} finally {
			if(fw != null)
			try {
				fw.close();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
		return true;
	}

	public void run() {
		if(_server != null) {
			while (true) {
			
	            Socket socket;
	            try {
	            	socket = _server.connectionQueue.take();
	            }
	            catch (InterruptedException e) {
	               continue; // (If interrupted, just go back to start of while loop.)
	            }
	            String clientAddress = socket.getInetAddress().toString();
	            try {
	    			System.out.println("Connection from " + clientAddress );
					System.out.println("Handled by worker " + this);
	
					final ObjectInputStream input = new ObjectInputStream(socket.getInputStream());
					final ObjectOutputStream output = new ObjectOutputStream(socket.getOutputStream());
	
					Object inputObj = null;
					Request message = null, response = null;
	
					inputObj = input.readObject();
					if(inputObj instanceof Request)
						message = (Request)inputObj;
					Operation op = message.getOperation();
					System.out.println("Request received in Worker Thread: " + op);
					switch(op){
						case MAP: 
							map(message.getObjContents());
							response = new Request(Operation.SUCCESS);
							break;
						case REDUCE: 
							reduce(message.getObjContents());
							response = new Request(Operation.SUCCESS);
							break;
						case ADDTOMASTERINDEX: 
							index(message.getObjContents());
							response = new Request(Operation.SUCCESS);
							break;
						case SEARCHMASTERINDEX:
							result = search(message.getObjContents());
							response = new Request(Operation.SEARCHRESULT);
							response.addObject(result);
							break;
					}
					
					output.writeObject(response);
					try {
					input.readObject();
					}
					catch(Exception ex){ }
					socket.close();
	            }
	            catch (Exception e){
	            	e.printStackTrace();
	            }
	        }
		}
		else {
			Operation op = (Operation)(params.remove(0));
			System.out.println("Request received: " + op);
			switch(op){
				case MAP: 
					map(params);
					break;
				case REDUCE: 
					reduce(params);
					break;
				case ADDTOMASTERINDEX: 
					index(params);
					break;
				case SEARCHMASTERINDEX:
					result = search(params);
					break;
			}
		}
		// register with NameServer
		/*try {
			serverSock = new ServerSocket(port);
		}
		catch (Exception ex) {
			ex.printStackTrace();
			return;
		}
		boolean shutDown = false;
		Socket socket = null;
		while(!shutDown)
		{
			boolean proceed = true;
			try {
				socket = serverSock.accept();
				input = new ObjectInputStream(socket.getInputStream());
				output = new ObjectOutputStream(socket.getOutputStream());
				do {
					Object message = input.readObject();
					if(message instanceof Request) {
						Request req = (Request)message;
						String msg = req.getMessage();
						if(msg.equals("MAP")) {
							map(req.getObjContents());
						}
						else if(msg.equals("REDUCE")) {
							reduce(req.getObjContents());
						}
						else if(msg.equals("SEARCH")) {
							search(req.getObjContents());
						}
						else if(msg.equals("DONE")) {
							proceed = false;
						}
						else if(msg.equals("DISCONNECT")) {
							shutDown = true;
							proceed = false;
						}
					}
				} while(proceed);
			}
			catch (IOException ioe) {
				ioe.printStackTrace();
			} catch (ClassNotFoundException e) {
				e.printStackTrace();
			}
		}*/
		
	}
	
}
