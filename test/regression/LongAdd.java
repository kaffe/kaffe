
// This test was originally added because of a jikes compiler bug
// (kaffe PR #523). The compiler would optimize the addition below
// and put an incorrect result of 0x80000000 in the class file.

public class LongAdd {
  public static void main(String[] args) {
    System.out.println(Long.toHexString( 0x100000000L + 0x80000000 ));
  }
}

/* Expected Output:
180000000
*/

