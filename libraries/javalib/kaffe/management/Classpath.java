/*
 * Classpath.java
 * Interface to KVM class path management.
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package kaffe.management;

import java.util.Properties;

public class Classpath {

	static {
		System.loadLibrary("management");
	}

	public static void add(String elem) {

		// Add to internal classpath
		add0(elem);

		// Add to classpath property
		Properties p = System.getProperties();
		String sep = p.getProperty("path.separator");
		String cpath = p.getProperty("java.class.path");
		p.setProperty("java.class.path", cpath + sep + elem);
	}

	public static void prepend(String elem) {

		// Add to internal classpath
		prepend0(elem);

		// Add to classpath property
		Properties p = System.getProperties();
		String sep = p.getProperty("path.separator");
		String cpath = p.getProperty("java.class.path");
		p.setProperty("java.class.path", elem + sep + cpath);
	}

	public native static void add0(String elem);

	public native static void prepend0(String elem);

/**
 * Print out the classpath.
 */
public static void main(String[] args) {
	System.out.println(System.getProperty("java.class.path"));
}

}
