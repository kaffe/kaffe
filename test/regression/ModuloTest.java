
// Reference: http://java.sun.com/docs/books/jls/html/15.doc.html#239829

class ModuloTest {
    static void mod(long a, long b) {
	long ab = a % b;
	long ba = b % a;
	System.out.println(a + " % " + b + " = " + ab);
	System.out.println(b + " % " + a + " = " + ba);
    }
    static public void main(String args[]) {
	mod(-1111123, -1123);
	mod(11123, -123);
	mod(-1111123, 1123);
	mod(java.lang.Long.MIN_VALUE, -1);
    }
}


/* Expected Output:
-1111123 % -1123 = -476
-1123 % -1111123 = -1123
11123 % -123 = 53
-123 % 11123 = -123
-1111123 % 1123 = -476
1123 % -1111123 = 1123
-9223372036854775808 % -1 = 0
-1 % -9223372036854775808 = -1
*/
