// --------- KaffeVerifyBug.java -----------------------------------------
 
import java.net.Socket;
 
public class KaffeVerifyBug
{
    public static void main(String args[])
    {
        doit();
	System.out.println("okay");
    }
 
    public static int doit()
    {
        int a = 1;
 
        try
        {
            synchronized(new Object())
            {
                new Socket("", 0);
                return 2;
            }
        }
        catch (Exception e)
        {
            a = 3;
        }
 
        return a;
    }
}

/* Expected Output:
okay
*/
