/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package kaffe.lang;

import java.io.InputStream;
import java.net.URL;
import java.lang.ClassLoader;
import java.lang.String;
import java.lang.ClassNotFoundException;

public class SystemClassLoader extends ClassLoader {

private static ClassLoader singleton = new SystemClassLoader();

public static ClassLoader getClassLoader() {
	return (singleton);
}

public URL getResource(String name) {
	return (getSystemResource(name));
}

public InputStream getResourceAsStream(String name) {
	return (getSystemResourceAsStream(name));
}

protected Class loadClass(String name, boolean resolve) throws ClassNotFoundException {
	Class cls = findLoadedClass(name);
	if (cls == null) {
		cls = findSystemClass(name);
	}
	if (resolve) {
		resolveClass(cls);
	}
	return (cls);
}

}
