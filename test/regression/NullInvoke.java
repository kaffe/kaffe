/**
 * Test case for PR#625.
 *
 * Submitted by Feng Qian <fqian@sable.mcgill.ca>.
 * Different implementation by Edouard G. Parmelan <egp@free.fr>
 */
public class NullInvoke {
    public static class InvokeSpecial {
	private String getName() {
	    return "InvokeSpecial";
	}

	public void printName(InvokeSpecial obj) {
	    String s = obj.getName();	// invokespecial with null
	    System.out.println(s);
	}
    }

    public static class InvokeVirtual {
	public String getName() {
	    return "InvokeVirtual";
	}

	public void printName(InvokeVirtual obj) {
	    String s = obj.getName();	// invokevirtual with null
	    System.out.println(s);
	}
    }

    public static class InvokeFinalVirtual {
	public final String getName() {
	    return "InvokeFinalVirtual";
	}

	public void printName(final InvokeFinalVirtual obj) {
	    String s = obj.getName();	// invokespecial with null
	    System.out.println(s);
	}
    }

    public static interface Interface {
	public String getName();
    }

    public static class InvokeInterface
	implements Interface
    {
	public String getName() {
	    return "InvokeInterface";
	}

	public void printName(Interface obj) {
	    String s = obj.getName();	// invokeinterface with null
	    System.out.println(s);
	}
    }

    public static void main (String[] args) {
	try {
	    new InvokeSpecial().printName(null);
	    System.out.println ("FAIL");
	}
	catch (NullPointerException npe) {
	    System.out.println ("PASS");
	}

	try {
	    new InvokeVirtual().printName(null);
	    System.out.println ("FAIL");
	}
	catch (NullPointerException npe) {
	    System.out.println ("PASS");
	}

	try {
	    new InvokeFinalVirtual().printName(null);
	    System.out.println ("FAIL");
	}
	catch (NullPointerException npe) {
	    System.out.println ("PASS");
	}

	try {
	    new InvokeInterface().printName(null);
	    System.out.println ("FAIL");
	}
	catch (NullPointerException npe) {
	    System.out.println ("PASS");
	}
    }
}

/* Expected Output:
PASS
PASS
PASS
PASS
*/
