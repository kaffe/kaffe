/*
 * GCTest.java
 *
 * Test that spawns a lot of threads, have each thread allocate various 
 * objects.  Then the gc is invoked, and afterwards the objects are checked
 * to make sure they didn't get freed or munged.  This test helped discover
 * two race conditions in the garbage collector.
 *
 * Courtesy Pat Tullmann (tullmann@cs.utah.edu)
 */
public class GCTest 
	implements Runnable
{
	class GCTest_Object
	{
		public GCTest_Object(int id_, GCTest_Object next_, String name_)
		{
			id = id_;
			next = next_;
			name = name_;
		}

		public int id;
		public GCTest_Object next;
		public String name;

		public void finalize()
		{
			id = -1;
			next = null;
			name = null;
		}
	}

	private static int ct_ = 113;
	private static boolean exitOnFailure_ = false;

	private int id_;

	public GCTest(int id)
	{
		id_ = id;
		
		Thread th = new Thread(this);
		th.start();
	}
	

	public static void main(String[] args)
	{
		int i;
		int thCt;

		// when run as part of testsuite, set some defaults
		thCt = 45;
		ct_ = 60;

		if (args.length < 2)
		{
			// in interactive use make this true
			if (false) {
				System.out.println("Usage: GCTest " 
				    + "<thread count> <block size (count)>");
				System.exit(1);
			} 
		} else {

			thCt = Integer.parseInt(args[0]);
			ct_ = Integer.parseInt(args[1]);

		}
		
		for (i = 0; i < thCt; i++)
			new GCTest(i);
	}

	public void run()
	{
		// Test various stack references
		try
		{
			testObj();
			// out("obj Success");
		}
		catch (Throwable t)
		{
			t.printStackTrace();
			failure("testObj: Caught exception: " + t.getMessage());
		}

		try
		{
			testPrimArray();
			// out("primarray Success");
		}
		catch (Throwable t)
		{
			t.printStackTrace();
			failure("testPrimArray: Caught exception: " + t.getMessage());
		}

		try
		{
			testObjArray();
			// out("objarray Success");
		}
		catch (Throwable t)
		{
			t.printStackTrace();
			failure("testObjArray: Caught exception: " + t.getMessage());
		}

		try
		{
			testArrayOfArray();
			// out("arrayofarray Success");
		}
		catch (Throwable t)
		{
			t.printStackTrace();
			failure("testArrayOfArray: Caught exception: " + t.getMessage());
		}

		try
		{
			testObjChain();
			// out("objChain Success");
		}
		catch (Throwable t)
		{
			t.printStackTrace();
			failure("testObjChain: Caught exception: " + t.getMessage());
		}
		out ("Success");
	}
	
	public void gc() 
	{
	    if (true) {
		// out("invoking gc");
		System.gc();

		// Sleep to see if the finalizer gets invoked..
		try
		{
			Thread.sleep(1);
		} 
		catch (Exception e)
		{
			out("sleep failure");
			System.exit(13);
		}
	    }
	}
	
	public void testObj()
	{
		Integer i = new Integer(42);
		gc();
		if (i.intValue() != 42)
			failure("testObj");
	}
	
	public void testPrimArray()
	{
		int[] intArray = new int[ct_];
		int i;
		
		for (i = 0; i < ct_; i++)
			intArray[i] = i;
		
		gc();
		
		for (i = 0; i < ct_; i++)
			if (intArray[i] != i)
				failure("testPrimArray: wanted " +i+ "; got " +intArray[i]+ ".");
	}
	
	public void testObjArray()
	{
		String[] strs = new String[ct_];
		int i;
		
		for (i = 0; i < ct_; i++)
			strs[i] = new String(Integer.toString(i));
		
		gc();
		
		for (i = 0; i < ct_; i++)
		{
			String cmp = new String(Integer.toString(i));
			if (!strs[i].equals(cmp))
				failure("testObjArray: wanted " +cmp+ "; got " +strs[i]+ ".");
		}
	}
			
	public void testArrayOfArray()
	{
		int[][] intArray = new int[ct_][ct_];
		int[][] intArray2 = new int[ct_][];
		int i, j;

		// test 1
		for (i = 0; i < ct_; i++)
			for (j = 0; j < ct_; j++)
				intArray[i][j] = id_;
		
		gc();
		
		for (j = 0; j < ct_; j++)
			for (i = 0; i < ct_; i++)
				if (intArray[i][j] != id_)
					failure("testArrayOfArray(1): " +intArray+ "[" +i+ "][" +j+ "].  Got " +intArray[i][j]+ "; expected " +id_+ ".");

		
		gc();

		// test 2

		intArray = new int[ct_][];
		for (i = 0; i < ct_; i++)
		{
			intArray[i] = intArray2[i] = new int[ct_];
			// out(intArray[i].toString());
			for (j = 0; j < ct_; j++)
				intArray[i][j] = id_;
		}
		
		gc();
		
		for (j = 0; j < ct_; j++) {
			if (intArray[j] != intArray2[j])
				failure("testArrayOfArray(3): " +intArray+ "[" +j+ "].  Got " +intArray[j]+ "; expected " +intArray2[j]+ ".");
			for (i = 0; i < ct_; i++)
				if (intArray[i][j] != id_)
					failure("testArrayOfArray(2): " +intArray+ "[" +i+ "][" +j+ "].  Got " +intArray[i][j]+ "; expected " +id_+ ".");
		}
	}

	public void testObjChain()
	{
		GCTest_Object head, next;
		int i;
		
		head = new GCTest_Object(0, null, "0");
		next = head;
		
		for (i = 1; i < 100; i++)
		{
			next.next = new GCTest_Object(i, null, Integer.toString(i));
			next = next.next;
		}

		gc();

		next = head;
		for (i = 0; i < 99; i++)
		{
			if ((next.id != i)
			    || (next.next == null)
			    || (!next.name.equals(Integer.toString(i))))
				failure("testObjChain at " +i+ "(0x" +next.hashCode()+ "):"
					+ " id is " +next.id+ " (should be " +i+ ");" 
					+ " name is " +next.name+ " (should be '" +i+ "').");
			next = next.next;
		}
		
		if ((next.id != 99)
		    || (next.next != null)
		    || (!next.name.equals("99")))
			failure("testObjChain at 99");
	}

	public void failure(String msg)
	{
		out("Failure: " +msg);

		if (exitOnFailure_)
			System.exit(11);
	}

	public void out(String msg)
	{
	    synchronized (GCTest.class) {
		System.out.println("[" +id_+ "]:  " +msg);
	    }
	}
}

// Sort output
/* Expected Output:
[0]:  Success
[10]:  Success
[11]:  Success
[12]:  Success
[13]:  Success
[14]:  Success
[15]:  Success
[16]:  Success
[17]:  Success
[18]:  Success
[19]:  Success
[1]:  Success
[20]:  Success
[21]:  Success
[22]:  Success
[23]:  Success
[24]:  Success
[25]:  Success
[26]:  Success
[27]:  Success
[28]:  Success
[29]:  Success
[2]:  Success
[30]:  Success
[31]:  Success
[32]:  Success
[33]:  Success
[34]:  Success
[35]:  Success
[36]:  Success
[37]:  Success
[38]:  Success
[39]:  Success
[3]:  Success
[40]:  Success
[41]:  Success
[42]:  Success
[43]:  Success
[44]:  Success
[4]:  Success
[5]:  Success
[6]:  Success
[7]:  Success
[8]:  Success
[9]:  Success
*/
