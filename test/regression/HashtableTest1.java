import java.util.*;
 
public class HashtableTest1 {
    private static class O {
        public int hashCode() {
            return 5;
        }
    }
 
    public static void main(String av[]) {
        Hashtable h = new Hashtable();
        O o1 = new O();
        O o2 = new O();
        h.put(o1, "A");
        h.put(o2, "B");
        h.remove(o1);
        h.put(o2, "B");
        int n = 0;
        for (Enumeration e = h.keys(); e.hasMoreElements(); n++) {
                e.nextElement();
        }
 
        if (n == 1)
            System.out.println("Success.");
        else 
            System.out.println("Failure: --- " + n);
    }
}

/* Expected Output:
Success.
*/
