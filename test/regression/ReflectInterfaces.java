/*
 * The Art of Reflecting Interfaces.
 */
import java.lang.reflect.Method;

public class ReflectInterfaces
{
	interface If1
	{
		public void if1Method();
		void if1MethodNP();
	}

	interface If2 extends If1
	{
		public void if2Method();
		void if2MethodNP();
	}

	public static void main(String av[])
	{
		Class c = If2.class;
		Class sc = c.getSuperclass();
		System.out.println((sc == null?"null":sc.getName()));
		System.out.println("all methods");
		Method [] m = c.getMethods();
		for (int i = 0; i < m.length; i++)
			System.out.println(m[i].toString());
		System.out.println("declared methods");
		m = c.getDeclaredMethods();
		for (int i = 0; i < m.length; i++)
			System.out.println(m[i].toString());
	}
}
