import java.io.File;
import java.util.ArrayList;
import java.util.Collections;


public class TestWorkerThread {
	
	public static void main(String [] args) {
		ArrayList<WorkerThread> workers = new ArrayList<WorkerThread>();
		ArrayList<ArrayList<Object>> params = new ArrayList<ArrayList<Object>>();
		//int cores = Runtime.getRuntime().availableProcessors();
		long startTime, endTime, duration;
		
		//String filename = "othello";
		//String filename = "Miserables.txt";
		//String path = "C:\\Users\\Services\\Documents\\Dropbox\\CS 1699 - Cloud Computing\\Documents\\Shakespeare\\comedies";
		//String path = "C:\\Users\\Services\\Documents\\Dropbox\\CS 1699 - Cloud Computing\\Documents\\Hugo";
		//String path = "/Users/Venkatesh/Downloads/shakespeare/tragedies";
		//File f = new File(path+"/"+filename);
		//int size = (((int)(f.length()))/cores);

		//startTime = System.currentTimeMillis();
		//for(int i=0; i<cores; i++) {
			//params.add(new ArrayList<Object>());
			//params.get(i).add(Operation.MAP);	//add which operation you are doing
			//params.get(i).add(filename);		//add the filename that you are trying to map
			//params.get(i).add(path);			//add the path of the file you are trying to map
			//params.get(i).add(new Integer(i));	//add the block (using the number of cores on virtual machine) you are using to perform the map
			//params.get(i).add(size);			//add the predetermined size that each core should be responsible for in computing
			//workers.add(new WorkerThread(params.get(i)));
			//workers.get(i).start();
		//}
		
		/*
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
		
		params.clear();
		workers.clear();
		
		startTime = System.currentTimeMillis();
		for(int i=0; i<Split.num_splits; i++) {
			params.add(new ArrayList<Object>());
			params.get(i).add(Operation.REDUCE);
			params.get(i).add(filename);
			params.get(i).add(path);
			params.get(i).add(i);
			params.get(i).add(cores);
			workers.add(new WorkerThread(params.get(i)));
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
		
		params.clear();
		workers.clear();
		
		startTime = System.currentTimeMillis();
		for(int i=0; i<Split.num_splits; i++) {
			params.add(new ArrayList<Object>());
			params.get(i).add(Operation.ADDTOMASTERINDEX);
			params.get(i).add(filename);
			params.get(i).add(path);
			params.get(i).add(i);
			workers.add(new WorkerThread(params.get(i)));
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
		params.clear();
		workers.clear();
		ArrayList<String> terms = new ArrayList<String>();
		terms.add("LOVE");
		terms.add("PLEASE");
		terms.add("COUNTRY");
		terms.add("DEATH");
		terms.add("SUPPORT");
		terms.add("MOUSE");
		Collections.sort(terms);
		startTime = System.currentTimeMillis();
		for(int i=0; i<Split.num_splits; i++) {
			params.add(new ArrayList<Object>());
			params.get(i).add(Operation.SEARCHMASTERINDEX);
			params.get(i).add(terms);
			params.get(i).add(i);
			workers.add(new WorkerThread(params.get(i)));
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
		System.out.println("Searching Master Index for "+terms.size()+" terms took "+duration+" milis.");
	}
}
