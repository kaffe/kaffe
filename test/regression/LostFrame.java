/**
 * This test makes sure we catch a null pointer exception in the frame
 * where it was thrown if we're asked to.
 *
 * @author Edouard Parmelan <Edouard.Parmelan@France.NCR.COM>
 */
public class LostFrame {
    static void segv() {
	try {
	    ((Object)null).hashCode();
	}
	catch (NullPointerException ex) {
	    System.out.println("PASS");
	}
    }

    static void test() {
	try {
	    segv();
	}
	catch (NullPointerException ex) {
	    System.out.println("FAIL");
	}
    }


    public static void main(String args[]) {
	test();
    }
}
