public class StackDump {
    StackDump()
    { }
    public void f(int i)
    {
        StackDump s;
        s = (i == 0) ? null : this;
        s.f(i - 1);
    }
    public static void main(String[] args)
    {
        StackDump s = new StackDump();

        s.f(10);
    }
}

/* Expected Output:
java.lang.NullPointerException
	at StackDump.f(StackDump.java:8)
	at StackDump.f(StackDump.java:8)
	at StackDump.f(StackDump.java:8)
	at StackDump.f(StackDump.java:8)
	at StackDump.f(StackDump.java:8)
	at StackDump.f(StackDump.java:8)
	at StackDump.f(StackDump.java:8)
	at StackDump.f(StackDump.java:8)
	at StackDump.f(StackDump.java:8)
	at StackDump.f(StackDump.java:8)
	at StackDump.f(StackDump.java:8)
	at StackDump.main(StackDump.java:14)
*/
