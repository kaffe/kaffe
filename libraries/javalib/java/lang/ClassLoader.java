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

import java.io.InputStream;
import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.Enumeration;
import java.util.Hashtable;

public abstract class ClassLoader {

/**
 * To prevent any classes from being gc'd before we are gc'd, we keep them
 * in this hashtable.  The contents of this table correspond to entries
 * in the VM-internal class entry pool.
 */
private Hashtable loadedClasses = new Hashtable();
private ClassLoader parent;

protected ClassLoader() {
	this(getBaseClassLoader());
}

protected ClassLoader(ClassLoader parent) {
	System.getSecurityManager().checkCreateClassLoader();
	this.parent = parent;
}

final protected Class defineClass(String name, byte data[], int offset, int length) {
	Class clazz =defineClass0(name, data, offset, length);
	if (name != null) {
		loadedClasses.put(name, clazz);
	} else {
		loadedClasses.put(clazz.getName(), clazz);
	}
	return (clazz);
}

/**
 * @deprecated
 */
final protected Class defineClass(byte data[], int offset, int length) {
	return (defineClass(null, data, offset, length));
}

final protected Class findLoadedClass(String name) {
	return (findLoadedClass0(name));
}

final protected Class findSystemClass(String name) throws ClassNotFoundException {
	return (findSystemClass0(name));
}

public URL getResource(String name) {
	return (null);	// Default implementation just returns null
}

public InputStream getResourceAsStream(String name) {
	return (null); // Default implementation just returns null
}

final public static URL getSystemResource(String name) {
	try {
		return (new URL("system", "", 0, name));
	}
	catch (MalformedURLException _) {
		return (null);
	}
}

public static final InputStream getSystemResourceAsStream(String name) {
	byte[] data = getSystemResourceAsBytes0(name);
	return (data != null) ? new ByteArrayInputStream(data) : null;
}

public Class loadClass(String name) throws ClassNotFoundException {
	return (loadClass(name, true));
}

protected Class loadClass(String name, boolean resolve) throws ClassNotFoundException {
	Class cls = findLoadedClass(name);
	if (cls == null) {
		try {
			if (parent != null) {
				cls = parent.loadClass(name, resolve);
			}
			else {
				cls = findSystemClass(name);
			}
		}
		catch (ClassNotFoundException _) {
			cls = findLocalClass(name);
		}
	}
	if (resolve) {
		resolveClass(cls);
	}
	return (cls);
}

void checkPackageAccess(String name) throws SecurityException {
}

protected Class findLocalClass(String name) throws ClassNotFoundException {
	throw new ClassNotFoundException(name);
}

final protected void resolveClass(Class c) {
	resolveClass0(c);
}

final protected void setSigners(Class cl, Object signers[]) {
	// Signer's are not currently supported.
}

public final ClassLoader getParent() {
	return (parent);
}

public final Enumeration getResources(String name) throws IOException {
	throw new kaffe.util.NotImplemented();
}


Enumeration getLocalResources(String name) throws IOException {
	return (null);
}

URL getLocalResource(String name) {
	return (null);
}

public static Enumeration getSystemResources(String name) throws IOException {
	throw new kaffe.util.NotImplemented();
}

static ClassLoader getBaseClassLoader() {
	return (null);
}

protected Package definePackage(String name, String specTitle, String specVersion, String specVendor, String implTitle, String implVersion, String implVendor, URL sealBase) throws IllegalArgumentException {
	throw new kaffe.util.NotImplemented();
}

protected Package getPackage(String name) {
	throw new kaffe.util.NotImplemented();
}

protected Package[] getPackages() {
	throw new kaffe.util.NotImplemented();
}

native private Class defineClass0(String name, byte data[], int offset, int length);
native private Class findSystemClass0(String name);
native private Class findLoadedClass0(String name);
native private void resolveClass0(Class cls);

native static byte[] getSystemResourceAsBytes0(String name);

final native private void finalize0();

private Object finalizeHelper = new Object() {
    protected void finalize() throws Throwable {
        finalize0();
    }
};
}
