/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.lang;

public class Thread implements Runnable {
  public final static int MIN_PRIORITY = 1;
  public final static int NORM_PRIORITY = 5;
  public final static int MAX_PRIORITY = 10;

  private static int threadCount = 0;  

  private char[] name;
  private int priority;
  private Thread threadQ;
  private kaffe.util.Ptr PrivateInfo;
  private boolean daemon;
  private boolean interrupting;
  private Runnable target;
  private ThreadGroup group;

  private kaffe.util.Ptr exceptPtr;
  private kaffe.util.Ptr exceptObj;
  private kaffe.util.Ptr jnireferences;
  private boolean dying;

  public native int countStackFrames();
  public static native Thread currentThread();
  public static native void yield();
  public static native void sleep(long millis) throws InterruptedException;
  public final native boolean isAlive();
  public synchronized native void start();
  
  private native void setPriority0(int prio);
  private native void stop0(Object obj);
  private native void suspend0();
  private native void resume0();

  public Thread()
  {
    this (null, null, generateName());
  }

  public Thread(Runnable target)
  {
    this (null, target, generateName());
  }

  public Thread(ThreadGroup group, Runnable target)
  {
    this (group, target, generateName());
  }
	
  public Thread(String name)
  {
    this (null, null, name);
  }

  public Thread(ThreadGroup group, String name)
  {
    this (group, null, name);
  }

  public Thread(Runnable target, String name)
  {
    this (null, target, name);
  }

  public Thread(ThreadGroup group, Runnable target, String name)
  {
    if (group == null) {
      this.group = Thread.currentThread().getThreadGroup();
    }
    else {
      this.group = group;
    }
    this.group.checkAccess();
    this.group.add(this);

    this.name = name.toCharArray();
    this.target = target;
    this.interrupting = false;

    int tprio = Thread.currentThread().getPriority();
    int gprio = this.group.getMaxPriority();
    if (tprio < gprio) {
      setPriority0(tprio);
    }
    else {
      setPriority0(gprio);
    }

    setDaemon(Thread.currentThread().isDaemon());
  }

  private static String generateName()
  {
    return new String("Thread-"+threadCount++);
  }

  public static void sleep(long millis, int nanos) throws InterruptedException
  {
    sleep(millis);
  }

  public void run()
  {
    if (target != null) {
      target.run();
    }
  }

  private void exit()
  {
    /* TSK.. Nice one Sun (Again).. Undocumented API call to Thread class.
       Called by both Java and Kaffe..

       Assume it does what any normal exit() does.. e.g. cleanup */
    
    if (group != null) {
      group.remove(this);
    }
  }

  public final void stop()
  {
    checkAccess();

    //    if (group!=null) group.remove(this);

    // Only stop thread if it's alive already.
    if (isAlive()) {
      stop0(null);
    }
  }

  public final synchronized void stop(Throwable o)
  {
    checkAccess();

    if (o==null) throw new NullPointerException();

    //    if (group!=null) group.remove(this);

    stop0(o);
  }

  public void interrupt()
  {
    interrupting = true;
  }

  public static boolean interrupted() 
  {
    return (Thread.currentThread().isInterrupted());
  }

  public boolean isInterrupted()
  {
    return (interrupting);
  }

  public void destroy()
  {
    stop0(null);
  }

  public final void suspend()
  {
    checkAccess();

    if (isAlive()) {
      suspend0();
    }
  }

  
  public final void resume()
  {
    checkAccess();

    if (isAlive()) {
      resume0();
    }
  }
  
  public final void setPriority(int newPriority)
  {
    checkAccess();

    if (newPriority < MIN_PRIORITY || newPriority > group.getMaxPriority()) {
      throw new IllegalArgumentException();
    }
    setPriority0(newPriority);
  }
  
  public final int getPriority()
  {
    return priority;
  }
  
  public final void setName(String name)
  {
    this.name = name.toCharArray();
  }
  
  public final String getName()
  {
    return (new String(name));
  }
  
  public final ThreadGroup getThreadGroup()
  {
    return (group);
  }
  
  public static int activeCount()
  {
    return (Thread.currentThread().getThreadGroup().activeCount());
  }
  
  public static int enumerate(Thread tarray[])
  {
    return (Thread.currentThread().getThreadGroup().enumerate(tarray));
  }
  
  public final synchronized void join(long millis) throws InterruptedException
  {
    join(millis, 0);
  }
  
  public final synchronized void join(long millis, int nanos) throws InterruptedException
  {
    /* Wait while time remains and alive */
    if ((millis==0) && (nanos==0)) {
      while (isAlive()) wait(0);
    }
    else {
      long start = System.currentTimeMillis();
      while (isAlive() && (System.currentTimeMillis()<millis+start)) {
	long toWait = millis+start-System.currentTimeMillis();
	wait(toWait);
      }
    }
  }
  
  public final void join() throws InterruptedException
  {
    join(0);
  }
  
  public static void dumpStack()
  {
    Throwable t = new Throwable();

    t.printStackTrace();
  }
  
  public final void setDaemon(boolean on)
  {
    daemon = on;
  }
  
  public final boolean isDaemon()
  {
    return daemon;
  }
  
  public void checkAccess()
  {
    SecurityManager man = System.getSecurityManager();
    if (man != null) {
      man.checkAccess(this);
    }
  }
  
  public String toString()
  {
    return getName();
  }

  protected void finalize()
  {
    finalize0();
  }

  private final native void finalize0();
}

