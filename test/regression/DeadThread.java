class btest extends Thread
{

  public void run()
    {
      synchronized(this) {
	System.out.println(isAlive()+" 1");
      }
    }
}

public class DeadThread
{
  public static void main(String args[])
    {
      btest b=null;
      int c = 0;
        
      b = new btest();
      synchronized(b) {
        b.start();
        while(b.isAlive() && c < 10)
        {
          System.out.println(++c+" "+b.isAlive());
          try{b.wait(1000);}catch(InterruptedException e){}
        }
      }
    }
}

/* Expected Output:
1 true
true 1
*/
