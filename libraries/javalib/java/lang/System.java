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

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.FileDescriptor;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.PrintStream;
import java.util.Properties;
import java.util.PropertyPermission;

import kaffe.lang.ThreadStack;

public final class System {
	final public static InputStream in;
	final public static PrintStream out;
	final public static PrintStream err;
	static Properties properties;

// When trying to debug Java code that gets executed early on during
// JVM initialization, eg, before System.err is initialized, debugging
// println() statements don't work. In these cases, the following routines
// are very handy. Simply uncomment the following two lines to enable them.
/****
public static native void debug(String s);	// print s to stderr, then \n
public static native void debugE(Throwable t);	// print stack trace to stderr
****/
static {
	// XXX what are the constraints on the initialization order in here?

	properties = initProperties(new Properties());

	// Initialise the I/O
	if (properties.getProperty("kaffe.embedded", "false").equals("false")) {
		in = new BufferedInputStream(new FileInputStream(FileDescriptor.in), 128);
		out = new PrintStream(new BufferedOutputStream(new FileOutputStream(FileDescriptor.out), 128), true);
		err = new PrintStream(new BufferedOutputStream(new FileOutputStream(FileDescriptor.err), 128), true);
	} else {
		in = new BufferedInputStream(new kaffe.io.StdInputStream(), 128);
		out = new PrintStream(new BufferedOutputStream(new kaffe.io.StdOutputStream(), 128), true);
		err = new PrintStream(new BufferedOutputStream(new kaffe.io.StdErrorStream(), 128), true);
	}
}

private System() { }

public static native void arraycopy(Object src, int src_position, Object dst, int dst_position, int length);

private static void checkPropertiesAccess() {
	SecurityManager sm = getSecurityManager();
	if (sm != null)
		sm.checkPropertiesAccess();
}

private static void checkPropertyAccess(String key) {
	SecurityManager sm = getSecurityManager();
	if (key.length() == 0)
		throw new IllegalArgumentException("key can't be empty");
	if (sm != null)
		sm.checkPropertyAccess(key);
}

native public static long currentTimeMillis();

public static void exit (int status) {
	Runtime.getRuntime().exit(status);
}

public static void gc() {
	Runtime.getRuntime().gc();
}

public static Properties getProperties() {
	checkPropertiesAccess();

	return properties;
}

public static String getProperty(String key) {
	checkPropertyAccess(key);

	return properties.getProperty(key);
}

public static String getProperty(String key, String def) {
	checkPropertyAccess(key);

	return properties.getProperty(key, def);
}

public static SecurityManager getSecurityManager() {
	return Runtime.securityManager;
}

public static String getenv(String name) {
	throw new Error("System.getenv no longer supported");
}

native public static int identityHashCode(Object x);

native private static Properties initProperties(Properties properties);

public static void load(String filename) {
	Runtime.getRuntime().load(filename,
	    ThreadStack.getCallersClassLoader(false));
}

public static void loadLibrary(String libname) {
	Runtime.getRuntime().loadLibrary(libname,
	    ThreadStack.getCallersClassLoader(false));
}

public static String mapLibraryName(String fn) {
	return NativeLibrary.getLibPrefix() + fn + NativeLibrary.getLibSuffix();
}

public static void runFinalization() {
	Runtime.getRuntime().runFinalization();
}

public static void runFinalizersOnExit(boolean value) {
	Runtime.getRuntime().runFinalizersOnExit(value);
}

private static void exitJavaCleanup() {
	Runtime.getRuntime().exitJavaCleanup();
}

public static void setErr(PrintStream err) {
	// XXX call security manager for RuntimePermission("SetIO")
	setErr0(err);
}

native private static void setErr0(PrintStream err);

public static void setIn(InputStream in) {
	// XXX call security manager for RuntimePermission("SetIO")
	setIn0(in);
}

native private static void setIn0(InputStream in);

public static void setOut(PrintStream out) {
	// XXX call security manager for RuntimePermission("SetIO")
	setOut0(out);
}

native private static void setOut0(PrintStream out);

public static String setProperty(String key, String value) {
	SecurityManager sm = getSecurityManager();
	if (sm != null)
		sm.checkPermission(new PropertyPermission(key, "write"));
	if (key.length() == 0)
		throw new IllegalArgumentException("key can't be empty");
	return (String)properties.setProperty(key, value);
}

public static void setProperties(Properties prps) {
	checkPropertiesAccess();
	if (prps == null) {
		properties.clear();
		return;
	}
	properties = prps;
}

public static void setSecurityManager(SecurityManager s) {
	if (Runtime.securityManager != null) {
		Runtime.securityManager.checkPermission(
			new RuntimePermission("setSecurityManager"));
	}
	Runtime.securityManager = s;
}
}
