
/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.util;

import java.io.IOException;
import java.io.InputStream;
import java.lang.Class;
import java.lang.String;
import java.lang.System;

abstract public class ResourceBundle {

protected ResourceBundle parent;

public ResourceBundle() {
	parent = null;
}

final public static ResourceBundle getBundle(String baseName) throws MissingResourceException {
	return (getBundle(baseName, Locale.getDefault()));
}

final public static ResourceBundle getBundle(String baseName, Locale locale) throws MissingResourceException {

	String lang = locale.getLanguage();
	String cntry = locale.getCountry();
	String var = locale.getVariant();
	if (lang != null) {
		lang = lang.toLowerCase();
	}

	ResourceBundle bundle = null;

	try {
		ResourceBundle nbundle = getSpecificBundle(baseName);
		nbundle.setParent(bundle);
		bundle = nbundle;
	}
	catch (MissingResourceException _) {
	}

	if (lang != null) {
		try {
			ResourceBundle nbundle = getSpecificBundle(baseName + "_" + lang);
			nbundle.setParent(bundle);
			bundle = nbundle;
		}
		catch (MissingResourceException _) {
		}
	}

	if (lang != null && cntry != null) {
		try {
			ResourceBundle nbundle = getSpecificBundle(baseName + "_" + lang + "_" + cntry);
			nbundle.setParent(bundle);
			bundle = nbundle;
		}
		catch (MissingResourceException _) {
		}
	}

	if (lang != null && cntry != null && var != null) {
		try {
			ResourceBundle nbundle = getSpecificBundle(baseName + "_" + lang + "_" + cntry + "_" + var);
			nbundle.setParent(bundle);
			bundle = nbundle;
		}
		catch (MissingResourceException _) {
		}
	}

	if (bundle == null) {
		throw new MissingResourceException("no bundles found: " + baseName, "ResourceBundle", baseName);
	}
	return (bundle);
}

abstract public Enumeration getKeys();

final public Object getObject(String key) throws MissingResourceException {
	try {
		Object obj = handleGetObject(key);
		if (obj != null) {
			return (obj);
		}
	}
	catch (MissingResourceException e) {
	}
	if (parent == null) {
		throw new MissingResourceException("resource not found", this.getClass().toString(), key);
	}
	return (parent.getObject(key));
}

final private static ResourceBundle getSpecificBundle(String baseName) throws MissingResourceException {
	baseName = baseName.replace('.', '/');
	try {
		Class cls = Class.forName(baseName);
		/* 
		 * Only call newInstance if the cast to resource bundle 
		 * will indeed succeed.
		 */
		if (ResourceBundle.class.isAssignableFrom(cls)) {
			return ((ResourceBundle)cls.newInstance());
		}
	}
	catch (Exception _) {
	}

	// Okay, failed to load bundle - so attempt to load properties as bundle.
	ClassLoader sysclassloader = System.class.getClassLoader();
	InputStream strm = null;

	if (sysclassloader != null) {
		strm = sysclassloader.getResourceAsStream(baseName + ".properties");
	}
	else {
		strm = ClassLoader.getSystemResourceAsStream(baseName + ".properties");
	}
	if (strm != null) {
		try {
			return (new PropertyResourceBundle(strm));
		}
		catch (IOException _) {
		}
	}
	throw new MissingResourceException("bundle not found", "ResourceBundle", baseName);
}

final public String getString(String key) throws MissingResourceException {
	return ((String)getObject(key));
}

final public String[] getStringArray(String key) throws MissingResourceException {
	return ((String[])getObject(key));
}

abstract protected Object handleGetObject(String key) throws MissingResourceException;

protected void setParent(ResourceBundle par) {
	parent = par;
}

}
