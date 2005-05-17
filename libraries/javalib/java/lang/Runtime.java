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

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Enumeration;
import java.util.StringTokenizer;
import java.util.Vector;

import kaffe.lang.ThreadStack;

public class Runtime
{
/**
 *  XXX This is not part of the public interface.
 */
public interface MemoryAdvice {

	// These should match those in kaffe.lang.MemoryAdvice
	int GREEN = 0;
	int YELLOW = 1;
	int ORANGE = 2;
	int RED = 3;
}

private static Runtime currentRuntime = new Runtime();
private static kaffe.lang.MemoryAdvice advice
	= kaffe.lang.MemoryAdvice.getInstance();
private static final Vector shutdownHooks = new Vector(0);
private static boolean VMShuttingDown = false;

private static final RuntimePermission SHUTDOWN_HOOKS =
	new RuntimePermission("shutdownHooks");

private Runtime () {
}

public Process exec(String command) throws IOException {
	return exec(command, null, null);
}

public Process exec(String command, String envp[]) throws IOException {
	return exec(command, envp, null);
}

public Process exec(String command, String envp[], File dir)
		throws IOException {
	StringTokenizer tokenizer=new StringTokenizer(command);

	int count=tokenizer.countTokens();
	String cmdarray[]=new String[count];

	for (int pos=0; pos<count; pos++) {
		cmdarray[pos]=tokenizer.nextToken();
	}

	return exec(cmdarray, envp, dir);
}

public Process exec(String cmdarray[]) throws IOException {
	return exec(cmdarray, null, null);
}

public Process exec(String cmdarray[], String envp[]) throws IOException {
	return exec(cmdarray, envp, null);
}

public Process exec(String[] cmdarray, String[] envp, File dir)
		throws IOException {
	SecurityManager sm = System.getSecurityManager();
	if (sm != null)
		sm.checkExec(cmdarray[0]);
	return execInternal(cmdarray, envp, dir);
}

private native Process execInternal(String cmdary[], String envp[], File dir)
	throws IOException;

/**
 * Callback used to execute the shutdown hooks.
 */
boolean exitJavaCleanup() {
	return runShutdownHooks();
}

public void exit(int status) throws SecurityException {
	SecurityManager sm = System.getSecurityManager();
	if (sm != null)
		sm.checkExit(status);

	/* First we cleanup the Virtual Machine */
	if (!exitJavaCleanup())
		/* Throw ThreadDeath to kill the currently running thread. */
		throw new ThreadDeath();

	/* Now we run the VM exit function */
	exit0(status);
}

public void halt(int status) throws SecurityException {
	SecurityManager sm = System.getSecurityManager();
	if (sm != null)
		sm.checkExit(status);

	exit0(status);
}

native private void exit0(int status);

native public long freeMemory();

native public void gc();

/**
 * @deprecated
 */
public InputStream getLocalizedInputStream(InputStream in) {
	return (in);
}

/**
 * @deprecated
 */
public OutputStream getLocalizedOutputStream(OutputStream out) {
	return (out);
}

public static Runtime getRuntime() {
	return currentRuntime;
}

public void loadLibrary(String libname) {
	loadLibrary(libname,
		    ThreadStack.getCallersClassLoader(false));
}

   /**
    * Tries to load and link a native library.
    *
    * @return true if successfull
    * @throws UnsatisfiedLinkError if the library was found but could not be linked.
    */
   private static native boolean linkLibrary(String name, ClassLoader loader)
      throws UnsatisfiedLinkError;

   private static native String getLibPrefix ();
   private static native String getLibSuffix ();

   static String mapLibraryName (String name)
   {
      return getLibPrefix() + name + getLibSuffix();
   }

void loadLibrary(String libname, ClassLoader loader) throws UnsatisfiedLinkError {
	SecurityManager sm = System.getSecurityManager();
	if (sm != null)
		sm.checkLink(libname);
	String errmsg = libname + ": not found";
	String filename;
	String[] names;

	if (loader != null && (filename = loader.findLibrary(libname)) != null) {
		if (linkLibrary(filename, loader))
			return;
	} else {
		String libPath = System.getProperty("kaffe.library.path")
				 + File.pathSeparatorChar
				 + System.getProperty("java.library.path");

		StringTokenizer t = new StringTokenizer(libPath,
					new String(new char[] { File.pathSeparatorChar }));
		while (t.hasMoreTokens ()) {
			if (linkLibrary(t.nextToken() + File.separator + mapLibraryName(libname),
					loader))
				return;
		}
	}
	throw new UnsatisfiedLinkError("Could not find library '" + libname +
				       "'\nAdding its directory to LD_LIBRARY_PATH may help.");
}

public void load(String filename) {
	load(filename, ThreadStack.getCallersClassLoader(false));
}

void load(String filename, ClassLoader loader) {
	SecurityManager sm = System.getSecurityManager();
	if (sm != null)
		sm.checkLink(filename);

	if (!linkLibrary(filename, loader))
		throw new UnsatisfiedLinkError(filename + ": not found");
}

native public long maxMemory();

int getMemoryAdvice() {
	return (advice.getColor());
}

int waitForMemoryAdvice(int level) throws InterruptedException {
	return (advice.waitForOtherColor(level));
}

/**
 * Mark the VM as shutting down and walk the list of hooks.
 */
private boolean runShutdownHooks() {
       Enumeration hook_enum;
       
       /* According to Java 1.3 we need to run all hooks simultaneously
	* and then wait for them.
	*/
       synchronized (this) {
	       if( VMShuttingDown )
	       {
		       /*
			* Another thread called exit(), ignore and kill the
			* thread.
			*/
		       /* throw new ThreadDeath();
			* We must not throw ThreadDeath because we are currently
			* shutting down a thread (in an exitThread call) this may
			* cause the VM to crash because it detects an infinite loop.
			* Moreover we may want to add some more termination system
			* later. Here we will just return.
			*/
		       return false;
	       }
	       else
	       {
		       VMShuttingDown = true;
	       }
       }
       /* We start all threads at once as in the specification */
       hook_enum = shutdownHooks.elements();
       while (hook_enum.hasMoreElements()) {
	       Thread hook = (Thread)hook_enum.nextElement();

	       try {
		       hook.start();
	       } catch (Exception e) {
		       e.printStackTrace();
	       }
       }

       /* Now we wait for each thread */
       hook_enum = shutdownHooks.elements();
       while (hook_enum.hasMoreElements()) {
	       Thread hook = (Thread)hook_enum.nextElement();
	       boolean join_interrupted = false;

	       /* XXX Should this timeout? */
	       do
	       {
		       join_interrupted = false;
		       try {
			       hook.join();
		       } catch (InterruptedException _) {
			       join_interrupted = true;
		       } catch (Exception e) {
			       e.printStackTrace();
		       }
	       }
	       while (join_interrupted);
       }
       return true;
}

public void addShutdownHook(Thread hook) throws IllegalArgumentException, IllegalStateException {
	SecurityManager sm = System.getSecurityManager();
	if (sm != null)
		sm.checkPermission(SHUTDOWN_HOOKS);

	synchronized(this) {
		if (VMShuttingDown)
			throw new IllegalStateException("VM is shutting down.");
	}
	if (hook.isAlive() || hook.isInterrupted() || hook.getThreadGroup() == null)
		throw new IllegalArgumentException("Thread has already been started once.");

	synchronized( shutdownHooks ) {
		if (shutdownHooks.contains(hook))
			throw new IllegalArgumentException("Thread already in shutdown queue.");
		shutdownHooks.addElement(hook);
	}
}

public boolean removeShutdownHook(Thread hook) throws IllegalStateException {
	SecurityManager sm = System.getSecurityManager();
	if (sm != null)
		sm.checkPermission(SHUTDOWN_HOOKS);

	synchronized(this) {
		if (VMShuttingDown)
			throw new IllegalStateException("VM is shutting down.");
	}

	return shutdownHooks.removeElement(hook);
}

private static void exitJavaCleanupHook() {
	Runtime.getRuntime().exitJavaCleanup();
}

native public void runFinalization();

native public static void runFinalizersOnExit(boolean value);

native public long totalMemory();

native public void traceInstructions(boolean on);

native public void traceMethodCalls(boolean on);
}
