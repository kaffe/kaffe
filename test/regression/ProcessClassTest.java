/**
 * check some more class loading etc, this time with superclasses
 *
 * test written by Edouard Parmelan <Edouard.Parmelan@France.NCR.COM>
 */
public class ProcessClassTest
{
    public static void test(final String tag, final String class_name) {
	Thread thread = new Thread() {
	    public void run() {
		setName(tag);
		try {
		    Class c = Class.forName(class_name);
		    Object o = c.newInstance();
		    System.out.println(getName() + " " + class_name + " " + o);
		}
		catch (Throwable t) {
		    System.out.println(getName() + " " + class_name + " " + t);
		    //t.printStackTrace();
		}
	    }
	};

	thread.start();
	try {
	    synchronized(thread) {
		thread.wait();
	    }
	}
	catch (InterruptedException t) {
	    System.out.println(thread.getName() + " " + class_name + " " + t);
	}
    }

    public static void main(String args[]) {
	new java.io.File("DontExist.class").delete();
	test("A", "Segv");
	test("B", "First");
	test("C", "Second");
	test("D", "Third");

	test("E", "Segv");
	test("F", "First");
	test("G", "Second");
	test("H", "Third");
    }
}

class Segv {
    public Segv() {
	Object segv = null;
	segv.hashCode();
    }
}

class First {
    static boolean inited;
    static {
	inited = false;
	new Segv();
	inited = true;
    }
    public String toString() {
	if (inited)
	    return new String("pass");
	else
	    return new String("fail");
    }
}

class DontExist {
}

class Second extends DontExist {
    static boolean inited;
    static {
	inited = false;
	inited = true;
    }
    public String toString() {
	if (inited)
	    return new String("pass");
	else
	    return new String("fail");
    }
}

class Third {
    static boolean inited;
    static {
	inited = false;
	new DontExist();
	inited = true;
    }
    public String toString() {
	if (inited)
	    return new String("pass");
	else
	    return new String("fail");
    }
}
