public class ExecTest 
{       
        public static void main ( String[] argv )
        {
                for ( int i = 0; i < 2; i++ )
                {
                        try
                        {
				String[] args = { "/bin/ls" };
                                Runtime myRuntime = Runtime.getRuntime ();
                                Process myProcess = myRuntime.exec ( args );
                                try { myProcess.waitFor (); }
                                catch ( InterruptedException e ) {}
				System.out.println("Okay");
                        }
                        catch ( java.io.IOException e )
                        {
                                System.out.println ( e );
                        }
                }
        }
}
