/**
 * This test check LNEG before GOTO and before LRETURN.
 * Bug found with KJC with i386.
 *
 * @author Edouard G. Parmelan <edouard.parmelan@quadratec.fr>
 */
public class LongNeg {
    static long lneg(boolean neg, long v) {
        return neg ? v : -v;
    }

    static long not_lneg(boolean neg, long v) {
        return (!neg) ? -v : v;
    }

    public static void main(String args[]) {
        System.out.println(lneg(true, -10));
        System.out.println(lneg(false, -10));
        System.out.println(not_lneg(true, -10));
        System.out.println(not_lneg(false, -10));
    }
}

/* Expected Output:
-10
10
-10
10
*/
