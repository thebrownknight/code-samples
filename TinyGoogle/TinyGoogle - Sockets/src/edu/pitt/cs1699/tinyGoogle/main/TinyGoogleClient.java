import java.net.*;
/* The java.io package contains the basics needed for IO operations. */
import java.io.*;
import java.util.*;

public class TinyGoogleClient {
	
	/** Define a host server */
    //private static String host = "elements.cs.pitt.edu";
	private static String host;
    /** Define a port */
    public static ArrayList<String> filesList = new ArrayList<String>();
	public static ArrayList<String> fileNameList = new ArrayList<String>();
    
	public static void main(String[] args) {

	    Socket connection = null;
	    Request indexRequest = null;
	    int port = 48321;

		BufferedReader br = null;
		try {
			String sCurrentLine;
			br = new BufferedReader(new FileReader("../serverinfo"));

			while ((sCurrentLine = br.readLine()) != null) {
				String[] testArgs = sCurrentLine.split(": ");
				if(testArgs[0].equalsIgnoreCase("TinyGoogle"))
					host = testArgs[1];
			}
				
		} catch (IOException e) {
			//e.printStackTrace();
		} finally {
			try {
				if (br != null) br.close();
			} catch (IOException ex) {
				ex.printStackTrace();
			}
		}

		if(args.length < 2 && (!args[0].equals("-index") || !args[0].equals("-search"))){
			System.out.println("Usage: -index [directory path] or -search [list of terms]");
			System.exit(0);
		}
		
		if(args.length == 2 && args[0].equals("-index")){
			int counter = 0;
			File directory = new File(args[1]);
			listFilesForFolder(directory);
			System.out.println(filesList.toString());
			for(String s : filesList){
				indexRequest = new Request(Operation.INDEX);
				indexRequest.addObject(s);
				indexRequest.addObject(fileNameList.get(counter));
				
				 try {
					 /** Obtain an address object of the server */
				    InetAddress address = InetAddress.getByName(host);
				     /** Establish a socket connection */
				      connection = new Socket(address, port);
				    
					/** Instantiate a ObjectOutputStream object to send the indexing request to the TinyGoogleServer */
					ObjectOutputStream bos = new ObjectOutputStream(connection.getOutputStream());
	      
					bos.writeObject(indexRequest);
					bos.flush();
	      					
					/** Instantiate an ObjectInputStream object to read in the information from server. **/
					ObjectInputStream bis = new ObjectInputStream(connection.getInputStream());
					processResponse(bis.readObject());
										
					try {
						connection.close();
					} catch (IOException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
				} catch (UnknownHostException e) {
					e.printStackTrace();
				} catch (IOException e) {
					e.printStackTrace();
				} catch (ClassNotFoundException e) {
					e.printStackTrace();
				}
				counter++;
			}
			try {
  				java.lang.Process process = java.lang.Runtime.getRuntime().exec("sh test.sh");	//remove all the mapped and reduced files generated
  				int retCode = process.exitValue();
			} catch (Exception e) {  }
			System.out.println("****** RESULTS ******");
			System.out.println("Indexing successfully completed.");	
		} else if (args[0].equals("-search")){
			ArrayList<String> terms = new ArrayList<String>();
			for(int k = 1; k < args.length; k++){
				terms.add(args[k]);
			}
			
			Request searchRequest = new Request(Operation.SEARCH);
		    searchRequest.addObject(terms);
		    
		    System.out.println("SEARCH REQUEST: "+getList(terms));
		    
		    try {
				/** Obtain an address object of the server */
				InetAddress address = InetAddress.getByName(host);
				/** Establish a socket connection */
				connection = new Socket(address, port);
				/** Instantiate a ObjectOutputStream object to send the indexing request to the TinyGoogleServer */
				ObjectOutputStream bos = new ObjectOutputStream(connection.getOutputStream());
      
				bos.writeObject(searchRequest);
				bos.flush();
      
				/** Instantiate an ObjectInputStream object to read in the information from server. **/
				ObjectInputStream bis = new ObjectInputStream(connection.getInputStream());
				processResponse(bis.readObject());
      
			} catch (UnknownHostException e) {
				e.printStackTrace();
			} catch (IOException e) {
				e.printStackTrace();
			} catch (ClassNotFoundException e) {
				e.printStackTrace();
			}
		    try {
				connection.close();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		

	}
	
	/** Utility method to list files given a directory **/
	public static void listFilesForFolder(final File folder) {
	    for (final File fileEntry : folder.listFiles()) {
	        if (fileEntry.isDirectory()) {
	            listFilesForFolder(fileEntry);
	        } else {
	        	if(!fileEntry.getName().equals(".DS_Store") || !fileEntry.getName().equals("glossary") || !fileEntry.getName().equals("README")){
				try{
	        			filesList.add(fileEntry.getPath().substring(0, fileEntry.getPath().lastIndexOf("/")));
					fileNameList.add(fileEntry.getName());
					System.out.println("filename..... " + fileEntry.getPath().substring(0, fileEntry.getPath().lastIndexOf("/")));
	        		} catch (Exception e) {}
			}
	        }
	    }
	}
	
	private static String getList(ArrayList<String> terms) {
		String response = "";
		for(String s : terms)
			response+=s+", ";
		return response.substring(0, response.length()-2);
	}
	
	public static void processResponse(Object inputObj) {
		Request response = null;
		if(inputObj instanceof Request)
			response = (Request)inputObj;
		if(response != null) {
			Operation op = response.getOperation();
			System.out.println("Response received: " + op);
			switch(op) {
				case SEARCHRESULT:
					System.out.println("****** RESULTS ******");
					String respString = (String) response.getObjContents().get(0);
					List<String> list = Arrays.asList(respString.substring(1, respString.length() - 1).split(", ")); 
					for (String l : list){
						System.out.println(l);
					}
				break;
			default:
				break;
			}
		}
	}

}
