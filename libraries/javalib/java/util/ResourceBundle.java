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
import kaffe.lang.DummyClassLoader;

public abstract class ResourceBundle {

	protected ResourceBundle parent;
	static Hashtable cache = new Hashtable();

public ResourceBundle() {
	parent = null;
}

public static final ResourceBundle getBundle(String baseName)
		throws MissingResourceException {
	return getBundle(baseName, Locale.getDefault(), null);
}

public static final ResourceBundle getBundle(String baseName, Locale locale)
		throws MissingResourceException {
	return getBundle(baseName, locale, null);
}

public static ResourceBundle getBundle(String baseName, Locale locale,
		ClassLoader loader) throws MissingResourceException {
	ResourceBundle   bundle = null;
	String           key = baseName + locale;
	Object           val = cache.get( key);

	if ( val != null ) {
		bundle = (ResourceBundle) val;
	}
	else {
		bundle = getBundleWithLocale(baseName, locale, loader);

		/* It would appear that if we fail to load a resource bundle
		 * for a given locale, we just load the default one instead.
		 */
		if (bundle == null && locale != Locale.getDefault()) {
			bundle = getBundleWithLocale(baseName,
			    Locale.getDefault(), loader);
		}

		if ( bundle != null ) {
			cache.put( key, bundle);
		}
	}

	if (bundle == null) {
		throw new MissingResourceException("no bundles found: "
		    + baseName, "ResourceBundle", baseName);
	}

	return (bundle);
}

private static final ResourceBundle getBundleWithLocale(String baseName,
		Locale locale, ClassLoader loader) {

	String lang = locale.getLanguage();
	String cntry = locale.getCountry();
	String var1 = locale.getVariant();
	String var2 = null;

	if (var1 != null) {
		int idx = var1.lastIndexOf('_');
		if (idx != -1) {
			var2 = var1.substring(idx + 1);
			var1 = var1.substring(0, idx);
		}
	}
	if (lang != null) {
		lang = lang.toLowerCase();
	}

	ResourceBundle bundle = null;

	try {
		ResourceBundle nbundle = getSpecificBundle(baseName, loader);
		nbundle.setParent(bundle);
		bundle = nbundle;
	}
	catch (MissingResourceException _) {
	}

	if (lang != null) {
		try {
			ResourceBundle nbundle = getSpecificBundle(baseName
			    + "_" + lang, loader);
			nbundle.setParent(bundle);
			bundle = nbundle;
		}
		catch (MissingResourceException _) {
		}
	}

	if (lang != null && cntry != null) {
		try {
			ResourceBundle nbundle = getSpecificBundle(baseName
			    + "_" + lang + "_" + cntry, loader);
			nbundle.setParent(bundle);
			bundle = nbundle;
		}
		catch (MissingResourceException _) {
		}
	}

	if (lang != null && cntry != null && var1 != null) {
		try {
			ResourceBundle nbundle = getSpecificBundle(baseName
			    + "_" + lang + "_" + cntry + "_" + var1, loader);
			nbundle.setParent(bundle);
			bundle = nbundle;
		}
		catch (MissingResourceException _) {
		}
	}

	if (lang != null && cntry != null && var1 != null && var2 != null) {
		try {
			ResourceBundle nbundle = getSpecificBundle(baseName
			    + "_" + lang + "_" + cntry + "_"
			    + var1 + "_" + var2, loader);
			nbundle.setParent(bundle);
			bundle = nbundle;
		}
		catch (MissingResourceException _) {
		}
	}
	return (bundle);
}

public abstract Enumeration getKeys();

public final Object getObject(String key) throws MissingResourceException {
	try {
		Object obj = handleGetObject(key);
		if (obj != null) {
			return (obj);
		}
	}
	catch (MissingResourceException e) {
	}
	if (parent == null) {
		throw new MissingResourceException("resource not found",
		    this.getClass().toString(), key);
	}
	return (parent.getObject(key));
}

private static final ResourceBundle getSpecificBundle(String baseName,
		ClassLoader loader) throws MissingResourceException {

	// baseName = baseName.replace('.', '/');
	if (loader == null)
		loader = DummyClassLoader.getCurrentClassLoader();
	try {
		// Class cls = Class.forName(baseName);
		Class cls = loader.loadClass(baseName);
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

	// Okay, failed to load bundle - attempt to load properties as bundle.
	InputStream strm;
	strm = loader.getResourceAsStream(baseName.replace('.', '/')
	    + ".properties");
	if (strm != null) {
		try {
			return (new PropertyResourceBundle(strm));
		}
		catch (IOException _) {
		}
	}
	throw new MissingResourceException("bundle not found",
	    "ResourceBundle", baseName);
}

public final String getString(String key) throws MissingResourceException {
	return ((String)getObject(key));
}

public final String[] getStringArray(String key)
		throws MissingResourceException {
	return ((String[])getObject(key));
}

protected abstract Object handleGetObject(String key)
	throws MissingResourceException;

protected void setParent(ResourceBundle par) {
	parent = par;
}
}
