/**
 * test proper behavior of getMethod when passed null.
 * Submitted by Moses DeJong <dejong@cs.umn.edu>
 */
import java.lang.reflect.*;
import java.util.*;
 
 
public class MethodBug {
    public static void main(String[] argv) throws Exception {
	Method m;
	Constructor c;
        m = MethodBug.class.getDeclaredMethod("m", null);
        System.out.println("m is " + m);
        m = MethodBug.class.getMethod("m", null);
        System.out.println("m is " + m);
        c = MethodBug.class.getConstructor(null);
        System.out.println("c is " + c);
        c = MethodBug.class.getDeclaredConstructor(null);
        System.out.println("c is " + c);
    }
 
    public void m() {}
    public MethodBug() {}
}

/* Expected Output:
m is public void MethodBug.m()
m is public void MethodBug.m()
c is public MethodBug()
c is public MethodBug()
*/
