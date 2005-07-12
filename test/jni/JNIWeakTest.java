public class JNIWeakTest
{
	static {
		System.out.println("Loading jniweaklib...");
		System.loadLibrary("jniweaklib");
	}

	native Object testWeak();

	static public void main(String args[])
	{
		JNIWeakTest o = new JNIWeakTest();
		Object o2;

		o2 = o.testWeak();

		if (o2 == o)
			System.out.println("OK !");
		else
			System.out.println("FAIL !");
	}
}

