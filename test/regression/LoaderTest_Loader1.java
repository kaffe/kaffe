public class LoaderTest_Loader1 extends ClassLoader {
	public Class findClass(String name) throws ClassNotFoundException {
		boolean debug = !name.startsWith("java");
		if (debug)
			System.out.print(this + ": finding " + name + "...");
		try {
			byte[] buf = LoaderTest.getClassBytes(name);
			Class rtn = defineClass(name, buf, 0, buf.length);
			if (debug)
				System.out.println("found");
			return rtn;
		} catch (ClassNotFoundException e) {
			if (debug)
				System.out.println("not found");
			return findSystemClass(name);
		}
	}
	public Class loadClass(String name, boolean resolve)
			throws ClassNotFoundException {
		Class c;
		if ((c = findClass(name)) == null)
			throw new ClassNotFoundException(name);
		if (resolve)
			resolveClass(c);
		return (c);
	}
	public String toString() {
		return "LoaderTest_Loader1";
	}
	protected String findLibrary(String libname) {
		System.out.println(this + ".findLibrary(" + libname + ")");
		return null;
	}
}
