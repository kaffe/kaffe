/*
 * Just a quick sanity check that we don't break the precise gc because
 * of how we allocate class objects.
 *
 * @author Godmar Back
 */
public class TestClassRef {
    Class	c;
    Object	o;
    static Class cs;
    static Object os;

    public static void main(String av[]) {

	/* the only point here is that the gc doesn't blow up. */
	TestClassRef f = new TestClassRef();
	f.c = Integer.TYPE;
	f.o = Integer.TYPE;
	cs = f.c;
	os = f.o;
	Runtime.getRuntime().gc();

	TestClassRef g = new TestClassRef();
	g.c = java.lang.System.class;
	g.o = java.lang.System.class;
	cs = g.c;
	os = g.o;
	Runtime.getRuntime().gc();

	TestClassRef h = new TestClassRef();
	h.c = TestClassRef.class;
	h.o = TestClassRef.class;
	cs = h.c;
	os = h.o;
	Runtime.getRuntime().gc();

	System.out.println("Success.");
    }
}

/* Expected Output:
Success.
 */
