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

import java.util.Enumeration;
import java.util.Hashtable;
import java.lang.Throwable;

public class Thread
  implements Runnable {

	final public static int MIN_PRIORITY = 1;
	final public static int NORM_PRIORITY = 5;
	final public static int MAX_PRIORITY = 10;
	private static int threadCount;
	private char[] name;
	private int priority;
	private Thread threadQ;
	private kaffe.util.Ptr PrivateInfo;
	private boolean daemon;
	private boolean interrupting;
	private Runnable target;
	private ThreadGroup group;
	private kaffe.util.Ptr exceptPtr;
	private Throwable exceptObj;
	private kaffe.util.Ptr jnireferences;
	private Throwable stackOverflowError;
	private int needOnStack;
	private boolean dying;
	private Hashtable threadLocals;
	private Object suspendResume;

public Thread() {
	this (null, null, generateName());
}

public Thread(Runnable target) {
	this (null, target, generateName());
}

public Thread(Runnable target, String name) {
	this (null, target, name);
}

public Thread(String name) {
	this (null, null, name);
}

public Thread(ThreadGroup group, Runnable target) {
	this (group, target, generateName());
}

public Thread(ThreadGroup group, Runnable target, String name) {
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

public Thread(ThreadGroup group, String name) {
	this (group, null, name);
}

public static int activeCount() {
	return (Thread.currentThread().getThreadGroup().activeCount());
}

public void checkAccess() {
	System.getSecurityManager().checkAccess(this);
}

native public int countStackFrames();

native public static Thread currentThread();

/**
 * Kill a thread immediately and don't try any kind of cleanup except removing it
 * from the thread group.
 */
public void destroy() {
	if (group != null) {
		group.remove(this);
	}
	destroy0();
}

native private void destroy0();

public static void dumpStack() {
	Throwable t = new Throwable();
	t.printStackTrace();
}

public static int enumerate(Thread tarray[]) {
	return (Thread.currentThread().getThreadGroup().enumerate(tarray));
}

final native private void finalize0();

/*
 * Called by system when thread terminates (for whatever reason)
 */
private void finish() {
	if (group != null) {
		group.remove(this);
	}
}

private static String generateName() {
	return new String("Thread-"+threadCount++);
}

final public String getName() {
	return (new String(name));
}

final public int getPriority() {
	return priority;
}

final public ThreadGroup getThreadGroup() {
	return (group);
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

public void interrupt() {
	interrupting = true;
	interrupt0();
}

native private void interrupt0();

public static boolean interrupted() {
	boolean i = Thread.currentThread().interrupting;
	Thread.currentThread().interrupting = false;
	return (i);
}

final native public boolean isAlive();

final public boolean isDaemon() {
	return daemon;
}

public boolean isInterrupted() {
	return (interrupting);
}

final public void join() throws InterruptedException
{
	join(0);
}

final public synchronized void join(long millis) throws InterruptedException
{
	join(millis, 0);
}

final public synchronized void join(long millis, int nanos) throws InterruptedException
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

/**
 * @deprecated
 */
final public void resume() {
	if (suspendResume != null) {
		synchronized (suspendResume) {
			suspendResume.notifyAll();
		}
	}
}

public void run() {
	if (target != null) {
		target.run();
	}
}

final public void setDaemon(boolean on) {
	daemon = on;
}

final public void setName(String name) {
	this.name = name.toCharArray();
}

final public void setPriority(int newPriority) {
	checkAccess();

	if (newPriority < MIN_PRIORITY || newPriority > group.getMaxPriority()) {
		throw new IllegalArgumentException();
	}
	setPriority0(newPriority);
}

native private void setPriority0(int prio);

public static void sleep(long millis) throws InterruptedException
{
	if (Thread.interrupted()) {
		throw new InterruptedException();
	}
	sleep0(millis);
	if (Thread.interrupted()) {
		throw new InterruptedException();
	}
}

public static void sleep(long millis, int nanos) throws InterruptedException
{
	sleep(millis);
}

native private static void sleep0(long millis);

native public synchronized void start();

/**
 * @deprecated
 */
final public void stop() {
	stop(new ThreadDeath());
}

/**
 * @deprecated
 */
final public synchronized void stop(Throwable o) {
	stop0(o);
}

native private void stop0(Object o);

/**
 * @deprecated
 */
final public void suspend() {
	if (suspendResume == null) {
		suspendResume = new Object();
	}
	synchronized (suspendResume) {
		try {
			suspendResume.wait();
		}
		catch (InterruptedException _) {
		}
	}
}

public String toString() {
	return getName();
}

native public static void yield();
}
