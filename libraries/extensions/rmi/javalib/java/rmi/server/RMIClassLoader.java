/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license-lesser.terms" for information on usage and 
 * redistribution of this file.
 */

package java.rmi.server;

import java.net.URL;
import java.net.URLConnection;
import java.io.IOException;
import java.io.DataInputStream;
import java.net.MalformedURLException;
import java.util.StringTokenizer;

public class RMIClassLoader {

static private class MyClassLoader extends ClassLoader {

Class defineClass(String name, byte[] data) {
	return (defineClass(name, data, 0, data.length));
}
}
static private MyClassLoader loader = new MyClassLoader();

/**
 * @deprecated
 */
public static Class loadClass(String name) throws MalformedURLException, ClassNotFoundException {
	return (loadClass(System.getProperty("java.rmi.server.codebase"), name));
}

public static Class loadClass(URL codebase, String name) throws MalformedURLException, ClassNotFoundException {
	URL u = new URL(codebase, name + ".class");
	try {
		URLConnection conn = u.openConnection();
		DataInputStream strm = new DataInputStream(conn.getInputStream());
		byte data[] = new byte[conn.getContentLength()];
		strm.readFully(data);
		return (loader.defineClass(name, data));
	}
	catch (IOException _) {
		throw new ClassNotFoundException(name);
	}
}

public static Class loadClass(String codebase, String name) throws MalformedURLException, ClassNotFoundException {
	StringTokenizer tok = new StringTokenizer(codebase, ":");
	while (tok.hasMoreTokens()) {
		try {
			return (loadClass(new URL(tok.nextToken()), name));
		}
		catch (ClassNotFoundException _) {
			// Ignore - try the next one.
		}
	}
	throw new ClassNotFoundException(name);
}

public static String getClassAnnotation(Class cl) {
	return (null);	// We don't yet do this.
}

/**
 * @deprecated
 */
public static Object getSecurityContext(ClassLoader loader) {
	throw new kaffe.util.NotImplemented();
}

}
