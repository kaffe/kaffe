class Str2 {
	public static void main (String args[]) {
		String a = "1";
		a += "2";
		System.out.println(a);
		System.out.println("abc".indexOf("", -32));
		System.out.println("abc".indexOf("", 100));
		System.out.println("abc".lastIndexOf("", -32));
		System.out.println("abc".lastIndexOf("", 100));
	}
}

/* Expected Output:
12
0
-1
-1
3
*/
