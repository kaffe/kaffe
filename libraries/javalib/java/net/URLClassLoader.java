
/*
 * Java core library component.
 *
 * Copyright (c) 1999
 *	Archie L. Cobbs.  All rights reserved.
 * Copyright (c) 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Author: Archie L. Cobbs <archie@whistle.com>
 */

package java.net;

import java.io.IOException;
import java.security.CodeSource;
import java.security.PermissionCollection;
import java.security.SecureClassLoader;
import java.util.Enumeration;
import java.util.Vector;
import java.util.jar.Manifest;

// XXX this class is not complete!

public class URLClassLoader extends SecureClassLoader {
	final Vector urls;
	final ClassLoader parent;

	public URLClassLoader(URL[] urls, ClassLoader parent) {
		this(urls, parent, null);
	}

	public URLClassLoader(URL[] urls) {
		this(urls, URLClassLoader.class.getClassLoader(), null);
	}

	public URLClassLoader(URL[] urls, ClassLoader parent,
			URLStreamHandlerFactory factory) {
		this.urls = new Vector();
		for (int i = 0; i < urls.length; i++) {
			this.urls.addElement(urls[i]);
		}
		this.parent = parent;
	}

	protected void addURL(URL url) {
		urls.addElement(url);
	}

	public URL[] getURLs() {
		return (URL[])urls.toArray(new URL[urls.size()]);
	}

	//protected Class findClass(String name) throws ClassNotFoundException {
	//}

	protected Package definePackage(String name, Manifest man, URL url)
			throws IllegalArgumentException {
		return super.definePackage(name, "", "", "", "", "", "", url);
	}

	//public URL findResource(String name) {
	//}

	//public Enumeration findResources(String name) throws IOException {
	//}

	protected PermissionCollection getPermissions(CodeSource codesource) {
		return null;
	}

	public static URLClassLoader newInstance(URL[] urls,
			ClassLoader parent) {
		return new URLClassLoader(urls, parent);
	}

	public static URLClassLoader newInstance(URL[] urls) {
		return new URLClassLoader(urls);
	}
}

