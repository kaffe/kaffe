/**
 * test that exceptions in finalizers are indeed ignored.
 *
 * @author Godmar Back <gback@cs.utah.edu>
 */
public class finalexc {
    static int f;
    public void finalize() {
	f++;
	((String)null).hashCode();
    }
    public static void main(String av[]) {
	for (int i = 0; i < 100; i++) {
	    new finalexc();
	    System.gc();
	}
	if (f > 0) {
	    System.out.println("Success.");
	}
    }
}

/* Expected Output:
Success.
*/

