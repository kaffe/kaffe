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

import java.util.Hashtable;
import java.util.Enumeration;
import kaffe.util.Deprecated;

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
  private Hashtable threadLocals;

  public native int countStackFrames();
  public static native Thread currentThread();
  public static native void yield();
  public final native boolean isAlive();
  public synchronized native void start();
  
  private native void setPriority0(int prio);

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
    final Thread parent = Thread.currentThread();

    if (group == null) {
      this.group = parent.getThreadGroup();
    }
    else {
      this.group = group;
    }
    this.group.checkAccess();
    this.group.add(this);

    this.name = name.toCharArray();
    this.target = target;
    this.interrupting = false;

    /*
     * Inherit all inheritable thread-local variables from parent to child
     */
    if (parent.threadLocals != null) {
      for (Enumeration e = parent.threadLocals.keys(); e.hasMoreElements(); ) {
	Object key = e.nextElement();
	if (key instanceof InheritableThreadLocal) {
	  InheritableThreadLocal i = (InheritableThreadLocal) key;
	  i.set(this, i.childValue(i.get()));
	}
      }
    }

    int tprio = parent.getPriority();
    int gprio = this.group.getMaxPriority();
    if (tprio < gprio) {
      setPriority0(tprio);
    }
    else {
      setPriority0(gprio);
    }

    setDaemon(parent.isDaemon());
  }

  private static String generateName()
  {
    return new String("Thread-"+threadCount++);
  }

  public static void sleep(long millis, int nanos) throws InterruptedException
  {
    sleep(millis);
  }

  public static void sleep(long millis) throws InterruptedException
  {
    sleep0(millis);
    if (Thread.interrupted()) {
      throw new InterruptedException();
    }
  }

  public void run()
  {
    if (target != null) {
      target.run();
    }
  }

  /*
   * Called by system when thread terminates (for whatever reason)
   */
  private void finish()
  {
    if (group != null) {
      group.remove(this);
    }
  }

  /**
   * @deprecated
   */
  public final void stop()
  {
    throw new Deprecated();
  }

  /**
   * @deprecated
   */
  public final synchronized void stop(Throwable o)
  {
    throw new Deprecated();
  }

  public void interrupt()
  {
    interrupting = true;
    interrupt0();
  }

  public static boolean interrupted() 
  {
    boolean i = Thread.currentThread().interrupting;
    Thread.currentThread().interrupting = false;
    return (i);
  }

  public boolean isInterrupted()
  {
    return (interrupting);
  }

  /**
   * @deprecated
   * Kill a thread immediately and don't try any kind of cleanup.
   */
  public void destroy()
  {
    destroy0();
  }

  /**
   * @deprecated
   */
  public final void suspend()
  {
    throw new Deprecated();
  }

  /**
   * @deprecated
   */
  public final void resume()
  {
    throw new Deprecated();
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
    System.getSecurityManager().checkAccess(this);
  }
  
  public String toString()
  {
    return getName();
  }

  /*
   * This method is used by java.lang.ThreadLocal. We don't
   * allocate the hashtable with each thread until we have to.
   */
  Hashtable getThreadLocals() {
    if (threadLocals == null) {
      threadLocals = new Hashtable();
    }
    return threadLocals;
  }

  private static native void sleep0(long millis);
  private native void interrupt0();
  private native void destroy0();
}

