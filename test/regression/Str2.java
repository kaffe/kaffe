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
*/
