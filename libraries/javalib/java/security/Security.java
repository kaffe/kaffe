
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
	private static final Properties props = new Properties();
	private static final Vector providers = new Vector();

	// Read in master security properties from "java.security" file
	static {
	  readProps: {

		// Read in java.security file properties
		File file = new File(
		    System.getProperties().getProperty("java.home")
			+ "/lib/kaffe/security/java.security");
		if (!file.exists()) {
			break readProps;
		}
		try {
			props.load(new BufferedInputStream(
			    new FileInputStream(file)));
		} catch (FileNotFoundException e) {
			break readProps;
		} catch (IOException e) {
			break readProps;
		}

		// Install configured security providers
		for (Iterator i = props.entrySet().iterator(); i.hasNext(); ) {
			Map.Entry ent = (Map.Entry)i.next();
			String key = (String)ent.getKey();
			int position = 0;

			if (!key.startsWith(PROV_PREFIX)) {
				continue;
			}
			try {
				insertProviderAt(
				    (Provider)Class.forName(
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
	}

	// This class is not instantiable
	private Security() {
	}

	public static String getAlgorithmProperty(String alg, String prop) {

		throw new kaffe.util.NotImplemented(
		    Security.class.getName() + ".getAlgorithmProperty()");
	}

	public static int insertProviderAt(Provider provider, int position) {
		System.getSecurityManager().checkSecurityAccess(
		    "insertProvider." + provider.getName());
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
		System.getSecurityManager().checkSecurityAccess(
		    "insertProvider." + provider.getName());
		synchronized (providers) {
			if (findProvider(provider.getName()) >= 0) {
				return -1;
			}
			return insertProviderAt(provider, providers.size() + 1);
		}
	}

	public static void removeProvider(String name) {
		System.getSecurityManager().checkSecurityAccess(
		    "removeProvider." + name);
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
			int position = findProvider(name) + 1;
			return (position > 0) ?
			    (Provider)providers.elementAt(position) : null;
		}
	}

	public static String getProperty(String key) {
		System.getSecurityManager().checkPermission(
		    new SecurityPermission("getProperty." + key));
		synchronized (props) {
			return (String)props.get(key);
		}
	}

	public static void setProperty(String key, String value) {
		System.getSecurityManager().checkPermission(
		    new SecurityPermission("setProperty." + key));
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
		final Object engine;
		Engine(Provider provider, Object engine) {
			this.provider = provider;
			this.engine = engine;
		}
	}

	static Engine getCryptInstance(String engClass, String algorithm)
			throws NoSuchAlgorithmException {
		Provider[] providers = Security.getProviders();
		for (int i = 0; i < providers.length; i++) {
			try {
				return getCryptInstance(engClass,
				    algorithm, providers[i]);
			} catch (NoSuchAlgorithmException e) {
			}
		}
		throw new NoSuchAlgorithmException(algorithm);
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
		String alias = (String)p.get("Alg.Alias."
					+ engClass + "." + alg);
		if (alias != null) {
			alg = alias;
		}

		// Find class that implements the algorithm
		String name = (String)p.get(engClass + "." + alg);
		if (name == null) {
			throw new NoSuchAlgorithmException("algorithm \""
			    + alg+  "\" not defined by provider");
		}

		// Instantiate class
		try {
			return new Engine(p, Class.forName(name).newInstance());
		} catch (ClassNotFoundException e) {
			throw new NoSuchAlgorithmException("class "
				+ name + " not found");
		} catch (InstantiationException e) {
			throw new NoSuchAlgorithmException("cannot instantiate"
				+ " class " + name + ": " + e);
		} catch (IllegalAccessException e) {
			throw new NoSuchAlgorithmException("cannot instantiate"
				+ " class " + name + ": " + e);
		} catch (ClassCastException e) {
			throw new NoSuchAlgorithmException("cannot instantiate"
				+ " class " + name + ": wrong type");
		}
	}
}


