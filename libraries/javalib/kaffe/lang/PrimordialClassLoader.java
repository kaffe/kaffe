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

import java.io.File;
import java.io.IOException;
import java.net.MalformedURLException;
import java.net.URL;
import java.security.AllPermission;
import java.security.CodeSource;
import java.security.Permissions;
import java.security.ProtectionDomain;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Map;
import java.util.StringTokenizer;
import java.util.Vector;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;

/**
 * Java-level access to the primordial class loader.
 */
public final class PrimordialClassLoader extends ClassLoader {

private static final PrimordialClassLoader SINGLETON =
	new PrimordialClassLoader();

private static final Package[] NO_PACKAGES = new Package[0];
private static final ProtectionDomain DEFAULT_PROTECTION_DOMAIN;

private static final Map bootjars = new Hashtable();

static {
	CodeSource source = new CodeSource(null, null);
	Permissions permissions = new Permissions();
	permissions.add(new AllPermission());
	permissions.setReadOnly();
	DEFAULT_PROTECTION_DOMAIN = new ProtectionDomain(source, permissions);
}

private PrimordialClassLoader() {
	super(null);
}

public Class loadClass(String name, boolean resolve)
	throws ClassNotFoundException
{
	Class retval;

	if( name.startsWith("kaffe.lang.") ||
            name.startsWith("gnu.classpath.") )
	{
		throw new ClassNotFoundException(name);
	}

	if( (retval = this.findLoadedClass(name)) == null )
	{
		retval = this.findClass(name);
	}

	if( resolve )
	{
		this.resolveClass(retval);
	}
	return retval;
}

// returns the component type name of an array type.
// Throws ClassNotFoundException if the component type
// is not a primitive type and is not enclosed by 'L' and ';'.
private static String componentType(String name) throws ClassNotFoundException {
	// find the start of the component type
	int componentStart = name.lastIndexOf('[') + 1;

	// if component type is an object type,
	// return the object type without 'L' and ';'
	if (name.charAt(componentStart) == 'L') {
	    if (name.endsWith(";")) {
		return name.substring(componentStart + 1, name.length() - 1);
	    }
	    else {
		throw new ClassNotFoundException(name);
	    }
	}
	// handle case of a class name ending with ';'
	// but not starting with 'L' to denote the object type
	else if (name.endsWith(";")) {
	    throw new ClassNotFoundException(name);
	}
	// if component type is a primitive type return primitive type.
	// if the length of the primitive type name is > 1,
	// then it's a bad primitive type:
	// just return the primitive type name
	// for the error message
	else if (name.length() - componentStart > 1) {
		return name.substring(componentStart);
	}
	// if the length is 1 then return [primitive type,
	// as just returning the primitive type name will fail.
	else {
		return name.substring(componentStart - 1);
	}
}

public static PrimordialClassLoader getSingleton() {
	return SINGLETON;
}

/*
 * Search through the CLASSPATH directories and ZIP files to find
 * the named resource (which may appear more than once). Make sure
 * it really exists in each place before adding it.
 */
protected void findResources(Vector v, String name) throws IOException {
	// search the bootclasspath first
	String fileSep = System.getProperties().getProperty("file.separator");
	String pathSep = System.getProperties().getProperty("path.separator");
	String classpath = System.getProperties().getProperty("sun.boot.class.path");
	StringTokenizer t = new StringTokenizer(classpath, pathSep);

	if (name.startsWith("/")) {
	    name = name.substring(1);
	}

	while (t.hasMoreTokens()) {
		File file = new File(t.nextToken());

		if (!file.exists()) {
			continue;
		}
		if (file.isDirectory()) {
			file = new File(file, name);
			if (file.isFile()) {
				try {
				    v.addElement(new URL("file", "", file.getCanonicalPath().replace(File.separatorChar, '/')));
				} catch (MalformedURLException e) {
				}
			}
			continue;
		}
		if (file.isFile()) {
			ZipFile zip = (ZipFile) bootjars.get(file.getName());
			try {
			  if (zip == null) {
			    zip = new ZipFile(file);
			    bootjars.put(file.getName(), zip);
			  }
			  ZipEntry entry = zip.getEntry(name);
			  if (entry != null && !entry.isDirectory()) {
			    URL ju = new URL("jar:file:"
					     + file.getCanonicalPath().replace(File.separatorChar, '/') + "!/" + entry.getName());
			    v.addElement(ju);
			  }
			} catch (IOException e) {
			}
		}
	}
}

public URL findResource (String name) {
	try {
		Vector v = new Vector ();

		findResources (v, name);

		if (v.size()>0) {
			return (URL)v.elementAt (0);
		}
	} catch (IOException _) {
	}

	return null;
}

public Enumeration findResources(String name) throws IOException {
	Vector retval = new Vector();

	this.findResources(retval, name);

	return retval.elements();
}

public Package getPackage (String name) {
	return null;
}

public Package[] getPackages () {
	return NO_PACKAGES;
}

public ProtectionDomain getProtectionDomain (Class clazz) {
	return DEFAULT_PROTECTION_DOMAIN;
}

protected Class findClass(String name) throws ClassNotFoundException {
	return findClass0(name);
}

private native Class findClass0(String name) throws ClassNotFoundException;

}
