
package kaffe.lang;

/**
 * Retrieves the classes whose methods correspond to each frame on the current
 * thread's stack.  The class is for internal use only and the loaders _should_
 * deny access to it by anything but a bootstrap class.
 */
public class ThreadStack
{
	/**
	 * @return An array of classes that correspond to the active methods on
	 * the current thread's stack.
	 */
	public static native Class[] getClassStack();

	/**
	 * @param cl The list of classes to search for the first non-null class
	 * loader.
	 * @return The first non-null class loader found in the array or null.
	 */
	public static ClassLoader firstClassLoader(Class cl[])
	{
		ClassLoader retval = null;
		int lpc;

		for( lpc = 0; (lpc < cl.length) && (retval == null); lpc++ )
		{
			ClassLoader loader;

			loader = cl[lpc].getClassLoader();
			if( loader != null )
			{
				retval = loader;
			}
		}
		return retval;
	}

	/**
	 * @return The first non-null class loader found while going up this
	 * thread's stack.
	 */
	public static ClassLoader firstClassLoader()
	{
		return firstClassLoader(getClassStack());
	}

	/**
	 * @param javaPrimordial whether to return the java interface to the
	 *        primordial class loader or simply null
	 *
	 * @return the class loader of the method that invoked the caller of
	 * this method.
	 */
	public static ClassLoader getCallersClassLoader(boolean javaPrimordial)
	{
		return getCallersClassLoader(getClassStack(), javaPrimordial);
	}

	/**
	 *
	 */
	public static ClassLoader getCallersClassLoader(Class[] classStack, boolean javaPrimordial)
	{
		/*
		 * classStack looks like this:
		 *	ThreadStack.getClassStack()
		 *	(ThreadStack.getCallersClassLoader())?
		 *	Caller of ThreadStack.getCallersClassLoader()
		 *	Caller of caller of ThreadStack.getCallersClassLoader()
		 * 	...
		 */
		/*
                 * we can't use ThreadStack.class here, since that uses forName
		 * and thus would cause an endless loop ...
		 */
		int frameIdx = classStack[0] == classStack[1] ? 3 : 2;

		/*
		 * if this is already the last class on the stack, we're done
		 * this should only happen in the very beginning when main.c
		 * calls FindClass and thus forName outside a java class, so
		 * we return the AppClassLoader in this case.
		 */
		if (frameIdx == classStack.length) {
			return AppClassLoader.getSingleton();
		}

		/*
		 * skip possible recursions inside the class that
		 * invoked getClassStack or getCallersClassLoader
		 */
		for (;frameIdx<classStack.length && classStack[frameIdx-1]==classStack[frameIdx]; frameIdx++);

		/*
		 * this should not happen ....
		 */
		if (frameIdx==classStack.length) {
			return javaPrimordial ? PrimordialClassLoader.getSingleton() : null;
		}

		/*
		 * skip java.lang.reflect.Method frames
		 * XXX what is with java.lang.reflect.Constructor?
		 * XXX anybody out there that invokes java.lang.reflect.Method via reflection :o) ?
		 * XXX what does the JDK do here?
		 */
		if (classStack[frameIdx].getName().equals ("java.lang.reflect.Method")) {
			frameIdx+=2;
		}

		ClassLoader ret = classStack[frameIdx].getClassLoader();

		return (ret==null && javaPrimordial) ? PrimordialClassLoader.getSingleton() : ret; 
	}

	/**
	 * This is a static only class.
	 */
	private ThreadStack()
	{
	}
}
