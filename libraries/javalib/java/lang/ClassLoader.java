/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 * Portions Copyright (C) 1998, 1999, 2001, 2002, 2003 Free Software Foundation, Inc.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.lang;

import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.security.ProtectionDomain;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Hashtable;
import java.util.Map;
import java.util.NoSuchElementException;
import java.util.Set;
import java.util.Vector;

import kaffe.lang.AppClassLoader;
import kaffe.lang.PrimordialClassLoader;

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

/**
 * We keep a reference to all packages loaded by this loader.
 */
private final Hashtable loadedPackages = new Hashtable();

/**
 * We keep the references to loaded classes and their protection domain
 * in this Map.
 */
private final Hashtable protectionDomains = new Hashtable();

/**
 * The command-line state of the default assertion status overrides.
 */
// Package visible for use by Class.
// XXX should be set from command line
static final boolean systemDefaultAssertionStatus = true;

/**
 * The desired assertion status of classes loaded by this loader, if not
 * overridden by package or class instructions.
 */
// Package visible for use by Class.
boolean defaultAssertionStatus = systemDefaultAssertionStatus;

/**
 * The command-line state of the package assertion status overrides. This
 * map is never modified, so it does not need to be synchronized.
 */
// Package visible for use by Class.
// XXX should be set from command line
static final Map systemPackageAssertionStatus = new HashMap();

/**
 * The map of package assertion status overrides, or null if no package
 * overrides have been specified yet. The values of the map should be
 * Boolean.TRUE or Boolean.FALSE, and the unnamed package is represented
 * by the null key. This map must be synchronized on this instance.
 */
// Package visible for use by Class.
Map packageAssertionStatus;

/**
 * The command-line state of the class assertion status overrides. This
 * map is never modified, so it does not need to be synchronized.
 */
// Package visible for use by Class.
// XXX should be set from command line
static final Map systemClassAssertionStatus = new HashMap();

/**
 * The map of class assertion status overrides, or null if no class
 * overrides have been specified yet. The values of the map should be
 * Boolean.TRUE or Boolean.FALSE. This map must be synchronized on this
 * instance.
 */
// Package visible for use by Class.
Map classAssertionStatus;

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
	Class c = findLoadedClass (name);

	// Second, try the parent class loader
	if (c == null) {
		try {
			if (parent != null) {
				c = parent.loadClass(name, resolve);
			} else {
				c = PrimordialClassLoader.getSingleton().
					loadClass(name, resolve);
			}
		} catch (ClassNotFoundException _) {
		}
	}

	// Third, try findClass()
	if (c==null && (c = findClass(name)) == null) {
		throw new ClassNotFoundException(name);
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
	return defineClass(name, data, off, len, null);
}

protected final Class defineClass(String name, byte data[], int off,
		int len, ProtectionDomain pd) throws ClassFormatError {
	if (off < 0 || len < 0 || off + len > data.length) {
		throw new IndexOutOfBoundsException();
	}
	Class clazz = null;
	clazz = defineClass0(name, data, off, len);
	if (name != null) {
		loadedClasses.put(name, clazz);
	}
	else {
		loadedClasses.put(clazz.getName(), clazz);
	}
	if (pd == null) {
		if (defaultProtectionDomain == null) {
			// XXX FIXME..
			defaultProtectionDomain = new ProtectionDomain(null, null);
		}
		pd = defaultProtectionDomain;
	}
	protectionDomains.put(clazz, pd);
	return (clazz);
}

protected final void resolveClass(Class c) {
	resolveClass0(c);
}

protected final Class findSystemClass(String name)
		throws ClassNotFoundException {
	return getSystemClassLoader().loadClass(name);
}

public final ClassLoader getParent() {
	return parent;
}

protected final void setSigners(Class cl, Object signers[]) {
	throw new kaffe.util.NotImplemented(getClass().getName()
		+ ".setSigners()");
}

protected final Class findLoadedClass(String name) {
	return (Class) loadedClasses.get(name); 
}

public URL getResource(String name) {
	URL ret = null;

	if (parent != null) {
		ret = parent.getResource (name);
	} else {
		// we have to call findResource directly, since getResource()
		// would cause an endless loop 
		ret = PrimordialClassLoader.getSingleton().findResource (name);
	}

	if (ret == null) {
		ret = findResource (name);
	}
	
	return ret;
}

public final Enumeration getResources(String name) throws IOException {
	Vector v = new Vector();
	
	if (parent != null) {
		for (Enumeration e = parent.getResources(name); e.hasMoreElements();) {
			v.addElement(e.nextElement());
		}
	} else {
		// we have to call findResources directly, since getResources() would cause an endless loop
		for (Enumeration e=PrimordialClassLoader.getSingleton().findResources(name); e.hasMoreElements();) {
			v.addElement (e.nextElement ());
		}
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
	return AppClassLoader.getSingleton();
}

protected Package definePackage(String name, String specTitle,
				String specVersion, String specVendor,
				String implTitle, String implVersion,
				String implVendor, URL sealBase)
	throws IllegalArgumentException {

	Package pack = getPackage(name);

	if (pack != null) {
		throw new IllegalArgumentException("Package " + name + " already defined");
	}

	pack = new Package(name, specTitle, specVersion, specVendor,
			   implTitle, implVersion, implVendor, sealBase);
	loadedPackages.put(name, pack);
	return pack;
}

protected Package getPackage(String name) {
	Package ret = null;

	if (parent!=null) {
		ret = parent.getPackage (name);
	} else {
		ret = PrimordialClassLoader.getSingleton().getPackage(name);
	}	

	if (ret == null) {
		ret = (Package) loadedPackages.get (name);
	}

	return ret;
}

protected Package[] getPackages() {
	Package[] myPackages;

	myPackages = new Package[loadedPackages.size()];
	loadedPackages.values().toArray (myPackages);

	Package[] ancestorPackages = null;

	if (parent != null) {
		ancestorPackages = parent.getPackages();
	} else {
		ancestorPackages = PrimordialClassLoader.getSingleton().getPackages();
	} 
	
	Package[] ret = new Package[myPackages.length + ancestorPackages.length];
	
	System.arraycopy (ret, 0, ancestorPackages, 0, ancestorPackages.length);
	System.arraycopy (ret, ancestorPackages.length, myPackages, 0, myPackages.length);
		
	return ret;
}

protected String findLibrary(String libname) {
	return null;
}

synchronized void addNativeLibrary(NativeLibrary lib) {
	loadedLibraries.add(lib);
}

ProtectionDomain getProtectionDomain(Class clazz) {
	return (ProtectionDomain) protectionDomains.get(clazz);
}

private native Class defineClass0(String name, byte data[], int off, int len);
private native void resolveClass0(Class cls);

/**
 * Set the default assertion status for classes loaded by this classloader,
 * used unless overridden by a package or class request.
 *
 * @param enabled true to set the default to enabled
 * @see #setClassAssertionStatus(String, boolean)
 * @see #setPackageAssertionStatus(String, boolean)
 * @see #clearAssertionStatus()
 * @since 1.4
 */
public void setDefaultAssertionStatus(boolean enabled)
{
	defaultAssertionStatus = enabled;
}
  
/**
 * Set the default assertion status for packages, used unless overridden
 * by a class request. This default also covers subpackages, unless they
 * are also specified. The unnamed package should use null for the name.
 *
 * @param name the package (and subpackages) to affect
 * @param enabled true to set the default to enabled
 * @see #setDefaultAssertionStatus(String, boolean)
 * @see #setClassAssertionStatus(String, boolean)
 * @see #clearAssertionStatus()
 * @since 1.4
 */
public synchronized void setPackageAssertionStatus(String name,
                                                   boolean enabled)
{
	if (packageAssertionStatus == null)
		packageAssertionStatus
		  = new HashMap(systemPackageAssertionStatus);
	packageAssertionStatus.put(name, enabled ? Boolean.TRUE : Boolean.FALSE);
}
  
/**
 * Set the default assertion status for a class. This only affects the
 * status of top-level classes, any other string is harmless.
 *
 * @param name the class to affect
 * @param enabled true to set the default to enabled
 * @throws NullPointerException if name is null
 * @see #setDefaultAssertionStatus(String, boolean)
 * @see #setPackageAssertionStatus(String, boolean)
 * @see #clearAssertionStatus()
 * @since 1.4
 */
public synchronized void setClassAssertionStatus(String name,
                                                 boolean enabled)
{
	if (classAssertionStatus == null)
		classAssertionStatus = new HashMap(systemClassAssertionStatus);
	// The toString() hack catches null, as required.
	classAssertionStatus.put(name.toString(), enabled ? Boolean.TRUE : Boolean.FALSE);
}
  
/**
 * Resets the default assertion status of this classloader, its packages
 * and classes, all to false. This allows overriding defaults inherited
 * from the command line.
 *
 * @see #setDefaultAssertionStatus(boolean)
 * @see #setClassAssertionStatus(String, boolean)
 * @see #setPackageAssertionStatus(String, boolean)
 * @since 1.4
 */
public synchronized void clearAssertionStatus()
{
	defaultAssertionStatus = false;
	packageAssertionStatus = new HashMap();
	classAssertionStatus = new HashMap();
}
}
