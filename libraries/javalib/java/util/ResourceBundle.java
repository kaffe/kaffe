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

import kaffe.lang.ThreadStack;

public abstract class ResourceBundle {

	protected ResourceBundle parent;
	static Hashtable cache = new Hashtable();
	private Locale locale;

public ResourceBundle() {
	parent = null;
}

public static final ResourceBundle getBundle(String baseName)
		throws MissingResourceException {
	return getBundle(baseName, Locale.getDefault(),
			 ThreadStack.getCallersClassLoader(true));
}

public static final ResourceBundle getBundle(String baseName, Locale locale)
		throws MissingResourceException {
	return getBundle(baseName, locale,
			 ThreadStack.getCallersClassLoader(true));
}

public static ResourceBundle getBundle(String baseName, Locale locale,
		ClassLoader loader) throws MissingResourceException {
	
  List names = generateCandidateBundleNames(baseName, locale);
  Iterator iter = names.iterator();

  while (iter.hasNext()) {
    String name = (String) iter.next();

    ResourceBundle bundle = getFromCache(name, loader);
    if (bundle == null) {
      bundle = instantiate(name, loader);
    }

    if (bundle != null) {
      putInCache(name, loader, bundle);
      instantiateParentChain(bundle, name, loader);
      return bundle;
    }
  }

  throw new MissingResourceException("BaseName: " + baseName + " Locale: " + locale + " ClassLoader : " + loader, "ResourceBundle", baseName);
}


  private static ResourceBundle getFromCache(String name, ClassLoader loader) {
    
    return (ResourceBundle)cache.get (loader + name);
  }

  private static void putInCache(String name, ClassLoader loader, ResourceBundle bundle) {

    cache.put (loader + name, bundle);
  }

  private static void instantiateParentChain(ResourceBundle bundle, String name, ClassLoader loader) {

    int last_underscore = name.lastIndexOf('_');
    if (last_underscore != -1) {
      String parent_name = name.substring(0, last_underscore);
      ResourceBundle parent = instantiate(parent_name, loader);
      bundle.setParent(parent);
      if (parent != null && parent.parent == null) {
	instantiateParentChain(parent, parent_name, loader);
      }
    }
  }
  
  private static ResourceBundle loadProperties(String name, ClassLoader loader) {
    InputStream strm;
    strm = loader.getResourceAsStream(name.replace('.', '/')
				      + ".properties");
    if (strm != null) {
      try {
	return (new PropertyResourceBundle(strm));
      }
      catch (IOException e) {
	e.printStackTrace();
      }
    }
    
    return null;
  }
  
  private static ResourceBundle loadClass(String name, ClassLoader loader) {
    try {
      Class cls = Class.forName(name.replace('/', '.'), true, loader);
      /* 
       * Only call newInstance if the cast to resource bundle 
       * will indeed succeed.
       */
      if (ResourceBundle.class.isAssignableFrom(cls)) {
	return ((ResourceBundle)cls.newInstance());
      }
    }
    catch (ClassNotFoundException e) {
      // ignore
    }
    catch (LinkageError e) {
      e.printStackTrace();
    }
    catch (IllegalAccessException e) {
      e.printStackTrace();
    }
    catch (InstantiationException e) {
      e.printStackTrace();
    }

    return null;
  }

private static ResourceBundle instantiate(String name, ClassLoader loader) {

  ResourceBundle bundle = loadClass(name, loader);
  if (bundle != null) {
    return bundle;
  }

  bundle = loadProperties(name, loader);
  return bundle;
}

private static List generateCandidateBundleNames(String baseName, Locale locale) {
  
  String language1 = locale.getLanguage();
  String country1 = locale.getCountry();
  String variant1 = locale.getVariant();

  Locale default_locale = Locale.getDefault();

  String language2 = default_locale.getLanguage();
  String country2 = default_locale.getCountry();
  String variant2 = default_locale.getVariant();
 
  List names = new ArrayList();

  if (variant1.length() != 0) {
    names.add(baseName + '_' + language1 + '_' + country1 + '_' + variant1);
  }

  if (country1.length() != 0) {
    names.add(baseName + '_' + language1 + '_' + country1);
  }

  if (language1.length() != 0) {
    names.add(baseName + '_' + language1);
  }

  if (variant2.length() != 0) {
    names.add(baseName + '_' + language2 + '_' + country2 + '_' + variant2);
  }

  if (country2.length() != 0) {
    names.add(baseName + '_' + language2 + '_' + country2);
  }

  if (language2.length() != 0) {
    names.add(baseName + '_' + language2);
  }

  names.add(baseName);

  return names;
}

public Locale getLocale () {
	return locale;
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
	  e.printStackTrace();
	}
	if (parent == null) {
		throw new MissingResourceException("resource not found",
		    this.getClass().toString(), key);
	}
	return (parent.getObject(key));
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
