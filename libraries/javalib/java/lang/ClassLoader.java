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

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.MalformedURLException;
import java.net.URL;
import java.security.CodeSource;
import java.security.ProtectionDomain;
import java.util.Enumeration;
import java.util.HashSet;
import java.util.Hashtable;
import java.util.NoSuchElementException;
import java.util.Set;
import java.util.Vector;
import kaffe.lang.SystemClassLoader;

public abstract class ClassLoader {

/**
 * To prevent any classes from being gc'd before we are gc'd, we keep them
 * in this hashtable.  The contents of this table correspond to entries
 * in the VM-internal class entry pool.  This table only contains classes
 * for which we are the defining loader (ie, not classes for which we are
 * merely the initiating loader).
 */
private final Hashtable loadedClasses = new Hashtable();

/**
 * Similarly, we keep a reference to all native libraries loaded by this
 * loader.  When this class loader is GC'd, the native libraries become
 * candidates for finalization, which unlinks the shared library.
 */
private final Set loadedLibraries = new HashSet();

private ProtectionDomain defaultProtectionDomain;
private final ClassLoader parent;

protected ClassLoader() {
	this(getSystemClassLoader());
}

protected ClassLoader(ClassLoader parent) {
	SecurityManager sm = System.getSecurityManager();
	if (sm != null)
		sm.checkCreateClassLoader();
	this.parent = parent;
}

public Class loadClass(String name) throws ClassNotFoundException {
	return (loadClass(name, false));
}

protected Class loadClass(String name, boolean resolve)
		throws ClassNotFoundException {
	Class c;

	// Search for class...
	search: {
		// First, see if already loaded by this class
		if ((c = findLoadedClass(name)) != null) {
			break search;
		}

		// Second, try the parent class loader
		try {
			if (parent != null) {
				c = parent.loadClass(name, resolve);
				break search;
			} else if (this != getSystemClassLoader()) {
				c = getSystemClassLoader()
					.loadClass(name, resolve);
				break search;
			}
		} catch (ClassNotFoundException e) {
		}

		// Third, try findClass()
		if ((c = findClass(name)) == null) {
			throw new ClassNotFoundException(name);
		}
	}

	// Now, optionally resolve the class
	if (resolve) {
		resolveClass(c);
	}
	return (c);
}

protected Class findClass(String name) throws ClassNotFoundException {
	throw new ClassNotFoundException(name);
}

/**
 * @deprecated
 */
protected final Class defineClass(byte data[], int off, int len)
		throws ClassFormatError {
	return (defineClass(null, data, off, len));
}

protected final Class defineClass(String name, byte data[], int off, int len)
		throws ClassFormatError {
	if (defaultProtectionDomain == null) {
		// XXX FIXME..
		defaultProtectionDomain = new ProtectionDomain(null, null);
	}
	return defineClass(name, data, off, len, defaultProtectionDomain);
}

protected final Class defineClass(String name, byte data[], int off,
		int len, ProtectionDomain pd) throws ClassFormatError {
	if (off < 0 || len < 0 || off + len > data.length) {
		throw new IndexOutOfBoundsException();
	}
	Class clazz = defineClass0(name, data, off, len);
	if (name != null) {
		loadedClasses.put(name, clazz);
	}
	else {
		loadedClasses.put(clazz.getName(), clazz);
	}
	return (clazz);
}

protected final void resolveClass(Class c) {
	resolveClass0(c);
}

protected final Class findSystemClass(String name)
		throws ClassNotFoundException {
	return getSystemClassLoader().findClass(name);
}

public final ClassLoader getParent() {
	return parent;
}

protected final void setSigners(Class cl, Object signers[]) {
	throw new kaffe.util.NotImplemented(getClass().getName()
		+ ".setSigners()");
}

protected final Class findLoadedClass(String name) {
	return findLoadedClass0(name);
}

public URL getResource(String name) {
	try {
	    /*
	     * Note: if you're looking right here because kaffe crashed
	     * at this point, there's probably a problem with exception
	     * handling, because this is usually the place where the very
	     * first exception is thrown after kaffe starts up.
	     */
		return (URL)getResources(name).nextElement();
	} catch (IOException e) {
	} catch (NoSuchElementException e) {
	}
	return null;
}

public final Enumeration getResources(String name) throws IOException {
	Vector v = new Vector();
	ClassLoader p;

	if (parent != null) {
		p = parent;
	} else if (this != getSystemClassLoader()) {
		p = getSystemClassLoader();
	} else {
		p = null;
	}
	if (p != null) {
		for (Enumeration e = p.getResources(name);
		    e.hasMoreElements(); )
			v.addElement(e.nextElement());
	}
	for (Enumeration e = findResources(name); e.hasMoreElements(); )
		v.addElement(e.nextElement());
	return v.elements();
}

protected Enumeration findResources(String name) throws IOException {
	return new Vector().elements();		// ie, an empty Enumeration
}

protected URL findResource(String name) {
	try {
		return (URL)findResources(name).nextElement();
	} catch (IOException e) {
	} catch (NoSuchElementException e) {
	}
	return null;
}

public static URL getSystemResource(String name) {
	return getSystemClassLoader().getResource(name);
}

public static Enumeration getSystemResources(String name) throws IOException {
	return getSystemClassLoader().getResources(name);
}

public InputStream getResourceAsStream(String name) {
	URL url = getResource(name);

	if (url != null) {
		try {
			return url.openStream();
		} catch (IOException e) {
		}
	}
	return null;
}

public static InputStream getSystemResourceAsStream(String name) {
	return getSystemClassLoader().getResourceAsStream(name);
}

public static ClassLoader getSystemClassLoader() {
	return SystemClassLoader.getClassLoader();
}

protected Package definePackage(String name, String specTitle,
	String specVersion, String specVendor, String implTitle,
	String implVersion, String implVendor, URL sealBase)
		throws IllegalArgumentException {
	throw new kaffe.util.NotImplemented(getClass().getName()
		+ ".definePackage()");
}

protected Package getPackage(String name) {
	throw new kaffe.util.NotImplemented(getClass().getName()
		+ ".getPackage()");
}

protected Package[] getPackages() {
	throw new kaffe.util.NotImplemented(getClass().getName()
		+ ".getPackages()");
}

protected String findLibrary(String libname) {
	return null;
}

synchronized void addNativeLibrary(NativeLibrary lib) {
	loadedLibraries.add(lib);
}

private native Class defineClass0(String name, byte data[], int off, int len);
private native Class findLoadedClass0(String name);
private native void resolveClass0(Class cls);

}
