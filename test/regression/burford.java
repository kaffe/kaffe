
import java.io.*;

public class burford implements Runnable {
    public static void main(String argv[])
    {
        burford b = new burford();
        Thread bt = new Thread(b);

        System.out.println("Starting burford...");
        
        bt.start();

        Thread f1 = new Thread(b.new feeder(b));
        Thread f2 = new Thread(b.new feeder(b));

        System.out.println("Starting feeders...");
        
        f1.start();
        f2.start();

        try {
            f1.join();
            f2.join();
        } catch (InterruptedException exc) {}

        System.out.println("Feeders completed...");
        
        b.awaken();

        System.out.println("burford awakened -- result = " + b.counter());
        System.exit(0);
    }

    public void run()
    {
        try {
            synchronized (v_mutex) {
                v_mutex.wait();
            }
        } catch (InterruptedException exc) {}
    }

    public synchronized int counter()
    {
        return (v_counter);
    }
    
    public synchronized void increment()
    {
        v_counter++;
    }
    
    public void awaken()
    {
        synchronized (v_mutex) {
            v_mutex.notify();
        }
    }

    class feeder implements Runnable {
        feeder(burford target)
        {
            v_target = target;
        }

        public void run()
        {
            for (int i = 0; i < 50; i++) {
                v_target.increment();
            }
        }

        private burford v_target;
    }
    
    private Object v_mutex = new Object();
    private int v_counter = 0;
}

/* Expected Output:
Starting burford...
Starting feeders...
Feeders completed...
burford awakened -- result = 100
*/
