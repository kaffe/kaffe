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

/* all classes for which this classloader was the initiating classloader
 * This table is a mapping from name -> loadTableEntry
 */
private Hashtable loaded = new Hashtable();
private static class LoadTableEntry {
	String name;
	Class clazz;
	LoadTableEntry(String name) {
	    this.name = name;
	}
};

private ClassLoader parent;

protected ClassLoader() {
	this(getBaseClassLoader());
}

protected ClassLoader(ClassLoader parent) {
	System.getSecurityManager().checkCreateClassLoader();
	this.parent = parent;
}

final protected Class defineClass(String name, byte data[], int offset, int length) {
	return (defineClass0(name, data, offset, length));
}

/**
 * @deprecated
 */
final protected Class defineClass(byte data[], int offset, int length) {
	return (defineClass(null, data, offset, length));
}

final protected Class findLoadedClass(String name) {
	LoadTableEntry entry = (LoadTableEntry)loaded.get(name);
	if (entry == null)
		return (null);
	else
		return (entry.clazz);
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
	if (data == null) {
		return (null);
	}
	else {
		return (new ByteArrayInputStream(data));
	}
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

/**
 * The VM will always call loadClassVM.  Here's we check whether we asked
 * the classloader already.  We won't ever ask a class loader twice for
 * the same class.
 * We synchronize this method to avoid confusing classloaders.
 */
private synchronized Class loadClassVM(String name, boolean resolve) throws ClassNotFoundException {
        LoadTableEntry entry = (LoadTableEntry)loaded.get(name);

	if (entry == null) {
		LoadTableEntry newEntry = new LoadTableEntry(name);
		/* record that we asked first */
		loaded.put(name, newEntry);
		newEntry.clazz = loadClass(name, resolve);
		return (newEntry.clazz);
        } else {
		/* whatever it said the first time will always be its answer */
		return (entry.clazz);
	}
}

protected void checkPackageAccess(String name) throws SecurityException {
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

public ClassLoader getParent() {
	return (parent);
}

public final Enumeration getResources(String name) throws IOException {
	throw new kaffe.util.NotImplemented();
}


public Enumeration getLocalResources(String name) throws IOException {
	return (null);
}

public URL getLocalResource(String name) {
	return (null);
}

public Enumeration getSystemResources(String name) throws IOException {
	throw new kaffe.util.NotImplemented();
}

public static ClassLoader getBaseClassLoader() {
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
native private void resolveClass0(Class cls);
/**
 *  This is not part of the public interface.
 */
native public static byte[] getSystemResourceAsBytes0(String name);

}
