class IndexTest {
	public static void main (String args[]) {
		try {
			int array[] = new int[5];
			for (int i = 0; i < 10; i++) {
				System.out.println(i);
				array[i] = 1;
			}
		}
		catch (Exception c) {
			System.out.println("Catch");
			c.printStackTrace();
		}
	}
}


/* Expected Output:
0
1
2
3
4
5
Catch
java.lang.ArrayIndexOutOfBoundsException
	at java.lang.Throwable.fillInStackTrace(Throwable.java:native)
	at java.lang.Throwable.<init>(Throwable.java:33)
	at java.lang.Exception.<init>(Exception.java:20)
	at java.lang.RuntimeException.<init>(RuntimeException.java:17)
	at java.lang.IndexOutOfBoundsException.<init>(IndexOutOfBoundsException.java:17)
	at java.lang.ArrayIndexOutOfBoundsException.<init>(ArrayIndexOutOfBoundsException.java:17)
	at IndexTest.main(IndexTest.java:7)
*/
