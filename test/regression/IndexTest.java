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

