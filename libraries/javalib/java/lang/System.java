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
import java.util.GregorianCalendar;
import java.util.Properties;
import java.util.SimpleTimeZone;

final public class System
{
	final static SecurityManager defaultSecurityManager = new NullSecurityManager();
	public static InputStream in;
	public static PrintStream out;
	public static PrintStream err;
	private static Properties props;
	private static SecurityManager security;

static {
	security = defaultSecurityManager;

	props = initProperties(new Properties());

	// Initiate the timezone & calendar.
	new SimpleTimeZone(0, "GMT");
	new GregorianCalendar();

	// Initialise the I/O
	in = new BufferedInputStream(new FileInputStream(FileDescriptor.in), 128);
	out = new PrintStream(new BufferedOutputStream(new FileOutputStream(FileDescriptor.out), 128), true);
	err = new PrintStream(new BufferedOutputStream(new FileOutputStream(FileDescriptor.err), 128), true);	
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

public static void setErr(PrintStream err) {
	System.err = err;
}

public static void setIn(InputStream in) {
	System.in = in;
}

public static void setOut(PrintStream out) {
	System.out = out;
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
