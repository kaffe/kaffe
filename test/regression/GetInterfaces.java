interface if0 {
}

interface if1 {
}

class cl implements if0, if1 {
}

class GetInterfaces {
    public static void main(String argv[]) {
	try {
	    Class c = Class.forName("cl");
	    Class interfaces[] = c.getInterfaces();
	    // It's all done allready :-(
	    System.out.println("Hello world!");
	    for(int i=0; i<interfaces.length; i++) {
		System.out.println("interface" + i + ": " + interfaces[i].getName());
	    }
	} catch (Exception e) {
	    System.out.println("Exception caught: " + e.toString());
	}
    }
}

/* Expected Output:
Hello world!
interface0: if0
interface1: if1
*/
