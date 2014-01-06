public class Utils {
	//public static final String masterIndexPath = "C:\\Users\\Services\\Documents\\Dropbox\\CS 1699 - Cloud Computing\\Documents\\MasterIndex";
	public static final String masterIndexPath = "/Users/Hadoop/Desktop/TinyGoogle/TinyGoogle - Sockets/src/edu/pitt/cs1699/tinyGoogle/MasterIndex";
	
	public static String getMappedPath(String path, String filename, String tag, int block) {
		return path+"/"+filename+"_mapped_"+tag+"_"+block+".txt";
	}

	public static String getReducedPath(String path, String filename, String tag) {
		return path+"/"+filename+"_reduced_"+tag+".txt";
	}
	
	public static String getMasterPath(String tag) {
		return masterIndexPath+"/master_index_"+tag+".txt";
	}
}
