
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

	synchronized (System.out) {
	  System.out.println(getName()+ " I" +k+ "\t TL: "
	    + ThreadLocalTest.tl.get());
	  System.out.println(getName()+ " I" +k+ "\tITL: "
	    + ThreadLocalTest.itl.get());
	}

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

// Sort output
/* Expected Output:
x I0	 TL:  TL initial
x I0	ITL: ITL initial
x I1	 TL:  TL initial changed by x at k == 0
x I1	ITL: null
x I2	 TL:  TL initial changed by x at k == 0 changed by x at k == 1
x I2	ITL: set by x at k == 1
x I3	 TL:  TL initial changed by x at k == 0 changed by x at k == 1 changed by x at k == 2
x I3	ITL: null
xx I0	 TL:  TL initial
xx I0	 TL:  TL initial
xx I0	ITL: null inherited from x
xx I0	ITL: null inherited from x
xx I1	 TL:  TL initial changed by xx at k == 0
xx I1	 TL:  TL initial changed by xx at k == 0
xx I1	ITL: null
xx I1	ITL: null
xx I2	 TL:  TL initial changed by xx at k == 0 changed by xx at k == 1
xx I2	 TL:  TL initial changed by xx at k == 0 changed by xx at k == 1
xx I2	ITL: set by xx at k == 1
xx I2	ITL: set by xx at k == 1
xx I3	 TL:  TL initial changed by xx at k == 0 changed by xx at k == 1 changed by xx at k == 2
xx I3	 TL:  TL initial changed by xx at k == 0 changed by xx at k == 1 changed by xx at k == 2
xx I3	ITL: null
xx I3	ITL: null
xy I0	 TL:  TL initial
xy I0	 TL:  TL initial
xy I0	ITL: null inherited from x
xy I0	ITL: null inherited from x
xy I1	 TL:  TL initial changed by xy at k == 0
xy I1	 TL:  TL initial changed by xy at k == 0
xy I1	ITL: null
xy I1	ITL: null
xy I2	 TL:  TL initial changed by xy at k == 0 changed by xy at k == 1
xy I2	 TL:  TL initial changed by xy at k == 0 changed by xy at k == 1
xy I2	ITL: set by xy at k == 1
xy I2	ITL: set by xy at k == 1
xy I3	 TL:  TL initial changed by xy at k == 0 changed by xy at k == 1 changed by xy at k == 2
xy I3	 TL:  TL initial changed by xy at k == 0 changed by xy at k == 1 changed by xy at k == 2
xy I3	ITL: null
xy I3	ITL: null
y I0	 TL:  TL initial
y I0	ITL: ITL initial
y I1	 TL:  TL initial changed by y at k == 0
y I1	ITL: null
y I2	 TL:  TL initial changed by y at k == 0 changed by y at k == 1
y I2	ITL: set by y at k == 1
y I3	 TL:  TL initial changed by y at k == 0 changed by y at k == 1 changed by y at k == 2
y I3	ITL: null
yx I0	 TL:  TL initial
yx I0	 TL:  TL initial
yx I0	ITL: null inherited from y
yx I0	ITL: null inherited from y
yx I1	 TL:  TL initial changed by yx at k == 0
yx I1	 TL:  TL initial changed by yx at k == 0
yx I1	ITL: null
yx I1	ITL: null
yx I2	 TL:  TL initial changed by yx at k == 0 changed by yx at k == 1
yx I2	 TL:  TL initial changed by yx at k == 0 changed by yx at k == 1
yx I2	ITL: set by yx at k == 1
yx I2	ITL: set by yx at k == 1
yx I3	 TL:  TL initial changed by yx at k == 0 changed by yx at k == 1 changed by yx at k == 2
yx I3	 TL:  TL initial changed by yx at k == 0 changed by yx at k == 1 changed by yx at k == 2
yx I3	ITL: null
yx I3	ITL: null
yy I0	 TL:  TL initial
yy I0	 TL:  TL initial
yy I0	ITL: null inherited from y
yy I0	ITL: null inherited from y
yy I1	 TL:  TL initial changed by yy at k == 0
yy I1	 TL:  TL initial changed by yy at k == 0
yy I1	ITL: null
yy I1	ITL: null
yy I2	 TL:  TL initial changed by yy at k == 0 changed by yy at k == 1
yy I2	 TL:  TL initial changed by yy at k == 0 changed by yy at k == 1
yy I2	ITL: set by yy at k == 1
yy I2	ITL: set by yy at k == 1
yy I3	 TL:  TL initial changed by yy at k == 0 changed by yy at k == 1 changed by yy at k == 2
yy I3	 TL:  TL initial changed by yy at k == 0 changed by yy at k == 1 changed by yy at k == 2
yy I3	ITL: null
yy I3	ITL: null
*/
