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
	at IndexTest.main(IndexTest.java:7)
*/
