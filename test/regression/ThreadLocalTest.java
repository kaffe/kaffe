
public class ThreadLocalTest {

  private static int threadNum;

  private static ThreadLocal tl = new ThreadLocal() {
    protected Object initialValue() {
      return " TL initial";
    }
  };

  private static InheritableThreadLocal itl = new InheritableThreadLocal() {
    protected Object initialValue() {
      return "ITL initial";
    }
    protected Object childValue(Object pval) {
      return (String) pval + " inherited from " + Thread.currentThread().getName();
    }
  };

  private static class TestThread extends Thread {
    private int num;
    private int delay;
    private boolean spawn;
    public TestThread(int delay, boolean spawn) {
      super("" + ++threadNum);
      this.num = threadNum;
      this.delay = delay;
      this.spawn = spawn;
    }
    public void run() {
      for (int k = 0; k < 4; k++) {

	try {
	  sleep(delay);
	} catch (InterruptedException e) {}

	System.out.println("Thread " + getName() + " interation " + k + ":");
	System.out.println("\t TL: " + ThreadLocalTest.tl.get());
	System.out.println("\tITL: " + ThreadLocalTest.itl.get());

	if (spawn && k == num) {
	  TestThread t1 = new TestThread(delay, false);
	  System.out.println("\tCreating new thread " + t1.getName());
	  t1.start();
	  TestThread t2 = new TestThread(delay, false);
	  System.out.println("\tCreating new thread " + t2.getName());
	  t2.start();
	}

	tl.set(tl.get() + " changed by " + num + " at k == " + k);
	itl.set(((k & 1) == 0) ? null : ("set by " + num + " at k == " + k));
      }
    }
  }

  public static void main(String[] args) {
    new TestThread(500, true).start();
    new TestThread(550, true).start();
  }
}

