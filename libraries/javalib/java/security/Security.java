
/*
 * Java core library component.
 *
 * Copyright (c) 1999
 *	Archie L. Cobbs.  All rights reserved.
 * Copyright (c) 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Author: Archie L. Cobbs <archie@whistle.com>
 */

package java.security;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.Iterator;
import java.util.Map;
import java.util.Properties;
import java.util.Vector;

public final class Security {
	private static final String PROV_PREFIX = "security.provider.";
	private static final String DEF_PROV = "kaffe.security.provider.Kaffe";
	private static final Properties props = new Properties();
	private static final Vector providers = new Vector();

	static {

		// Set default security provider if none specified
		props.put(PROV_PREFIX + "1", DEF_PROV);

		// Read in master security properties from "java.security" file
		readProps: {
			File file = new File(
			    System.getProperties().getProperty("java.home")
				+ "/jre/lib/security/java.security");
			if (file.exists()) {
				try {
					props.load(
					    new BufferedInputStream(
					    new FileInputStream(file)));
				} catch (FileNotFoundException e) {
				} catch (IOException e) {
				}
			}
		}

		// Install configured security providers
		for (Iterator i = props.entrySet().iterator(); i.hasNext(); ) {
			ClassLoader cl = ClassLoader.getSystemClassLoader();
			Map.Entry ent = (Map.Entry)i.next();
			String key = (String)ent.getKey();

			if (!key.startsWith(PROV_PREFIX)) {
				continue;
			}
			try {
				insertProviderAt(
				    (Provider)cl.loadClass(
					(String)ent.getValue()).newInstance(),
				    Integer.parseInt(
					key.substring(PROV_PREFIX.length())));
			} catch (NumberFormatException e) {
			} catch (ClassNotFoundException e) {
			} catch (InstantiationException e) {
			} catch (IllegalAccessException e) {
			} catch (IllegalArgumentException e) {
			} catch (ClassCastException e) {
			}
		}

		// Should also read Policy stuff here...
	}

	// This class is not instantiable
	private Security() {
	}

	public static String getAlgorithmProperty(String alg, String prop) {
		String id = "Alg." + prop + "." + alg;
		for (int i = 0; i < providers.size(); i++) {
			Provider p = (Provider)providers.elementAt(i);
			String val = p.getProperty(id);
			if (val != null) {
				return (val);
			}
		}
		return (null);
	}

	public static int insertProviderAt(Provider provider, int position) {
		SecurityManager sm = System.getSecurityManager();
		if (sm != null)
			sm.checkSecurityAccess("insertProvider." + provider.getName());
		if (--position < 0) {
			throw new IllegalArgumentException();
		}
		synchronized (providers) {
			int tempPosition;
			if ((tempPosition
			    = findProvider(provider.getName())) >= 0) {
				return tempPosition + 1;
			}
			if (position > providers.size()) {
				position = providers.size();
			}
			providers.insertElementAt(provider, position);
		}
		return position + 1;
	}

	public static int addProvider(Provider provider) {
		SecurityManager sm = System.getSecurityManager();
		if (sm != null)
			sm.checkSecurityAccess("insertProvider." + provider.getName());
		synchronized (providers) {
			if (findProvider(provider.getName()) >= 0) {
				return -1;
			}
			return insertProviderAt(provider, providers.size() + 1);
		}
	}

	public static void removeProvider(String name) {
		SecurityManager sm = System.getSecurityManager();
		if (sm != null)
			sm.checkSecurityAccess("removeProvider." + name);
		synchronized (providers) {
			int posn = findProvider(name);
			if (posn >= 0) {
					providers.removeElementAt(posn);
			}

		}
	}

	public static Provider[] getProviders() {
		synchronized (providers) {
			Provider[] pa = new Provider[providers.size()];
			System.arraycopy(
				providers.toArray(), 0, pa, 0, pa.length);
			return pa;
		}
	}

	public static Provider getProvider(String name) {
		synchronized (providers) {
			int position = findProvider(name);
			return (position >= 0) ?
			    (Provider)providers.elementAt(position) : null;
		}
	}

	public static String getProperty(String key) {
		SecurityManager sm = System.getSecurityManager();
		if (sm != null)
			sm.checkPermission(new SecurityPermission("getProperty." + key));
		synchronized (props) {
			return (String)props.get(key);
		}
	}

	public static void setProperty(String key, String value) {
		SecurityManager sm = System.getSecurityManager();
		if (sm != null)
			sm.checkPermission(new SecurityPermission("setProperty." + key));
		synchronized (props) {
			props.put(key, value);
		}
	}

	private static int findProvider(String name) {
		for (int i = 0; i < providers.size(); i++) {
			Provider p = (Provider)providers.elementAt(i);
			if (p.getName().equals(name)) {
				return i;
			}
		}
		return -1;
	}
	
	static class Engine {
		final Provider provider;
		final String algorithm;
		final Object engine;
		Engine(Provider provider, String algorithm, Object engine) {
			this.provider = provider;
			this.algorithm = algorithm;
			this.engine = engine;
		}

		Provider getProvider()
		{
			return this.provider;
		}
		
		String getAlgorithm()
		{
			return this.algorithm;
		}

		Object getEngine()
		{
			return this.engine;
		}
	}

	static Engine getCryptInstance(String engClass)
			throws NoSuchAlgorithmException {
		return getCryptInstance(engClass, null);
	}

	static Engine getCryptInstance(String engClass, String algorithm)
			throws NoSuchAlgorithmException {
		Provider[] providers = getProviders();
		for (int i = 0; i < providers.length; i++) {
			try {
				return getCryptInstance(engClass,
				    algorithm, providers[i]);
			} catch (NoSuchAlgorithmException e) {
			}
		}
		throw algorithm == null ?
			new NoSuchAlgorithmException() :
			new NoSuchAlgorithmException(algorithm);
	}

	static Engine getCryptInstance(String engClass, String alg, String prov)
		    throws NoSuchAlgorithmException, NoSuchProviderException {

		// Make sure provider is installed
		Provider p = getProvider(prov);
		if (p == null) {
			throw new NoSuchProviderException(prov);
		}
		return getCryptInstance(engClass, alg, p);
	}

	static Engine getCryptInstance(String engClass, String alg, Provider p)
		    throws NoSuchAlgorithmException {

		// See if algorithm name is an alias
		if (alg != null) {
			String alias = (String)p.get("Alg.Alias."
						+ engClass + "." + alg);
			if (alias != null) {
				alg = alias;
			}
		}

		// Find a class that implements the class and algorithm
		String name = null;
		if (alg != null) {
			name = (String)p.get(engClass + "." + alg);
		} else {
			String prefix = engClass + ".";
			for (Iterator i = p.entrySet().iterator();
					i.hasNext(); ) {
				Map.Entry e = (Map.Entry)i.next();
				String key = (String)e.getKey();
				if (key.startsWith(prefix)) {
					alg = key.substring(prefix.length());
					name = (String)e.getValue();
					break;
				}
			}
		}
		if (name == null) {
			throw new NoSuchAlgorithmException(
			  "\"" + alg + "\" not supported by provider");
		}

		// Instantiate class
		try {
			return new Engine(p, alg,
				Class.forName(name).newInstance());
		} catch (ClassNotFoundException e) {
			throw new NoSuchAlgorithmException("class "
				+ name + " not found");
		} catch (Exception e) {
			throw new NoSuchAlgorithmException("can't instantiate"
				+ " class " + name + ": " + e);
		}
	}
}


