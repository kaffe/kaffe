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
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.MalformedURLException;
import java.net.URL;
import java.security.SecureClassLoader;
import java.security.CodeSource;
import java.security.cert.Certificate;
import java.util.Enumeration;
import java.util.StringTokenizer;
import java.util.Vector;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;
import java.util.jar.JarFile;
import java.util.jar.Attributes;

public class SystemClassLoader extends SecureClassLoader {

	/**
	 * One source of classes.
	 */
	private abstract class Source {
		Source next;
                CodeSource codeSource;

		abstract Class findClass (String name, String fileName);

		abstract void findResources (Vector v, String name);
	}

	/**
	 * A source which is a directory.
 	 */
	private final class DirSource extends Source {
		File dir;
                
		DirSource (File f) throws IOException {
			dir = f.getCanonicalFile ();
		}

		Class findClass (String name, String fileName) {
			File file = new File (dir, fileName);

			if (!file.exists ()) {
				return null;
			}

			try {
				int length = (int)file.length();
				FileInputStream in = new FileInputStream (file);
				byte[] buf = new byte[length];
				for (int j=0; j<length; j+=in.read (buf, j, length-j));

				if (codeSource == null) {
					try {
						codeSource = new CodeSource (dir.toURL(), new Certificate[0]);
					} catch (MalformedURLException _) {}
				}

				Class ret = defineClass (name, buf, 0, length, codeSource);

				String pkgName = PackageHelper.getPackageName(ret);
				if (getPackage(pkgName) == null) {
					definePackage(pkgName, null, null, null, null, null, null, null);
                                }

				return ret;
			} catch (Exception _) {}

			return null;
		}
		
		void findResources (Vector v, String name) {
			File file = new File (dir, name);

			if (file.exists ()) {
				try {
					v.add (file.toURL());
				} catch (MalformedURLException _) {}
			}
		}
	}

	/**
	 * A source which is a jar file.
	 */
	private final class JarSource extends Source {
		JarFile jar;
		String urlPrefix;
                File file;

		public JarSource (File file) throws IOException {
			File canonicalFile = file.getCanonicalFile ();
			this.jar = new JarFile (canonicalFile); 
                        this.urlPrefix = "jar:file:"+canonicalFile.getPath().replace(File.separatorChar, '/') + "!/";
			this.file = canonicalFile;
		}
		
		Class findClass (String name, String fileName) {
			ZipEntry classFile = jar.getEntry(fileName);

			if (classFile==null) {
				return null;
			}

               		int length = (int)classFile.getSize();
               		byte[] buf = new byte[length];
             		try {
				InputStream in = jar.getInputStream(classFile);
				for (int j=0;j<length;j+=in.read(buf, j, length-j));
					
				if (codeSource == null) {
					try {
						codeSource = new CodeSource (file.toURL(), new Certificate[0]);
					} catch (MalformedURLException _) {}
       				}

                                Class ret = defineClass (name, buf, 0, length, codeSource);

				String pkgName = PackageHelper.getPackageName(ret);
				if (getPackage(pkgName) == null) {
					Attributes attrs = jar.getManifest().getAttributes (pkgName);

					if (attrs == null) {
						attrs = jar.getManifest().getMainAttributes();
					}
                        		definePackage(pkgName,  
                                		attrs.getValue(Attributes.Name.SPECIFICATION_TITLE),
                                		attrs.getValue(Attributes.Name.SPECIFICATION_VERSION),
                                		attrs.getValue(Attributes.Name.SPECIFICATION_VENDOR),
                                		attrs.getValue(Attributes.Name.IMPLEMENTATION_TITLE),
                                		attrs.getValue(Attributes.Name.IMPLEMENTATION_VERSION),
                                		attrs.getValue(Attributes.Name.IMPLEMENTATION_VENDOR),
                                		null);
				}

				return ret;
			} catch (IOException _) {}
			return null;
		}

		void findResources (Vector v, String name) {
			ZipEntry resourceFile = jar.getEntry (name);

			if (resourceFile!=null && !resourceFile.isDirectory()) {
				try {
					v.addElement(new URL(urlPrefix + resourceFile.getName()));
				} catch (MalformedURLException _) {_.printStackTrace();}
			}
		}
	}

private static final ClassLoader singleton = new SystemClassLoader();

/* linked list of Sources as extracted from classpath */
private Source sources;

private SystemClassLoader() {
	super(null);

	StringTokenizer tok = new StringTokenizer (System.getProperty("java.class.path"), File.pathSeparator);

        Source tmpSources = null;

        while (tok.hasMoreTokens ())
        {
		File f = new File (tok.nextToken().trim());

                if (!f.exists()) {
			continue;
		}

		Source s = null;

		try {
                	if (f.isDirectory()) {
				s = new DirSource (f.getAbsoluteFile ());
			} else {
				s = new JarSource (f.getAbsoluteFile ());
			}

			if (this.sources==null) {
				this.sources = s;
				tmpSources = this.sources;
			} else {
				tmpSources.next = s; 
				tmpSources = tmpSources.next;
			}
		} catch (Exception _) { }
 	}    	   
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

public static ClassLoader getClassLoader() {
	return (singleton);
}

/*
 * Search through the CLASSPATH directories and ZIP files to find
 * the named resource (which may appear more than once). Make sure
 * it really exists in each place before adding it.
 */
public Enumeration findResources(String name) throws IOException {
	// search the bootclasspath first
	String fileSep = System.getProperties().getProperty("file.separator");
	String pathSep = System.getProperties().getProperty("path.separator");
	String classpath = System.getProperties().getProperty("sun.boot.class.path");
	StringTokenizer t = new StringTokenizer(classpath, pathSep);
	Vector v = new Vector();
	
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

	for (Source i=this.sources; i!=null; i=i.next) {
		i.findResources (v, name);
	}	

	return v.elements();
}


protected Class findClass(String name) throws ClassNotFoundException {
	// check bootclasspath first
	try {
		return findClass0 (name);
	} catch (ClassNotFoundException _) {
	}
	
	// then search the application classpath
	String fileName = name.replace ('.', '/') + ".class";

	Class ret = null;

	for (Source i=this.sources; i!=null && ret==null; i=i.next) {
		ret = i.findClass (name, fileName);
	}

	// throw an error, if nobody is able to find that class
        if (ret == null) {
		throw new ClassNotFoundException (name);
	} 

	return ret;
}

private native Class findClass0(String name) throws ClassNotFoundException;

}
