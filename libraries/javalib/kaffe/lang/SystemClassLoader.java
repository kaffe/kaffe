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
import java.util.Enumeration;
import java.util.StringTokenizer;
import java.util.Vector;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;

public class SystemClassLoader extends ClassLoader {

private static final ClassLoader singleton = new SystemClassLoader();

private SystemClassLoader() {
	super(null);		// this line is not really necessary!
}

public static ClassLoader getClassLoader() {
	return (singleton);
}

/*
 * Search through the CLASSPATH directories and ZIP files to find
 * the named resource (which may appear more than once). Make sure
 * it really exists in each place before adding it.
 */
public Enumeration findResources(String name) throws IOException {
	String fileSep = System.getProperties().getProperty("file.separator");
	String pathSep = System.getProperties().getProperty("path.separator");
	String classpath = System.getProperties().getProperty("java.class.path");
	StringTokenizer t = new StringTokenizer(classpath, pathSep);
	Vector v = new Vector();

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
			ZipFile zip = null;
			try {
				zip = new ZipFile(file);
				ZipEntry entry = zip.getEntry(name);
				if (entry != null && !entry.isDirectory()) {
				    URL ju = new URL("jar:file:"
					+ file.getCanonicalPath().replace(File.separatorChar, '/') + "!/" + entry.getName());
				    v.addElement(ju);
				}
			} catch (IOException e) {
			} finally {
				if (zip != null) {
					try {
						zip.close();
					} catch (IOException e) {
					}
				}
			}
		}
	}
	return v.elements();
}

protected Class findClass(String name) throws ClassNotFoundException {
	return findClass0(name);
}

private native Class findClass0(String name) throws ClassNotFoundException;

}

