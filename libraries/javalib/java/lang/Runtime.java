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
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.FileNotFoundException;

import java.util.StringTokenizer;

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

public void exit(int status) {
	SecurityManager sm = System.getSecurityManager();
	if (sm != null)
		sm.checkExit(status);
	// Handle application extensions - if this thread is part of an
	// application then we exit that rather than the whole thing.
	if (!kaffe.lang.Application.exit(status)) {
		exitInternal(status);
	}
	// kaffe.lang.Application.exit does not destroy the thread
	// that invoked exit().  We stop that thread now.
	Thread.currentThread().destroy();
}

public void halt(int status) {
	exitInternal(status);
}

native private void exitInternal(int status);

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
		    Class.CallStack.getCallersClassLoader());
}

void loadLibrary(String libname, ClassLoader loader) {
	SecurityManager sm = System.getSecurityManager();
	if (sm != null)
		sm.checkLink(libname);
	String errmsg = libname + ": not found";
	String filename;
	String[] names;

	if (loader != null && (filename = loader.findLibrary(libname)) != null)
		names = new String[] { filename };
	else
		names = NativeLibrary.getLibraryNames(libname);
	for (int i = 0; i < names.length; i++) {
		try {
			new NativeLibrary(names[i], loader);
			return;
		} catch (FileNotFoundException e) {
		} catch (UnsatisfiedLinkError e) {
			errmsg = e.getMessage();
		}
	}
	throw new UnsatisfiedLinkError(errmsg + "\nAdding its directory to LD_LIBRARY_PATH may help.");
}

public void load(String filename) {
	load(filename, Class.CallStack.getCallersClassLoader());
}

void load(String filename, ClassLoader loader) {
	SecurityManager sm = System.getSecurityManager();
	if (sm != null)
		sm.checkLink(filename);
	try {
		new NativeLibrary(filename, loader);
	}
	catch (FileNotFoundException e) {
		throw new UnsatisfiedLinkError(filename + ": not found");
	}
}

int getMemoryAdvice() {
	return (advice.getColor());
}

int waitForMemoryAdvice(int level) throws InterruptedException {
	return (advice.waitForOtherColor(level));
}

public void addShutdownHook(Thread hook) {
	// XXX implement me
	throw new kaffe.util.NotImplemented(
	    getClass().getName() + ".addShutdownHook()");
}

public boolean removeShutdownHook(Thread hook) {
	// XXX implement me
	return false;
}

native public void runFinalization();

native public static void runFinalizersOnExit(boolean value);

native public long totalMemory();

native public void traceInstructions(boolean on);

native public void traceMethodCalls(boolean on);
}
