/*
 * test we don't screw up the interface <clinit> method
 */
interface Intf {
    public final String [] hw = new String[] 
	{
		"Hello",
		"World."
	};
};

public class IntfTest {
    public static void main(String av[]) {
	String a = Intf.hw[0];
	System.gc();
	System.out.println(a + " " + Intf.hw[1]);
	System.gc();
    }
}

/* Expected Output:
Hello World.
*/
