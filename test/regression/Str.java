public class Str {
	public static void main(String[] args) {

		StringBuffer a = new StringBuffer("1234567890");
		a.insert(2, "abcd");
		System.out.println(a);

		a = new StringBuffer();
		a.insert(0, "Hello");
		System.out.println(a);

		a = new StringBuffer();
		try {
			a.insert(-1, "Str");
		} catch (StringIndexOutOfBoundsException si) {
			System.out.println("Caught -1");
		}

		a = new StringBuffer();
		try {
			a.insert(1, "Str");
		} catch (StringIndexOutOfBoundsException si) {
			System.out.println("Caught +1");
		}

		a = new StringBuffer("Blah");
		a.insert(4, "Blah");
		System.out.println(a);

		a = new StringBuffer();
		a.append("Life sucks.".toCharArray());
		a.replace(5, 10, "is absolutely wonderful");
		a.deleteCharAt(28);
		a.insert(28, "!");
		System.out.println(a);

		a = new StringBuffer(1);
		try {
			a.insert(1, "Str");
		} catch (StringIndexOutOfBoundsException si) {
			System.out.println("Caught 1+1");
		}

		// Pass -1 as the size of a StringBuffer
		try {
			new StringBuffer(-1);
		} catch (NegativeArraySizeException e) {
			System.out.println("Caught -1");
		}

		// Pass 0 as the size of a StringBuffer
		new StringBuffer(0);
		System.out.println("Zero OK");
	}
}

/* Expected Output:
12abcd34567890
Hello
Caught -1
Caught +1
BlahBlah
Life is absolutely wonderful!
Caught 1+1
Caught -1
Zero OK
*/

