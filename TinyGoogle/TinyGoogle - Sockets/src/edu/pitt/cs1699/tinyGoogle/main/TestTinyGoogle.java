import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.ArrayList;

public class TestTinyGoogle {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		//TinyGoogleServer server = new TinyGoogleServer(48321);
		//server.start();

		Socket sock;
		try {
			
		
		ObjectOutputStream output;
		ObjectInputStream input;
	    
	    Request indexRequest = new Request(Operation.INDEX);
	    String filename = "hamlet";
	    indexRequest.addObject(filename);
	    indexRequest.addObject("/Users/Venkatesh/Downloads/shakespeare/tragedies");
	    
	    Request searchRequest = new Request(Operation.SEARCH);
	    ArrayList<String> terms = new ArrayList<String>();
		terms.add("REDEMPTION");
		terms.add("PLEASE");
		terms.add("COUNTRY");
		terms.add("DEATH");
		terms.add("SUPPORT");
		terms.add("MOUSE");
		terms.add("SOLDIER");
		terms.add("CITIZEN");
		terms.add("ROBB");
	    searchRequest.addObject(terms);
	    
	    
	    sock = new Socket("localhost", 48321);
	    output = new ObjectOutputStream(sock.getOutputStream());
	    input = new ObjectInputStream(sock.getInputStream());
		System.out.println("INDEX REQUEST: "+filename);
	    output.writeObject(indexRequest);
	    processResponse(input.readObject());
	    sock.close();
	    
	    
	    sock = new Socket("localhost", 48321);
	    output = new ObjectOutputStream(sock.getOutputStream());
	    input = new ObjectInputStream(sock.getInputStream()); 
	    System.out.println("SEARCH REQUEST: "+getList(terms));
	    output.writeObject(searchRequest);
	    processResponse(input.readObject());
		sock.close();
	    
		} catch (UnknownHostException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		} catch (ClassNotFoundException e) {
			e.printStackTrace();
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
					System.out.println((String)(response.getObjContents().get(0)));
				break;
			default:
				break;
			}
		}
	}

}
