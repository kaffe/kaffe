/**
 * This test check DNEG and LNEG with 0.0.
 *
 * @author Edouard G. Parmelan <edouard.parmelan@quadratec.fr>
 */
public class DoubleNeg {
    static double dneg(boolean neg, double v) {
        return neg ? v : -v;
    }

    static double not_dneg(boolean neg, double v) {
        return (!neg) ? -v : v;
    }

    static float fneg(boolean neg, float v) {
        return neg ? v : -v;
    }

    static float not_fneg(boolean neg, float v) {
        return (!neg) ? -v : v;
    }

    public static void main(String args[]) {
        System.out.println(dneg(true, -10.0));
        System.out.println(dneg(false, -10.0));
        System.out.println(not_dneg(true, -10.0));
        System.out.println(not_dneg(false, -10.0));
        System.out.println(dneg(true, 0.0));
        System.out.println(dneg(false, 0.0));

        System.out.println(fneg(true, -10.0f));
        System.out.println(fneg(false, -10.0f));
        System.out.println(not_fneg(true, -10.0f));
        System.out.println(not_fneg(false, -10.0f));
        System.out.println(fneg(true, 0.0f));
        System.out.println(fneg(false, 0.0f));
    }
}

/* Expected Output:
-10.0
10.0
-10.0
10.0
0.0
-0.0
-10.0
10.0
-10.0
10.0
0.0
-0.0
*/
