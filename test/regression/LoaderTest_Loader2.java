public class LoaderTest_Loader2 extends ClassLoader {
	public Class findClass(String name) throws ClassNotFoundException {
		if (!name.startsWith("java"))
			System.out.println(this + ": finding " + name);
		try {
			byte[] buf = LoaderTest.getClassBytes(name);
			return defineClass(name, buf, 0, buf.length);
		} catch (ClassNotFoundException e) {
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
		return "LoaderTest_Loader2";
	}
	protected String findLibrary(String libname) {
		System.out.println(this + ".findLibrary(" + libname + ")");
		return null;
	}
}
