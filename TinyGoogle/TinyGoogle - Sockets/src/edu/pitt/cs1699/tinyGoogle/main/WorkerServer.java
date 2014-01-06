import java.io.*;
import java.net.*;
import java.util.ArrayList;
import java.util.concurrent.ArrayBlockingQueue;

public class WorkerServer {
	private int processors;
	private ArrayList<WorkerThread> workerThreads;
	protected ArrayBlockingQueue<Socket> connectionQueue;
	private static final int QUEUE_CAPACITY = 25;
	private int port;
	private ServerSocket listener;
	private Socket connection;
	
	public WorkerServer(int port) {
		this(port, Runtime.getRuntime().availableProcessors());
	}
	
	public WorkerServer(int port, int processors) {
		this.port = port;
		this.processors = processors;
		this.workerThreads = new ArrayList<WorkerThread>();	
		this.connectionQueue = new ArrayBlockingQueue<Socket>(QUEUE_CAPACITY);
	}
	
	public void start() {
		// register with name server
		
		
		try {
			for(int i=0; i<processors-1; i++) {
				workerThreads.add(new WorkerThread(this));
			}
			
			listener = new ServerSocket(port);
			System.out.println("Worker Server established on port "+port+" with "+(processors-1)+" workers.");
		
			InetAddress ip = InetAddress.getLocalHost();
                        String content = "Workers: " + ip.getHostAddress();
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
                        System.out.println("Wrote workers server ip info: " + content);
			
			while (true) {

				connection = listener.accept();
				try {
					connectionQueue.put(connection); // Blocks if queue is full.
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
			}
         } catch (IOException e1) {
			e1.printStackTrace();
         }
	}
	
	public int getAvailableProcessors() {
		return processors;
	}

	
	public static void main(String [] args) {
		WorkerServer server = new WorkerServer(8765, 8);
		server.start();
		
	}
}
