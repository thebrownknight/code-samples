import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.*;

public class TinyGoogleServer {
	private int port, processors, workers;
	private ServerSocket serverSock;
	
	//protected String workerServer = "136.142.227.9";
	protected String workerServer = null;
	protected int workerPort = 8765;
	
	public TinyGoogleServer(int port) {
		this.port = port;
	}
	
	public int getAvailableProcessors() {
		return processors;
	}
	
	public int getAvailableWorkers() {
		return workers;
	}
	
	public void start() {
		Runtime runtime = Runtime.getRuntime();
		runtime.addShutdownHook(new ShutDownListener(this));
		// register with NameServer
		
		// get WorkerServers from NameServer
		BufferedReader br = null;
                try {
                        String sCurrentLine;
                        br = new BufferedReader(new FileReader("../serverinfo"));

                        while ((sCurrentLine = br.readLine()) != null) {
                                String[] testArgs = sCurrentLine.split(": ");
                                if(testArgs[0].equalsIgnoreCase("Workers"))
                                        workerServer = testArgs[1];
                        }

                } catch (IOException e) {
                        e.printStackTrace();
                } finally {
                        try {
                                if (br != null) br.close();
                        } catch (IOException ex) {
                                ex.printStackTrace();
                        }
                }
	
		
		//This block listens for connections and creates threads on new connections
		try
		{
			
			ServerSocket serverSock = new ServerSocket(port);
			System.out.println("Server started on port "+port);

			InetAddress ip = InetAddress.getLocalHost();
			String content = "TinyGoogle: " + ip.getHostAddress();
			File pubFile = new File("../serverinfo");
			FileWriter fw = null;
			BufferedWriter bw = null;
                        if (!pubFile.exists()) {
                                pubFile.createNewFile();
                                fw = new FileWriter(pubFile.getAbsoluteFile());
				bw = new BufferedWriter(fw);
                        	bw.write(content);
                        } else {
                                fw = new FileWriter(pubFile.getAbsoluteFile(), true);
				bw = new BufferedWriter(fw);
                        	bw.write("\n" + content);
                        }
			bw.close();

			System.out.println("Wrote TinyGoogle server ip info: " + content);
			Socket sock = null;
			TinyGoogleThread thread = null;
			
			while(true)
			{
				sock = serverSock.accept();
				System.out.println("Client connected on port " + port);
				thread = new TinyGoogleThread(sock, this);
				thread.start();
			}
		}
		catch(Exception e)
		{
			System.err.println("Error: " + e.getMessage());
			e.printStackTrace(System.err);
		}
	}
	
	public static void main(String [] args) {
		TinyGoogleServer server = new TinyGoogleServer(48321);
		server.start();
	}
	
	public void shutdown() {
		try {
			if(serverSock != null)
				serverSock.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
}
// this thread nicely shuts down server
class ShutDownListener extends Thread
{
	public TinyGoogleServer my_tgs;
	
	public ShutDownListener (TinyGoogleServer _tgs) {
		my_tgs = _tgs;
	}
	
	public void run()
	{
		System.out.println("Shutting down server");
		try {
                	java.lang.Process process = java.lang.Runtime.getRuntime().exec("rm ../serverinfo");
                        int retCode = process.exitValue();
                } catch (Exception e) {  }
		my_tgs.shutdown();
	}
}
