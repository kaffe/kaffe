
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
	 * This is a static only class.
	 */
	private ThreadStack()
	{
	}
}
