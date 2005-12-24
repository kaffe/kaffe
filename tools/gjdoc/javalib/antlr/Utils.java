package antlr;

public class Utils {
	private static boolean useSystemExit = true;
	private static boolean useDirectClassLoading = false;
	static {
		if ("true".equalsIgnoreCase(System.getProperty("ANTLR_DO_NOT_EXIT", "false")))
			useSystemExit = false;
		if ("true".equalsIgnoreCase(System.getProperty("ANTLR_USE_DIRECT_CLASS_LOADING", "false")))
			useDirectClassLoading = true;
	}

	/** Thanks to Max Andersen at JBOSS and Scott Stanchfield */
	public static Class loadClass(String name) throws ClassNotFoundException {
		try {
			ClassLoader contextClassLoader = Thread.currentThread().getContextClassLoader();
			if (!useDirectClassLoading && contextClassLoader!=null ) {
				return contextClassLoader.loadClass(name);
			}
			return Class.forName(name);
		}
		catch (Exception e) {
			return Class.forName(name);
		}
	}

	public static Object createInstanceOf(String name) throws ClassNotFoundException, InstantiationException, IllegalAccessException {
		return loadClass(name).newInstance();
	}

	public static void error(String message) {
		if (useSystemExit)
			System.exit(1);
		throw new RuntimeException("ANTLR Panic: " + message);
	}

	public static void error(String message, Throwable t) {
		if (useSystemExit)
			System.exit(1);
		throw new RuntimeException("ANTLR Panic", t);
	}
}
