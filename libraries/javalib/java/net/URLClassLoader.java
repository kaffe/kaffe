
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

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.security.CodeSource;
import java.security.PermissionCollection;
import java.security.SecureClassLoader;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.NoSuchElementException;
import java.util.Vector;
import java.util.jar.Attributes;
import java.util.jar.Manifest;

public class URLClassLoader extends SecureClassLoader {
	private final Vector urls;
	private final URLStreamHandlerFactory factory;
	private final HashMap handlers = new HashMap();
	private final HashMap jarFiles = new HashMap();

	public URLClassLoader(URL[] urls, ClassLoader parent) {
		this(urls, parent, null);
	}

	public URLClassLoader(URL[] urls) {
		this(urls, URLClassLoader.class.getClassLoader(), null);
	}

	public URLClassLoader(URL[] urls, ClassLoader parent,
			URLStreamHandlerFactory factory) {
		super(parent);
		this.urls = new Vector();
		this.factory = factory;
		for (int i = 0; i < urls.length; i++) {
			addURL(urls[i]);
		}
	}

	protected void addURL(URL url) {
		urls.addElement(url);
	}

	public URL[] getURLs() {
		return (URL[])urls.toArray(new URL[urls.size()]);
	}

	protected Class findClass(String name) throws ClassNotFoundException {
		URL url = findResource(name.replace('.', '/') + ".class");
		if (url == null) {
			throw new ClassNotFoundException(name);
		}
		try {
			InputStream in = url.openStream();
			ByteArrayOutputStream out = new ByteArrayOutputStream();
			byte[] buf = new byte[1024];
			for (int r; (r = in.read(buf)) != -1; ) {
				out.write(buf, 0, r);
			}
			in.close();
			buf = out.toByteArray();
			return defineClass(name, buf, 0, buf.length,
			    (CodeSource)null);	// XXX specify codesource
		} catch (IOException e) {
			throw new ClassNotFoundException(name + ": " + e);
		}
	}

	private URLStreamHandler getHandler(URL url) {
		if (factory == null) {
			return null;
		}
		String prot = url.getProtocol();
		URLStreamHandler handler = (URLStreamHandler)handlers.get(prot);
		if (handler == null) {
			handler = factory.createURLStreamHandler(prot);
			handlers.put(prot, handler);
		}
		return handler;
	}

	// XXX incomplete
	protected Package definePackage(String name, Manifest man, URL url)
			throws IllegalArgumentException {
		return super.definePackage(name,
		    man.getAttributes(name).getValue(
		      Attributes.Name.SPECIFICATION_TITLE),
		    man.getAttributes(name).getValue(
		      Attributes.Name.SPECIFICATION_VERSION),
		    man.getAttributes(name).getValue(
		      Attributes.Name.SPECIFICATION_VENDOR),
		    man.getAttributes(name).getValue(
		      Attributes.Name.IMPLEMENTATION_TITLE),
		    man.getAttributes(name).getValue(
		      Attributes.Name.IMPLEMENTATION_VERSION),
		    man.getAttributes(name).getValue(
		      Attributes.Name.IMPLEMENTATION_VENDOR),
		    null);
	}

	public URL findResource(String name) {
		try {
			return (URL)findResources(name).nextElement();
		} catch (IOException e) {
		} catch (NoSuchElementException e) {
		}
		return null;
	}

	public Enumeration findResources(String name) throws IOException {
		Vector v = new Vector();
		for (int i = 0; i < urls.size(); i++) {
			URL url = (URL)urls.elementAt(i);
			try {
				// Get URL to putative resource
				if (url.getFile().endsWith("/")) {	// dir
					url = new URL(url, name,
					    getHandler(url));
				} else {				// jar
					File fn = getJar(url);
					if (fn == null) {
						continue;
					}
					url = new URL("jar:file:"
					    + fn + "!/" + name);
				}

				// Try to get it, to see if it's really there
				URLConnection u = url.openConnection();

				// OK it's there, add it
				v.addElement(url);

				// Try not to leave a lingering connection
				try {
					u.getClass().getMethod("disconnect",
					    null).invoke(u, new Object[0]);
				} catch (Exception e) {
				}
			} catch (IOException e) {	// resource not found
			}
		}
		return v.elements();
	}

	// Find a JAR file, keeping a cache of already downloaded ones
	private File getJar(URL url) {

		// Check the cache
		File file = (File)jarFiles.get(url);
		if (file != null) {
			return file;
		}

		// Download JAR file
		try {
			file = File.createTempFile("jar", null);
			file.deleteOnExit();
			InputStream in = url.openStream();
			FileOutputStream out = new FileOutputStream(file);
			byte[] buf = new byte[1024];
			for (int r; (r = in.read(buf)) != -1; )
				out.write(buf, 0, r);
			in.close();
			out.close();

			// Cache it
			jarFiles.put(url, file);
			return file;
		} catch (IOException e) {
			return null;
		}
	}

	// XXX incomplete
	protected PermissionCollection getPermissions(CodeSource codesource) {
		return super.getPermissions(codesource);
	}

	public static URLClassLoader newInstance(URL[] urls,
			ClassLoader parent) {
		return new URLClassLoader(urls, parent);
	}

	public static URLClassLoader newInstance(URL[] urls) {
		return new URLClassLoader(urls);
	}
}

