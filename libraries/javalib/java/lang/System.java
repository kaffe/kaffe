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

final public class System
{
	final static SecurityManager defaultSecurityManager = new NullSecurityManager();
	final public static InputStream in;
	final public static PrintStream out;
	final public static PrintStream err;
	private static Properties props;
	private static SecurityManager security;

static {
		// XXX what are the constraints on the initialization order in here?

	security = defaultSecurityManager;

	props = initProperties(new Properties());

	// Initialise the I/O
	in = new BufferedInputStream(new FileInputStream(FileDescriptor.in), 128);
	out = new PrintStream(new BufferedOutputStream(new FileOutputStream(FileDescriptor.out), 128), true);
	err = new PrintStream(new BufferedOutputStream(new FileOutputStream(FileDescriptor.err), 128), true);	

	// Initiate the default timezone implementation & default calendar implementation.  
	try
	{
		Class.forName("java.util.SimpleTimeZone");
		Class.forName("java.util.GregorianCalendar");
	}
	catch (ClassNotFoundException _)
	{
		// Kaffe won't let exceptions be thrown this early in
		// the init process, anyway...
	}
}

private System() { }

native public static void arraycopy(Object src, int src_position, Object dst, int dst_position, int length);

private static void checkPropertyAccess() {
	getSecurityManager().checkPropertiesAccess();
}

native public static long currentTimeMillis();

public static void exit (int status) {
	Runtime.getRuntime().exit(status);
}

public static void gc() {
	Runtime.getRuntime().gc();
}

public static Properties getProperties() {
	checkPropertyAccess();

	return props;
}

public static String getProperty(String key) {
	checkPropertyAccess();

	return getProperty(key, null);
}

public static String getProperty(String key, String def) {
	checkPropertyAccess();

	return props.getProperty(key, def);
}

public static SecurityManager getSecurityManager() {
	return security;
}

public static String getenv(String name) {
	throw new Error("System.getenv no longer supported");
}

native public static int identityHashCode(Object x);

native private static Properties initProperties(Properties props);

public static void load(String filename) {
	Runtime.getRuntime().load(filename);
}

public static void loadLibrary(String libname) {
	Runtime.getRuntime().loadLibrary(libname);
}

public static void runFinalization() {
	Runtime.getRuntime().runFinalization();
}

public static void runFinalizersOnExit(boolean value) {
	Runtime.getRuntime().runFinalizersOnExit(value);
}

native private static void setIn0(InputStream in);
native private static void setOut0(PrintStream out);
native private static void setErr0(PrintStream err);

public static void setErr(PrintStream err) {
	// XXX call security manager for RuntimePermission("SetIO")
	setErr0(err);
}

public static void setIn(InputStream in) {
	// XXX call security manager for RuntimePermission("SetIO")
	setIn0(in);
}

public static void setOut(PrintStream out) {
	// XXX call security manager for RuntimePermission("SetIO")
	setOut0(out);
}

public static void setProperties(Properties prps) {
	checkPropertyAccess();

	props = prps;
}

public static void setSecurityManager(SecurityManager s) {
	if (security != defaultSecurityManager) {
		throw new SecurityException();
	}
	if (s != null) {
		security = s;
	}
}
}
