class Str2 {
	public static void main (String args[]) {
		String a = "1";
		a += "2";
		System.out.println(a);
		System.out.println("abc".indexOf("", -99999999));
		System.out.println("abc".indexOf("", 99999999));
		System.out.println("".indexOf("a"));
		System.out.println("".indexOf(""));
		System.out.println("abc".lastIndexOf("", -99999999));
		System.out.println("abc".lastIndexOf("", 99999999));
		System.out.println("".lastIndexOf("a"));
		System.out.println("".lastIndexOf(""));

		substring();
	}

	/** Substring match.
	 *  @author Peter Graves <peter@armedbear.org>
	 */
	public static void substring() {
		String s1 = "this is a test yes it is";
		String s2 = "yet another test";
		String s3 = s2.substring(12); // "test"
		for (int i = 0; i < s1.length(); i++)
			System.out.println("i = " + i + " index = " + s1.lastIndexOf(s3, i));
	}
	
}

/* Expected Output:
12
0
-1
-1
0
-1
3
-1
0
i = 0 index = -1
i = 1 index = -1
i = 2 index = -1
i = 3 index = -1
i = 4 index = -1
i = 5 index = -1
i = 6 index = -1
i = 7 index = -1
i = 8 index = -1
i = 9 index = -1
i = 10 index = 10
i = 11 index = 10
i = 12 index = 10
i = 13 index = 10
i = 14 index = 10
i = 15 index = 10
i = 16 index = 10
i = 17 index = 10
i = 18 index = 10
i = 19 index = 10
i = 20 index = 10
i = 21 index = 10
i = 22 index = 10
i = 23 index = 10
*/
