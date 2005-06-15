/* This testcase is taken from
 * http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=255502
 */
import java.lang.*;

public class DoubleIEEE {
    volatile double x, y, z;

    public void doTest()
    {
        x = 9007199254740994.0; /* 2^53 + 2 */
        y = 1.0 - 1/65536.0;
        z = x + y;
        System.out.println("z = " + z);
    }
	
    public static void main(String[] args) throws Exception {
	(new DoubleIEEE()).doTest();
    }
}
/* Expected output:
z = 9.007199254740994E15
*/
