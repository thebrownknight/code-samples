import java.util.ArrayList;


public class Request implements java.io.Serializable {
	
	/**
	 * 
	 */
	private static final long serialVersionUID = -7726335089122193103L;
	private Operation msg;
	private ArrayList<Object> objContents = new ArrayList<Object>();
	
	public Request(Operation text)
	{
		msg = text;
	}
	
	public Operation getOperation()
	{
		return msg;
	}
	
	public ArrayList<Object> getObjContents()
	{
		return objContents;
	}
	
	public void addObject(Object object)
	{
		objContents.add(object);
	}

}
