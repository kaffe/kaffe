class btest extends Thread
{

  public void run()
    {
      System.out.println(isAlive()+" 1");
      this.stop();
    }
}

public class DeadThread
{
  public static void main(String args[])
    {
      btest b=null;
      int c = 0;
        
      b = new btest();
      b.start();
      while(b.isAlive() && c < 10)
        {
          System.out.println(++c+" "+b.isAlive());
          try{Thread.sleep(1000);}catch(InterruptedException e){}
        }
    }
}
