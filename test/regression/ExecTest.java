public class ExecTest 
{       
        public static void main ( String[] argv )
        {
		String[] args = { "/bin/echo", "Hello World" };
		String[] env  = { "A=B", "Hello=World" };
                Runtime myRuntime = Runtime.getRuntime ();
                Process myProcess = null;

                for ( int i = 0; i < 2; i++ )
                {
                        try
                        {
                                myProcess = myRuntime.exec ( args );
                                try { myProcess.waitFor (); }
                                catch ( InterruptedException e ) {}
				System.out.println("Okay");
                        }
                        catch ( java.io.IOException e )
                        {
                                System.out.println ( e );
                        }
                }
                try
                {
                        myProcess = myRuntime.exec ( args, env );
                        try { myProcess.waitFor (); }
                        catch ( InterruptedException e ) {}
			System.out.println("Okay");
                 }
                 catch ( java.lang.Exception e )
                 {
                        System.out.println ( e );
                 }
        }
}

/* Expected Output:
Okay
Okay
Okay
*/
