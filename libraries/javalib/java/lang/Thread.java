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

import java.lang.Throwable;
import java.util.Enumeration;
import java.util.Hashtable;
import kaffe.lang.Application;
import kaffe.lang.ApplicationResource;

public class Thread
  implements Runnable, ApplicationResource {

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
private Throwable outOfMemoryError;
private boolean dying;
private Hashtable threadLocals;
private Object suspendResume;
private Object sleeper;
private Object holder;
private kaffe.util.Ptr sem;
private Thread nextlk;
private Throwable death;
private int needOnStack;
private int noStopCount = 0;

private static class Sleeper {
}
private static class Suspender {
}

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

	// make sure this.name is non-zero before calling addResource
	this.name = name.toCharArray();
	Application.addResource(this);
	this.target = target;
	this.interrupting = false;

	this.stackOverflowError = new StackOverflowError();
	this.outOfMemoryError = new OutOfMemoryError();

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

/*
 * This isn't infact a good idea since the Thread class isn't final and we can't
 * guarantee this is ever called - which seems like dumb semantics to me but there
 * you go.
 */
protected void finalize() throws Throwable { 
	finalize0();
	super.finalize();
}

final native private void finalize0();

/*
 * Called by system when thread terminates (for whatever reason)
 */
private void finish() {
	synchronized(this) {
		dying = true;
		notifyAll();     // in case somebody is joining on us
	}
	if (group != null) {
		group.remove(this);
	}
	Application.removeResource(this);
}

public void freeResource() {
	// NB:
	// Each thread is associated with the application that created it
	// as an ApplicationResource.
	// If an application exits, it frees all its resources, thereby
	// destroying all its threads.
	// To make sure we free all threads, we must make sure that the
	// invoking thread is not killed prematurely before it can finish
	// that task.
	// See also Runtime.exit()
        if (isAlive() && currentThread() != this) {
		destroy();
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
	Object h = holder;
	if (h != null) {
		holder = null;
		synchronized (h) {
			h.notify();
		}
	}
}

public static boolean interrupted() {
	Thread curr = Thread.currentThread();
	boolean i = curr.interrupting;
	curr.interrupting = false;
	return (i);
}

final public boolean isAlive () {
	return (!dying);
}

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
	Thread curr = currentThread();

	/* Wait while time remains and alive */
	if (millis == 0 && nanos == 0) {
		while (isAlive()) {
			curr.waitOn(this, 0);
		}
	}
	else {
		long end = System.currentTimeMillis() + millis;
		for (;;) {
			long remain = end - System.currentTimeMillis();
			if (!isAlive() || remain <= 0) {
				break;
			}
			curr.waitOn(this, remain);
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

public static void sleep(long millis) throws InterruptedException {
	Thread curr = Thread.currentThread();
	if (curr.sleeper == null) {
		curr.sleeper = new Sleeper();
	}
	/* If we're sleeping for 0 time, then we'll really sleep for a very
	 * short time instead.
	 */
	if (millis == 0) {
		millis = 1;
	}
	curr.waitOn(curr.sleeper, millis);
}

public static void sleep(long millis, int nanos) throws InterruptedException
{
	sleep(millis);
}

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
	death = o;
	Object h = holder;
	if (h != null) {
		holder = null;
		synchronized (h) {
			h.notify();
		}
	}
}

/**
 * @deprecated
 */
final public void suspend() {
	if (suspendResume == null) {
		suspendResume = new Suspender();
	}
	if (Thread.currentThread() != this) {
		throw new kaffe.util.Deprecated("suspending of other threads not supported");
	}
	synchronized (suspendResume) {
		for (;; ) {
			try {
				waitOn(suspendResume, 0);
				break;
			}
			catch (InterruptedException _) {
				/* We cannot interrupt a suspend */
			}
		}
	}
}

public String toString() {
	return getName();
}

void waitOn(Object hold, long timeout) throws InterruptedException {
	if (timeout < 0) {
		throw new IllegalArgumentException("timeout is negative");
	}
	if (interrupting) {
		interrupting = false;
		throw new InterruptedException();
	}
	if (death != null) {
		Error t = (Error)death;
		death = null;
		throw t;
	}
	holder = hold;
	try {
		synchronized (hold) {
			hold.wait0(timeout);
		}
	}
	/* This is a hack to deal with the fact that Kaffe doesn't correctly
	 * release the 'hold' lock when handing ThreadDeath - hence when it
	 * exits the syncronized block we throw an exception.  Here we catch
	 * this and throw a new ThreadDeath which is why it happens.
	 */
	catch (IllegalMonitorStateException _) {
		throw new ThreadDeath();
	}
	finally {
		holder = null;
	}
	if (interrupting) {
		interrupting = false;
		throw new InterruptedException();
	}
	if (death != null) {
		Error t = (Error)death;
		death = null;
		throw t;
	}
}

public static void yield() {
	yield0();
	Thread curr = Thread.currentThread();
	if (curr.death != null) {
		Error t = (Error)curr.death;
		curr.death = null;
		throw t;
	}
}

native public static void yield0();

}
