/* Although this appears to be testing CharToByteDefault, it is
   actually a test for sysdepCallMethod.  Since
   CharToByteDefault.convert takes 6 arguments, plus the implicit
   `this' and the JNIEnv, and several architectures can pass at most 6
   arguments on registers, this will end up testing whether the last
   two arguments are passed correctly */

import kaffe.io.CharToByteDefault;

public class sysdepCallMethod {
    public static void main(String[] argv) {
	char[] ca = { 'a', 'b', 'c' };
	byte[] ba = new byte[ca.length*2];
	CharToByteDefault cb = new kaffe.io.CharToByteDefault();
	cb.convert(ca, 0, ca.length, ba, 0, ba.length);
	for (int i = 0; i < ba.length; ++i)
	    System.out.println(ba[i]);
    }
}

/* Expected output:
97
98
99
0
0
0
*/
