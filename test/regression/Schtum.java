public class Schtum {
    public static void main(String argv[])
    {
        new Schtum();
	System.out.println("Okay");
    }

    public Schtum()
    {
        Class formals[] = new Class[2];

        try {
            formals[0] = Class.forName("java.lang.String");
        } catch (ClassNotFoundException exc) {
            System.out.println(exc);
        }
    }
}

/* Expected Output:
Okay
*/
