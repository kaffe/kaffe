import java.lang.reflect.Method;
import java.io.ByteArrayOutputStream;
import java.io.FileInputStream;

public class LoaderTest {

	public static void main(String argv[]) throws Exception {
		System.out.println("Loading Class0 with default ClassLoader");
		Class c0 = Class.forName("Class0");
		show(c0);
		ClassLoader l1 = new Loader1();
		System.out.println("Loading Class1 with Loader1");
		Class c1 = Class.forName("Class1", true, l1);
		show(c1);

		System.out.println("Loading Class2 via Class1");
		Object o = c1.newInstance();
		Method m = c1.getMethod("getClass2", null);
		Class c2 = (Class)m.invoke(o, null);
		show(c2);

		System.out.println("Loading Class3 via Loader2");
		ClassLoader l2 = new Loader2();
		Class c3 = Class.forName("Class3", true, l2);
		show(c3);
	}

	public static byte[] getClassBytes(String name)
			throws ClassNotFoundException {
		ByteArrayOutputStream bas = new ByteArrayOutputStream();
		try {
			FileInputStream fis = new FileInputStream(
			    name + ".class");
			byte[] buf = new byte[1024];
			for (int r; (r = fis.read(buf)) != -1; )
				bas.write(buf, 0, r);
			fis.close();
			bas.close();
		} catch (Exception e) {
			throw new ClassNotFoundException();
		}
		return bas.toByteArray();
	}

	public static void show(Class c) throws Exception {
		ClassLoader loader = c.getClassLoader();
		if (ClassLoader.getSystemClassLoader().equals(loader))
			loader = null;
		System.out.println("-> class " + c.getName()
		    + " loader " + loader);
	}
}

class Class0 {
}

class Class1 {
	public Class1 () {
	}
	public Class getClass2() throws Exception {
		return Class.forName("Class2");
	}
}

class Class2 {
	static {
		try {
			System.loadLibrary("Class2.lib");
		} catch (UnsatisfiedLinkError e) {
		}
	}
}

class Class3 {
	static {
		try {
			System.loadLibrary("Class3.lib");
		} catch (UnsatisfiedLinkError e) {
		}
	}
}

class Loader1 extends ClassLoader {
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
		return "Loader1";
	}
	protected String findLibrary(String libname) {
		System.out.println(this + ".findLibrary(" + libname + ")");
		return null;
	}
}

class Loader2 extends ClassLoader {
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
		return "Loader2";
	}
	protected String findLibrary(String libname) {
		System.out.println(this + ".findLibrary(" + libname + ")");
		return null;
	}
}

/* Expected Output:
Loading Class0 with default ClassLoader
-> class Class0 loader null
Loading Class1 with Loader1
Loader1: finding Class1
-> class Class1 loader Loader1
Loading Class2 via Class1
Loader1: finding Class2
Loader1.findLibrary(Class2.lib)
-> class Class2 loader Loader1
Loading Class3 via Loader2
Loader2: finding Class3
Loader2.findLibrary(Class3.lib)
-> class Class3 loader Loader2
*/
