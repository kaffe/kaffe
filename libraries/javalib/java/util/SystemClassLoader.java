/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.util;

import java.io.InputStream;
import java.net.URL;

public class SystemClassLoader
  extends ClassLoader
{

static private ClassLoader singleton;

static {
	singleton = new SystemClassLoader();
}

public static ClassLoader getClassLoader() {
	return (singleton);
}

public URL getResource(String name) {
	return (getSystemResource(name));
}

public InputStream getResourceAsStream(String name) {
	return (getSystemResourceAsStream(name));
}

public Class loadClass(String name, boolean resolve) throws ClassNotFoundException {
	Class cls = findSystemClass(name);
	resolveClass(cls);
	return (cls);
}
}
