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
import java.io.IOException;
import java.io.InputStream;
import java.io.PrintStream;
import java.lang.reflect.Array;
import java.util.Properties;
import java.util.PropertyPermission;

public final class System {
	final public static InputStream in;
	final public static PrintStream out;
	final public static PrintStream err;
	private static Properties props;
	private static SecurityManager security;

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

	props = initProperties(new Properties());

	// Initialise the I/O
	if (props.getProperty("kaffe.embedded", "false").equals("false")) {
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

public static void arraycopy(Object src, int src_position, Object dst, int dst_position, int length) {
	Class src_class = src.getClass();
	Class src_component = src_class.getComponentType();
	Class dst_class = dst.getClass();
	Class dst_component = dst_class.getComponentType();

	/* check for:
	 * src or dst not array
	 * src and dst reference vs. primitive type mismatch
	 * src and dst both primitive arrays, but for different primitives
	 */
	if (!src_class.isArray()
	    || !dst_class.isArray()
	    || (src_component.isPrimitive()
		&& !dst_component.isPrimitive())
	    || (!src_component.isPrimitive()
		&& dst_component.isPrimitive())
	    || (src_component.isPrimitive()
		&& dst_component.isPrimitive()
		&& src_component != dst_component)) {
		throw new ArrayStoreException("not assignable: source type is "
					      + src_class.getName()
					      + ", destination type is "
					      + dst_class.getName());
	}

	if (length < 0) {
		throw new ArrayIndexOutOfBoundsException();
	}

	if (src == dst && src_position < dst_position) {
		Object tmp = Array.newInstance(dst_component, length);
		System.arraycopy(src, src_position, tmp, 0, length);
		System.arraycopy(tmp, 0, dst, dst_position, length);
	}
	else if (dst instanceof Object[]) {
		Object[] d = (Object[])dst;
		Object[] s = (Object[])src;

		for (int i = length - 1; i >= 0; --i) {
			d[dst_position++] = s[src_position++];
		}
	}
	else if (dst instanceof int[]) {
		int[] d = (int[])dst;
		int[] s = (int[])src;

		for (int i = length - 1; i >= 0; --i) {
			d[dst_position++] = s[src_position++];
		}
	}
	else if (dst instanceof float[]) {
		float[] d = (float[])dst;
		float[] s = (float[])src;

		for (int i = length - 1; i >= 0; --i) {
			d[dst_position++] = s[src_position++];
		}
	}
	else if (dst instanceof long[]) {
		long[] d = (long[])dst;
		long[] s = (long[])src;

		for (int i = length - 1; i >= 0; --i) {
			d[dst_position++] = s[src_position++];
		}
	}
	else if (dst instanceof double[]) {
		double[] d = (double[])dst;
		double[] s = (double[])src;

		for (int i = length - 1; i >= 0; --i) {
			d[dst_position++] = s[src_position++];
		}
	}
	else if (dst instanceof boolean[]) {
		boolean[] d = (boolean[])dst;
		boolean[] s = (boolean[])src;

		for (int i = length - 1; i >= 0; --i) {
			d[dst_position++] = s[src_position++];
		}
	}
	else if (dst instanceof byte[]) {
		byte[] d = (byte[])dst;
		byte[] s = (byte[])src;

		for (int i = length - 1; i >= 0; --i) {
			d[dst_position++] = s[src_position++];
		}
	}
	else if (dst instanceof char[]) {
		char[] d = (char[])dst;
		char[] s = (char[])src;

		for (int i = length - 1; i >= 0; --i) {
			d[dst_position++] = s[src_position++];
		}
	}
	else if (dst instanceof short[]) {
		short[] d = (short[])dst;
		short[] s = (short[])src;

		for (int i = length - 1; i >= 0; --i) {
			d[dst_position++] = s[src_position++];
		}
	}
}

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

	return props;
}

public static String getProperty(String key) {
	checkPropertyAccess(key);

	return props.getProperty(key);
}

public static String getProperty(String key, String def) {
	checkPropertyAccess(key);

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
	Runtime.getRuntime().load(filename,
	    Class.CallStack.getCallersClassLoader());
}

public static void loadLibrary(String libname) {
	Runtime.getRuntime().loadLibrary(libname,
	    Class.CallStack.getCallersClassLoader());
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
	return (String)props.setProperty(key, value);
}

public static void setProperties(Properties prps) {
	checkPropertiesAccess();
	if (prps == null) {
		props.clear();
		return;
	}
	props = prps;
}

public static void setSecurityManager(SecurityManager s) {
	if (security != null) {
		security.checkPermission(
			new RuntimePermission("setSecurityManager"));
	}
	security = s;
}
}
