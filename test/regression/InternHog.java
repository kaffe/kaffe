public class InternHog {
	public static final int MAX_NUM = 100000;
	public static void main(String[] args) {
		int x = 0;
		try {
			while (x < MAX_NUM)
				Integer.toString(x++).intern();
			System.out.println("Success.");
		} catch (Throwable t) {
			System.out.println("Failed at " + x + ": " + t);
			t.printStackTrace(System.out);
		}
	}
}

