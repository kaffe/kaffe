/*
 * test that Double.valueOf works as it should.
 */
public class DoubleBug {

    static boolean checkValid(String num) {
	try {
	    double d = Double.valueOf(num).doubleValue();
	    return true;
	}
	catch (NumberFormatException e) {
	    return false;
	}
    }


    static String good[] = { "0.2", "0.2f", "0.2F", "0.2d", "0.2D" };
    static String bad[] = { "0.2L", "" };
    
    public static void main(String argv[]) {
	boolean fail = false;

	for(int i = good.length; i-- > 0; ) {
	    if (checkValid(good[i]) != true)
		fail = true;
	}
	if (fail)
	    System.out.println("valid floating-point number FAIL");
	else
	    System.out.println("valid floating-point number PASS");

	fail = false;

	for(int i = bad.length; i-- > 0; ) {
	    if (checkValid(bad[i]) != false)
		fail = true;
	}
	if (fail)
	    System.out.println("invalid floating-point number FAIL");
	else
	    System.out.println("valid floating-point number PASS");
    }
}

/* Expected Output:
valid floating-point number PASS
valid floating-point number PASS
*/
