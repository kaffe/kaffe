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

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.StringTokenizer;

public class Runtime
{
/**
 *  This is not part of the public interface.
 */
public static interface MemoryAdvice {

// These should match those in kaffe.lang.MemoryAdvice
public static final int GREEN = 0;
public static final int YELLOW = 1;
public static final int ORANGE = 2;
public static final int RED = 3;

}

private static Runtime currentRuntime = new Runtime();
private String[] paths = null;
private static kaffe.lang.MemoryAdvice advice = kaffe.lang.MemoryAdvice.getInstance();

private Runtime () {
	String pathSpec = initializeLinkerInternal();

	if ( pathSpec != null ) {
		StringTokenizer tk = new StringTokenizer( pathSpec, System.getProperty("path.separator"));
		paths = new String[tk.countTokens()];

		for (int pos = 0; tk.hasMoreTokens(); pos++) {
			paths[pos] = tk.nextToken();
		}
	}
}

native private String  buildLibName(String path, String name);

public Process exec(String command) throws IOException {
	return exec(command, null);
}

public Process exec(String command, String envp[]) throws IOException {
	StringTokenizer tokenizer=new StringTokenizer(command);

	int count=tokenizer.countTokens();
	String cmdarray[]=new String[count];

	for (int pos=0; pos<count; pos++) {
		cmdarray[pos]=tokenizer.nextToken();
	}

	return exec(cmdarray, envp);
}

public Process exec(String cmdarray[]) throws IOException {
	return exec(cmdarray, null);
}

public Process exec(String cmdarray[], String envp[]) throws IOException {
	System.getSecurityManager().checkExec(cmdarray[0]);
	return execInternal(cmdarray, envp);
}

native private Process execInternal(String cmdarray[], String envp[]) throws IOException;

public void exit(int status) {
	System.getSecurityManager().checkExit(status);
	// Handle application extensions - if this thread is part of an
	// application then we exit that rather than the whole thing.
	if (kaffe.lang.Application.exit(status) == false) {
		exitInternal(status);
	}
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

private void initPaths() {
	StringTokenizer tk = new StringTokenizer(initializeLinkerInternal(), System.getProperty("path.separator"));

	paths = new String[tk.countTokens()];

	for (int pos = 0; tk.hasMoreTokens(); pos++) {
		paths[pos] = tk.nextToken();
	}
}

native private String  initializeLinkerInternal();

public synchronized void load(String filename) {
	if (loadInternal(filename) == false) {
		throw new UnsatisfiedLinkError(filename);
	}
}

native private boolean loadFileInternal(String filename);

private boolean loadInternal(String filename) {
	System.getSecurityManager().checkLink(filename);
	return loadFileInternal(filename);
}

public synchronized void loadLibrary(String libname) {
	if ( paths != null ) {
		/* Try library for each path */
		for (int path = 0; path < paths.length; path++) {
			if (loadInternal(buildLibName(paths[path], libname)) == true) {
				return;
			}
		}

		/* Not found */
		throw new UnsatisfiedLinkError(libname);
	}
	// otherwise we don't have external libraries at all
}

int getMemoryAdvice() {
	return (advice.getColor());
}

int waitForMemoryAdvice(int level) throws InterruptedException {
	return (advice.waitForOtherColor(level));
}

native public void runFinalization();

native public static void runFinalizersOnExit(boolean value);

native public long totalMemory();

native public void traceInstructions(boolean on);

native public void traceMethodCalls(boolean on);
}
