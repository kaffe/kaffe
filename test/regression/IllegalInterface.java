interface Illegalif {
}

class IllegalInterface {
    public static void main(String argv[]) {
try {
    Class c = Class.forName("Illegalif");
    Object o = c.newInstance();
// Shouldn't happen:
    System.out.println(o.toString());
} catch (Exception e) {
    System.out.println("Exception caught: " + e.toString());
}
    }
}

/* Expected Output:
Exception caught: java.lang.InstantiationException: Illegalif
*/
