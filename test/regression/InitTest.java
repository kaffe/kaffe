/*
 * test for class initialization.
 *
 * provided by Artur Biesiadowski <abies@pg.gda.pl>
 */
public class InitTest
{
	static boolean passed = true;

	public static void main(String argv[] )
	{
		int x;
		if ( argv.length > 10 )
		{
			x = Class1.field;
			Class1.field = 5;
			Class3.runMe();
		}
		else
		{
			Class2.field = 5;
			Class2.runMe();
		}

		x = Class4.finalfield;

		if ( passed )
			System.out.println("Success 2.");
		else
			System.out.println("FAILED");
	}
}

class Class1
{
	static int field;
	static {
		System.out.println("Class1 clinit - WRONG(static access)");
		InitTest.passed = false;
	}
}

class Class2
{
	static int field;
	static {
		System.out.println("Success 1.");
	}

	static void runMe() {}
}

class Class3
{
	static {
		System.out.println("Class3 clinit - WRONG(static mcall)");
		InitTest.passed = false;
	}

	static void runMe() {}
}

class Class4
{
	final static int finalfield = 5;
	static {
		System.out.println("Class4 clinit - WRONG(final static access)");
		InitTest.passed = false;
	}
}

/* Expected Output:
Success 1.
Success 2.
*/
