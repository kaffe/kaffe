/*
 * This class tests that 
 * - we do not report java.io.Serializable for arrays 
 * - we report the same array modifiers as Sun
 * - we implement isAssignableFrom properly for classes and arrays
 */
public class TestSerializable
{
    class help implements java.io.Serializable
    {
    }

    public static void main(String av[])
    {
	Class c = TestSerializable[].class;
	System.out.println(c.getInterfaces().length);
	System.out.println(c.getModifiers());
	Class s = java.io.Serializable.class;
	System.out.println(s.isAssignableFrom(help.class));
	System.out.println(s.isAssignableFrom(help[].class));
	System.out.println(s.isAssignableFrom(TestSerializable.class));
	System.out.println(s.isAssignableFrom(TestSerializable[].class));
	System.out.println(s.isAssignableFrom(boolean.class));
	System.out.println(s.isAssignableFrom((new boolean[0]).getClass()));
    }
}
