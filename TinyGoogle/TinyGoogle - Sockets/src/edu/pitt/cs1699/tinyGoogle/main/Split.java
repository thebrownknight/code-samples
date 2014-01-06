public enum Split {
    SPLIT0 (0, "A", "DZ"),
    SPLIT1 (1, "E", "HZ"),
    SPLIT2 (2, "I", "LZ"),
    SPLIT3 (3, "M", "NZ"),
    SPLIT4 (4, "O", "QZ"),
    SPLIT5 (5, "R", "SZ"),
    SPLIT6 (6, "T", "VZ"),
    SPLIT7 (7, "W", "ZZ");

    private final String beginning;
    private final String ending;
    private final String tag;
    private final int num;
    public static final int num_splits = 8;
    
    Split(int num, String start, String end) {
    	this.num = num;
        this.beginning = start;
        this.ending = end;
        this.tag = start+"-"+end;
    }
    public String tag() {
    	return tag;
    }
    public String start() {
    	return beginning;
    
    }
    public String end() {
    	return ending;
    }
    
    public int num() {
    	return num;
    }
    
    public boolean contains(String str) {
    	return beginning.compareTo(str) <= 0 && ending.compareTo(str) >=0;
    }
    
    public static Split getSplit(String str) {
    	if(SPLIT0.contains(str))
    		return SPLIT0;
    	if(SPLIT1.contains(str))
    		return SPLIT1;
    	if(SPLIT2.contains(str))
    		return SPLIT2;
    	if(SPLIT3.contains(str))
    		return SPLIT3;
    	if(SPLIT4.contains(str))
    		return SPLIT4;
    	if(SPLIT5.contains(str))
    		return SPLIT5;
    	if(SPLIT6.contains(str))
    		return SPLIT6;
    	if(SPLIT7.contains(str))
    		return SPLIT7;
    	return null;
    }
    
    public static Split getSplit(int i) {
    	if(i == 0)
    		return SPLIT0;
    	if(i == 1)
    		return SPLIT1;
    	if(i == 2)
    		return SPLIT2;
    	if(i == 3)
    		return SPLIT3;
    	if(i == 4)
    		return SPLIT4;
    	if(i == 5)
    		return SPLIT5;
    	if(i == 6)
    		return SPLIT6;
    	if(i == 7)
    		return SPLIT7;
    	return null;
    }

}
