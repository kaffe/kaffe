/*
 * a simple test for kaffe.lang.UNIXProcess.
 * This will run on Unixoids only.
 *
 * We spawn a copy of the kaffe JVM which executes this program, sends
 * a message through stdout back to its parent, which is compared.
 *
 * If the output is "readline returning null", that likely means the exec
 * failed.  Check execve/execvp/your path, look in jthread.c
 */
import java.io.*;

public class ProcessTest {

    // note that this will only work if we have execvp()
    public static final String childmsg = "Hi, mom";

    public static void main(String args[])
    {
	if (args.length > 1) {
	    // child
	    System.out.println(args[1]);
	} else {
	    String kaffe_exe = args[0];

	    // parent
	    try {
		Process process = Runtime.getRuntime().exec(
		    new String [] {
			kaffe_exe, "ProcessTest", "-child", childmsg
		    });
		InputStream is = process.getInputStream();
		if (is == null)
			throw new Exception("no input stream");

		InputStreamReader isr = new InputStreamReader(is);
		LineNumberReader r = new LineNumberReader(isr);

		String s = r.readLine();
		if (s == null)
			throw new Exception("readline returning null");
		if (!s.equals(childmsg))
		    throw new Exception("child talked back " + s);
		process.waitFor();
		System.out.println("ProcessTest: Success.");

	    } catch (Throwable th) {
		System.out.println("Caught exception: " +th.getMessage());
		th.printStackTrace(System.out);
	    }
	}
    }
}

// java args: ProcessTest $JAVA
/* Expected Output:
ProcessTest: Success.
*/
