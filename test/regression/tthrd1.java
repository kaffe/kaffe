class btst1 extends Thread
{

  public void run()
    {
      System.out.println(isAlive()+" 1");
    }
}

public class tthrd1
{
  public static void main(String args[])
    {
      btst1 b=null;
      int c = 0;
        
      System.err.println("main start");
      b = new btst1();
      b.start();
      // System.err.println("thrd start");

      // System.err.println("thrd join");
      try{b.join();}catch(InterruptedException e){}
      System.err.println("main end");
    }
}

/* Expected Output:
main start
true 1
main end
*/
