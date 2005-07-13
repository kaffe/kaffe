/*
 * JNIWeakTest.java -- Test the handling of JNI weak reference.
 *
 * Copyright (C) 2005
 *    The Kaffe.org's developers. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

public class JNIWeakTest
{
	static {
		System.out.println("Loading jniweaklib...");
		System.loadLibrary("jniweaklib");
	}

	native Object testWeak();

	static void testArgWeak(Object a, Object b)
	{
		if (a == b)
			System.out.println("Args OK !");
		else
			System.out.println("Args FAIL !");
	}

	void testInvoke()
	{
		System.out.println("Invoke OK !");
	}

	static native void passWeakArg(Object a);

	static native void invokeWeak(Object a);

	static public void main(String args[])
	{
		JNIWeakTest o = new JNIWeakTest();
		Object o2;

		o2 = o.testWeak();

		if (o2 == o)
			System.out.println("Return OK !");
		else
			System.out.println("Return FAIL !");

		passWeakArg(new Object());

		invokeWeak(o);
	}
}

