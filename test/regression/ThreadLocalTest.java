
public class ThreadLocalTest {

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
    private boolean spawn;
    public TestThread(boolean spawn, String name) {
      super(name);
      this.spawn = spawn;
      //System.out.println("\tThread " + getName() + " created");
    }
    public void run() {
      for (int k = 0; k < 4; k++) {

	System.out.println(getName()+ " I" +k+ "\t TL: "
	  + ThreadLocalTest.tl.get());
	System.out.println(getName()+ " I" +k+ "\tITL: "
	  + ThreadLocalTest.itl.get());

	if (spawn && (k & 1) == 1) {
	  TestThread t1 = new TestThread(false, getName() + "x");
	  t1.start();
	  TestThread t2 = new TestThread(false, getName() + "y");
	  t2.start();
	}

	tl.set(tl.get() + " changed by " + getName() + " at k == " + k);
	itl.set(((k & 1) == 0) ? null : ("set by " + getName() + " at k == " + k));
      }
    }
  }

  public static void main(String[] args) {
    new TestThread(true, "x").start();
    new TestThread(true, "y").start();
  }
}

