import java.lang.reflect.Method;

public class LoaderTest_Class1 {
	public LoaderTest_Class1 () {
	}
	public Class getClass2() throws Exception {
		return Class.forName("LoaderTest_Class2");
	}
	public static Class getClass3() throws Exception {
		Class c = Class.class;
		Method m = c.getMethod("forName", new Class[] { String.class });
		return (Class)m.invoke(c, new Object[] { "LoaderTest_Class3" });
	}
}
