import java.util.*;

public class ttest {
  public static void main(String args[]) {
    TestThread t1 = new TestThread();
    TestThread t2 = new TestThread();
    try {
      t2.start();
      t2.join();
      t1.start();
      t1.join();
    }
    catch (InterruptedException _) {
    }
  }
}

class TestThread extends Thread {
  int cnt = 0;

  TestThread() {
  }

  public void run() {
    while (cnt < 100) {
      System.err.println("run "+cnt);
      cnt++;
    }
  }
}
