/**
 * Test case for PR#690.
 * The read method of java.io.PipedInputStream has no 'early out'.
 *
 * @author Win van Velthoven <ftu@fi.uu.nl>
 */
import java.io.*;
class PipeTest extends Thread
{
        PipedInputStream in;
        PipedOutputStream out;

        InputStreamReader isr;
        OutputStreamWriter osw;

        BufferedReader br;
        PrintWriter pw;

        PipeTest() throws IOException
        {
                in = new PipedInputStream();
                isr = new InputStreamReader(in);
                br  = new BufferedReader(isr);
                out = new PipedOutputStream(in);
                osw = new OutputStreamWriter(out);
                pw  = new PrintWriter(osw,true);
        }

        public void run()
        {
           try
           {
                String line = br.readLine();
                System.out.println(line);
           } catch(IOException _) {}
        }

        public static void main(String[] args)  throws IOException
        {
           PipeTest t = new PipeTest();
           t.start();
           t.pw.println("PipeTest");
        }
}
/* Expected Output:
PipeTest
*/
