/* test that we don't get tripped up by x86 problem */

public class divtest {

    public static int divfunc(int x, int y) {
      return x/y;
    }

    public static void main(String args[]) {

      int i = 0x80000000;
      int j = -1;

      int k = divfunc(i,j);	// x86 causes trap for no good reason
      System.out.println("Result is "+Integer.toHexString(k));
    }
}
/* Expected Output:
Result is 80000000
*/
