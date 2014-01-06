import java.io.File;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.net.Socket;
import java.util.ArrayList;
import java.util.Collections;

public class TinyGoogleThread extends Thread {
	private TinyGoogleServer _server;
	private Socket socket;
	public TinyGoogleThread(Socket sock, TinyGoogleServer serv) {
		socket = sock;
		_server = serv;
	}
	
	private boolean index(ArrayList<Object> params) {
		if(params.size() != 2 || !(params.get(0) instanceof String) || !(params.get(1) instanceof String)) 
			return false;
		// parse parameters
		String filename = (String)(params.get(1));
		String path = (String)(params.get(0));
		
		System.out.println(path);
		// get workers from NameServer

		
		int cores = Runtime.getRuntime().availableProcessors();
		

		//File f = new File(path+"/"+filename);
		File f = new File(path);
		int size = (((int)(f.length()))/cores);

		ArrayList<Socket> workerSocks = new ArrayList<Socket>();
		Socket tempSock = null;
		for(int i=0; i<cores; i++) { // TODO: figure out how many we want
			Request req = new Request(Operation.MAP);
			req.addObject(path);
			req.addObject(filename);
			req.addObject(i);
			req.addObject(size);
			try {
				tempSock = new Socket(_server.workerServer, _server.workerPort);
				ObjectOutputStream output = new ObjectOutputStream(tempSock.getOutputStream());
				output.writeObject(req);
				workerSocks.add(tempSock);
			}
			catch(Exception e) {
				e.printStackTrace();
			}
		}

		boolean failed = !waitForSockets(workerSocks);
		workerSocks.clear();
		tempSock = null;
		
		for(int i=0; i<Split.num_splits; i++) {
			Request req = new Request(Operation.REDUCE);
			req.addObject(path);
			req.addObject(filename);
			req.addObject(i);
			req.addObject(cores);
			try {
				tempSock = new Socket(_server.workerServer, _server.workerPort);
				ObjectOutputStream output = new ObjectOutputStream(tempSock.getOutputStream());
				output.writeObject(req);
				workerSocks.add(tempSock);
			}
			catch(Exception e) {
				e.printStackTrace();
			}
		}

		failed = failed && !waitForSockets(workerSocks);
		
		for(int i=0; i<Split.num_splits; i++) {
			Request req = new Request(Operation.ADDTOMASTERINDEX);
			req.addObject(path);
			req.addObject(filename);
			req.addObject(i);
			try {
				tempSock = new Socket(_server.workerServer, _server.workerPort);
				ObjectOutputStream output = new ObjectOutputStream(tempSock.getOutputStream());
				output.writeObject(req);
				workerSocks.add(tempSock);
			}
			catch(Exception e) {
				e.printStackTrace();
			}
		}

		failed = failed && !waitForSockets(workerSocks);
		
		
		/*
		ArrayList<WorkerThread> workers = new ArrayList<WorkerThread>();
		ArrayList<ArrayList<Object>> paramLists = new ArrayList<ArrayList<Object>>();
		long startTime, endTime, duration;
		startTime = System.currentTimeMillis();
		for(int i=0; i<cores; i++) {
			paramLists.add(new ArrayList<Object>());
			paramLists.get(i).add(Operation.MAP);
			paramLists.get(i).add(filename);
			paramLists.get(i).add(path);
			paramLists.get(i).add(new Integer(i));
			paramLists.get(i).add(size);
			workers.add(new WorkerThread(paramLists.get(i)));
			workers.get(i).start();
		}
		
		try {
			for(int i=0; i<cores; i++) {
				workers.get(i).join();
			}
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		endTime = System.currentTimeMillis();
		duration = endTime - startTime;
		System.out.println("Mapping for "+filename+" took "+duration+" milis.");
		
		
		paramLists.clear();
		workers.clear();
		
		startTime = System.currentTimeMillis();
		for(int i=0; i<Split.num_splits; i++) {
			paramLists.add(new ArrayList<Object>());
			paramLists.get(i).add(Operation.REDUCE);
			paramLists.get(i).add(filename);
			paramLists.get(i).add(path);
			paramLists.get(i).add(i);
			paramLists.get(i).add(cores);
			workers.add(new WorkerThread(paramLists.get(i)));
			workers.get(i).start();
		}
		
		try {
			for(int i=0; i<Split.num_splits; i++) {
				workers.get(i).join();
			}
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		endTime = System.currentTimeMillis();
		duration = endTime - startTime;
		System.out.println("Reducing for "+filename+" took "+duration+" milis.");
		
		paramLists.clear();
		workers.clear();
		
		startTime = System.currentTimeMillis();
		for(int i=0; i<Split.num_splits; i++) {
			paramLists.add(new ArrayList<Object>());
			paramLists.get(i).add(Operation.ADDTOMASTERINDEX);
			paramLists.get(i).add(filename);
			paramLists.get(i).add(path);
			paramLists.get(i).add(i);
			workers.add(new WorkerThread(paramLists.get(i)));
			workers.get(i).start();
		}
		
		try {
			for(int i=0; i<Split.num_splits; i++) {
				workers.get(i).join();
			}
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		endTime = System.currentTimeMillis();
		duration = endTime - startTime;
		System.out.println("Adding to Master Index for "+filename+" took "+duration+" milis.");
		*/
		return failed;
	}
	
	private boolean waitForSockets(ArrayList<Socket> socks) {
		boolean result = true;
		try {
			for(Socket s : socks) {
				if(s.isClosed())
					continue;
				ObjectInputStream input = new ObjectInputStream(s.getInputStream());
				Object inputObj = null;
				Request message = null;
				
				inputObj = input.readObject();
				if(inputObj instanceof Request)
					message = (Request)inputObj;
				Operation op = message.getOperation();
				System.out.println("Request received: " + op);
				switch(op) {
					case SEARCHRESULT:
						break;
					default:
						//FAILED
						result = false;
						break;
				}
				input.close();
				s.close();
			}
				
		} catch (Exception e) {
			e.printStackTrace();
		}
		return result;
	}
	
	private String search(ArrayList<Object> params) {
		if(params.size() != 1 || !(params.get(0) instanceof ArrayList)) {
			return "Invalid parameters for SEARCH";
		}
		// parse parameters
		ArrayList<String> terms = (ArrayList<String>)(params.get(0));
		Collections.sort(terms);
		
		// get workers from NameServer
		/*ArrayList<WorkerThread> workers = new ArrayList<WorkerThread>();
		ArrayList<ArrayList<Object>> paramLists = new ArrayList<ArrayList<Object>>();
		int cores = Runtime.getRuntime().availableProcessors();*/
		ArrayList<Socket> workerSocks = new ArrayList<Socket>();
		Socket tempSock = null;
		for(int i=0; i<Split.num_splits; i++) {
			/*paramLists.add(new ArrayList<Object>());
			paramLists.get(i).add(Operation.SEARCHMASTERINDEX);
			paramLists.get(i).add(terms);
			paramLists.get(i).add(i);
			workers.add(new WorkerThread(paramLists.get(i)));
			workers.get(i).start();
			*/
			Request req = new Request(Operation.SEARCHMASTERINDEX);
			req.addObject(terms);
			req.addObject(i);
			try {
				tempSock = new Socket(_server.workerServer, _server.workerPort);
				ObjectOutputStream output = new ObjectOutputStream(tempSock.getOutputStream());
				output.writeObject(req);
				//output.close();
				workerSocks.add(tempSock);
			}
			catch(Exception e) {
				e.printStackTrace();
			}
		}
		ArrayList<String> result = new ArrayList<String>();
		try {
			/*for(int i=0; i<Split.num_splits; i++) {
				workers.get(i).join();
				temp = workers.get(i).getResult();
				if(temp != null)
					result.addAll(temp);
			}*/
			for(Socket s : workerSocks) {		//loop through the worker sockets created above when the request to search the master index was made
				ObjectInputStream input = new ObjectInputStream(s.getInputStream());		//obtain the input stream for the socket
				Object inputObj = null;
				Request message = null;

				inputObj = input.readObject();		//from the worker thread, an object is returned containing an ArrayList<String> of the lines returned by the search
				if(inputObj instanceof Request)
					message = (Request)inputObj;
				Operation op = message.getOperation();
				System.out.println("Request received: " + op);
				switch(op) {
					case SEARCHRESULT:
						Object tempObj = message.getObjContents().get(0);	//should only be returning one line with the term, and doc, frequency
						if(tempObj != null && tempObj instanceof ArrayList){
							result.addAll((ArrayList<String>)(tempObj));		//add the line to the result ArrayList and loop through the sockets doing the same for each
						}
					break;
				}
				input.close();
				//output.writeObject(new Request(Operation.SUCCESS));
				//output.close();
				s.close();
			}
				
		} catch (Exception e) {
			e.printStackTrace();
		}
		System.out.println("RESULTS: " + result.toString());
		return combine(result, terms.size(), true);
	}
	
	private String combine(ArrayList<String> records, int terms, boolean and) {
		
		String result = "";
		/*
		ArrayList<String> docs = new ArrayList<String>();
		ArrayList<Integer> freqs = new ArrayList<Integer>();
		ArrayList<Integer> counters = new ArrayList<Integer>();
		String [] vals, pairs, tuple;
		for(String res : records) {
			vals = res.split("[|]");
			// dummy-proofing it
			if(vals.length == 2) {
				pairs = vals[1].split(";");
				// build the map of <document, frequency> value pairs
				for(String pair : pairs) {
					tuple = pair.split(",");
					if(tuple.length == 2) {
						int index = docs.indexOf(tuple[0]);
						if(index == -1) {
							docs.add(tuple[0]);
							freqs.add(Integer.parseInt(tuple[1]));
							counters.add(1);
						}
						else {
							freqs.set(index, freqs.get(index) + (Integer.parseInt(tuple[1])));
							counters.set(index, counters.get(index) + 1);
						}
					}
				}
			}
		}
		System.out.println("SECOND SET OF COUNTERS: " + terms);

		for(int i=0; i<docs.size(); i++) {
			//if(!and || (and && counters.get(i) == terms)) {
				result += docs.get(i) +","+freqs.get(i)+"\n";
			//}
		}
		System.out.println("SECOND SET OF RESULTS: " + result);
		*/
		result = records.toString();
		return result;
	}

	public void run() {
		try
		{
			//Announces connection and opens object streams
			System.out.println("*** New request from " + socket.getInetAddress() + ":" + socket.getPort() + "***");
			final ObjectInputStream input = new ObjectInputStream(socket.getInputStream());
			final ObjectOutputStream output = new ObjectOutputStream(socket.getOutputStream());

			Object inputObj = null;
			Request message = null, response = null;

			inputObj = input.readObject();
			if(inputObj instanceof Request)
				message = (Request)inputObj;
			Operation op = message.getOperation();
			System.out.println("Request received: " + op);
			switch(op) {
				case INDEX:
					index(message.getObjContents());
					response = new Request(Operation.SUCCESS);
					break;
				case SEARCH:
					String result = search(message.getObjContents());
					response = new Request(Operation.SEARCHRESULT);
					response.addObject(result);
					break;
			}
			output.writeObject(response);
		}
		catch(Exception e)
		{
			System.err.println("Error: " + e.getMessage());
			e.printStackTrace(System.err);
		}
	}
}
