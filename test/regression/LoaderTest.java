import java.lang.reflect.Method;
import java.io.ByteArrayOutputStream;
import java.io.FileInputStream;
import java.beans.Introspector;

public class LoaderTest {

	public static void main(String argv[]) throws Exception {
		System.out.println("Loading Class0 with default ClassLoader");
		Class c0 = Class.forName("LoaderTest_Class0");
		show(c0);
		ClassLoader l1 = new LoaderTest_Loader1();
		System.out.println("Loading Class1 with Loader1");
		Class c1 = Class.forName("LoaderTest_Class1", true, l1);
		show(c1);

		System.out.println("Loading Class1BeanInfo");
		Introspector.getBeanInfo(c1);

		System.out.println("Loading Class2 via Class1");
		Object o = c1.newInstance();
		Method m = c1.getMethod("getClass2", null);
		Class c2 = (Class)m.invoke(o, null);
		show(c2);

		System.out.println("Loading Class3 via Class1");
		m = c1.getMethod("getClass3", null);
		Class c31 = (Class)m.invoke(null, null);
		show(c31);

		System.out.println("Loading Class3 via Loader2");
		ClassLoader l2 = new LoaderTest_Loader2();
		Class c3 = Class.forName("LoaderTest_Class3", true, l2);
		show(c3);
		System.out.println(c3.equals(c31));
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

// Sources: LoaderTest_Class0.java LoaderTest_Class1.java LoaderTest_Class1BeanInfo.java LoaderTest_Class2.java LoaderTest_Class3.java LoaderTest_Loader1.java LoaderTest_Loader2.java
/* Expected Output:
Loading Class0 with default ClassLoader
-> class LoaderTest_Class0 loader null
Loading Class1 with Loader1
LoaderTest_Loader1: finding LoaderTest_Class1...found
-> class LoaderTest_Class1 loader LoaderTest_Loader1
Loading Class1BeanInfo
LoaderTest_Loader1: finding LoaderTest_Class1BeanInfo...found
Loading Class2 via Class1
LoaderTest_Loader1: finding LoaderTest_Class2...found
LoaderTest_Loader1.findLibrary(LoaderTest_Class2.lib)
-> class LoaderTest_Class2 loader LoaderTest_Loader1
Loading Class3 via Class1
LoaderTest_Loader1: finding LoaderTest_Class3...found
LoaderTest_Loader1.findLibrary(LoaderTest_Class3.lib)
-> class LoaderTest_Class3 loader LoaderTest_Loader1
Loading Class3 via Loader2
LoaderTest_Loader2: finding LoaderTest_Class3
LoaderTest_Loader2.findLibrary(LoaderTest_Class3.lib)
-> class LoaderTest_Class3 loader LoaderTest_Loader2
false
*/
