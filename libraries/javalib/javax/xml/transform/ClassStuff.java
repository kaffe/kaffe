/*
 * $Id: ClassStuff.java,v 1.2 2003/12/02 21:38:02 dalibor Exp $
 * Copyright (C) 2001 David Brownell
 * 
 * This file is part of GNU JAXP, a library.
 *
 * GNU JAXP is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * GNU JAXP is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * As a special exception, if you link this library with other files to
 * produce an executable, this library does not by itself cause the
 * resulting executable to be covered by the GNU General Public License.
 * This exception does not however invalidate any other reasons why the
 * executable file might be covered by the GNU General Public License. 
 */

package javax.xml.transform;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.lang.reflect.Method;
import java.util.Properties;


// $Id: ClassStuff.java,v 1.2 2003/12/02 21:38:02 dalibor Exp $

/**
 * Package-private utility methods for sharing
 * magic related to class loading.
 * NOTE:  This is cloned in javax.xml.parsers,
 * where a different exception is thrown (bleech).
 * Keep changes to the two copies in sync.
 *
 * @author David Brownell
 * @version	$Id: ClassStuff.java,v 1.2 2003/12/02 21:38:02 dalibor Exp $
 */
final class ClassStuff
{
    private ClassStuff () { }

    /**
     * Get the default factory using the four-stage defaulting
     * mechanism defined by JAXP.
     */
    static Object
    createFactory (String label, String defaultClass)
    throws TransformerFactoryConfigurationError
    {
	String		name = null;
	ClassLoader	loader = null;

	// figure out which class loader to use.
	// source compiles on jdk 1.1, but works with jdk 1.2+ security
        try {
	    Method 	m = null;

	    // Can we use JDK 1.2 APIs?  Preferred: security policies apply.
            m = Thread.class.getMethod ("getContextClassLoader", null);
            loader = (ClassLoader) m.invoke (Thread.currentThread(), null);
        } catch (NoSuchMethodException e) {
            // Assume that we are running JDK 1.1; use current ClassLoader
            loader = ClassStuff.class.getClassLoader();
	} catch (Exception e) {
	    // "should not happen"
            throw new UnknownError (e.getMessage());
        }

	// 1. Check System Property
	// ... normally fails in applet environments
	try { name = System.getProperty (label);
	} catch (SecurityException e) { /* IGNORE */ }

	// 2. Check in $JAVA_HOME/lib/jaxp.properties
	try {
	    if (name == null) {
		String	javaHome;
		File	file;

		javaHome = System.getProperty ("java.home");
		file = new File (new File (javaHome, "lib"), "jaxp.properties");
		if (file.exists() == true) {
		    FileInputStream	in = new FileInputStream (file);
		    Properties		props = new Properties();

		    props.load (in);
		    name  = props.getProperty (label);
		    in.close ();
		}
	    }
	} catch (Exception e) { /* IGNORE */ }

	// 3. Check Services API
	if (name == null) {
	    try {
		String		service = "META-INF/services/" + label;
		InputStream	in;
		BufferedReader	reader;

		if (loader == null)
		    in = ClassLoader.getSystemResourceAsStream (service);
		else
		    in = loader.getResourceAsStream (service);
		if (in != null) {
		    reader = new BufferedReader (
			new InputStreamReader (in, "UTF8"));
		    name = reader.readLine();
		    in.close ();
		}
	    } catch (Exception e2) { /* IGNORE */ }
	}

	// 4. Distro-specific fallback
	if (name == null)
	    name = defaultClass;
	
	// Instantiate!
	try {
	    Class	klass;

	    if (loader == null)
		klass = Class.forName (name);
	    else
		klass = loader.loadClass (name);
	    return klass.newInstance ();

	} catch (ClassNotFoundException e) {
	    throw new TransformerFactoryConfigurationError (e,
		"Factory class " + name
		    + " not found");
	} catch (IllegalAccessException e) {
	    throw new TransformerFactoryConfigurationError (e,
		"Factory class " + name
		    + " found but cannot be loaded");
	} catch (InstantiationException e) {
	    throw new TransformerFactoryConfigurationError (e,
		"Factory class " + name
		    + " loaded but cannot be instantiated"
		    + " ((no default constructor?)");
	}
    }
}
