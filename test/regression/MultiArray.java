/* This file has been contributed b Mark Wielaard <mark@klomp.org> */
public class MultiArray
{
  static final char[] initCharArray =
      new char[] {'\u0000', '\u0000', '\u0000', '\u0000', '\u0000',
'\u0000'};
  public static final char[][][][][] charArray_length =
      new char[1][1][1][1][];

  public static void main(String[] args)
  {
    charArray_length[0][0][0][0] = initCharArray;
    System.out.println("Success.");
  }
}
/* Expected output:
Success.
*/
