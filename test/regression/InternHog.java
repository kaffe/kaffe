public class InternHog {
	public static final int MAX_NUM = 100000;
	public static void main(String[] args) {
		int x = 0;
		try {
			if ((Integer.toString(0) + "aa").intern() != "0aa")
				throw new Exception("intern inequality");
			while (x < MAX_NUM)
				Integer.toString(x++).intern();
			System.out.println("Success.");
		} catch (Throwable t) {
			System.out.println("Failed at " + x + ": " + t);
			t.printStackTrace(System.out);
		}
	}
}


/* Expected Output:
Success.
*/
