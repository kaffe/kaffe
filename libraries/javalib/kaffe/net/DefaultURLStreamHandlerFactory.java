/*
 * Java core library component.
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package kaffe.net;

import java.lang.String;
import java.lang.System;
import java.net.URLStreamHandlerFactory;
import java.net.URLStreamHandler;
import java.util.StringTokenizer;
import java.util.Hashtable;

public class DefaultURLStreamHandlerFactory implements URLStreamHandlerFactory {

final private static String defaultName = "kaffe.net.www.protocol.";
final private static String tvtName = "com.transvirtual.net.www.protocol.";

private Hashtable cache = new Hashtable();

public URLStreamHandler createURLStreamHandler(String protocol)
{
	URLStreamHandler handler;

	handler = (URLStreamHandler)cache.get(protocol);
	if (handler != null) {
		return (handler);
	}

	String pkgs = System.getProperty("java.protocol.handler.pkgs");
	if (pkgs != null) {
		StringTokenizer tokenizer = new StringTokenizer(pkgs, "|");
		while (tokenizer.hasMoreTokens()) {
			String theClass = tokenizer.nextToken();
			handler = tryClass(theClass + "." + protocol + ".Handler");
			if (handler != null) {
				cache.put(protocol, handler);
				return (handler);
			}
		}
	}

	/* Try the TVT default name */
	String classPath = tvtName + protocol + ".Handler";
	handler = tryClass(classPath);
	if (handler != null) {
		cache.put(protocol, handler);
		return (handler);
	}

	/* Try the default name */
	classPath = defaultName + protocol + ".Handler";
	handler = tryClass(classPath);
	if (handler != null) {
		cache.put(protocol, handler);
		return (handler);
	}

	return (null);
}

private static URLStreamHandler tryClass(String theClass) {
	try {
		Class cl = Class.forName(theClass);
		Object obj = cl.newInstance();
		if (obj instanceof URLStreamHandler) {
			return (URLStreamHandler )obj;
		}
	}
	catch (UnsatisfiedLinkError e) {
	}
	catch (ClassNotFoundException e) {
	}
	catch (IllegalAccessException e) {
	}
	catch (InstantiationException e) {
	}
	return (null);
}

}
