public class TestSplits {

	public static void main(String[] args) {
		String [] strs = {"Apple", "Banana", "Cherry", "Date", 
				"Elderberry", "Fig", "Grape", "Huckleberry", "Ivy", 
				"Jujube", "Kiwi", "Lemon", "Melon", "Nutmeg", "Orange", 
				"Plum", "Quince", "Raspberry", "Strawberry", "Tangerine",
				"Ugni", "Vanilla", "Watermelon", "Xfruit", "Yfruit", "Zfruit"};
		
		for(String s : strs) {
			Split sp = Split.getSplit(s.toUpperCase());
			System.out.println(s+" "+sp + (sp == null ? "" : " "+sp.tag()));
		}

	}

}
