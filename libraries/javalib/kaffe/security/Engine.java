
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

package kaffe.security;

import java.util.Map;
import java.util.Iterator;

import java.security.Provider;
import java.security.Security;
import java.security.NoSuchProviderException;
import java.security.NoSuchAlgorithmException;

public class Engine
{
	private final Provider provider;
	private final String algorithm;
	private final Object engine;
	
	private Engine(Provider provider, String algorithm, Object engine)
	{
		this.provider = provider;
		this.algorithm = algorithm;
		this.engine = engine;
	}

	public Provider getProvider()
	{
		return this.provider;
	}

	public String getAlgorithm()
	{
		return this.algorithm;
	}

	public Object getEngine()
	{
		return this.engine;
	}
	
	public static Engine getCryptInstance(String engClass)
		throws NoSuchAlgorithmException
	{
		return getCryptInstance(engClass, null);
	}

	public static Engine getCryptInstance(String engClass,
					      String algorithm)
		throws NoSuchAlgorithmException
	{
		Provider[] providers = Security.getProviders();
		
		for (int i = 0; i < providers.length; i++)
		{
			try
			{
				return getCryptInstance(engClass,
							algorithm,
							providers[i]);
			}
			catch (NoSuchAlgorithmException e)
			{
			}
		}
		throw algorithm == null ?
			new NoSuchAlgorithmException() :
			new NoSuchAlgorithmException(algorithm);
	}

	public static Engine getCryptInstance(String engClass,
					      String alg,
					      String prov)
	    throws NoSuchAlgorithmException,
		   NoSuchProviderException
	{
		// Make sure provider is installed
		Provider p = Security.getProvider(prov);

		if (p == null)
		{
			throw new NoSuchProviderException(prov);
		}
		return getCryptInstance(engClass, alg, p);
	}

	public static Engine getCryptInstance(String engClass,
					      String alg,
					      Provider p)
	    throws NoSuchAlgorithmException
	{
		// See if algorithm name is an alias
		if (alg != null)
		{
			String alias = (String)p.get("Alg.Alias."
						     + engClass + "." + alg);
			
			if (alias != null)
			{
				alg = alias;
			}
		}

		// Find a class that implements the class and algorithm
		String name = null;
		
		if (alg != null)
		{
			name = (String)p.get(engClass + "." + alg);
		}
		else
		{
			Iterator i = p.entrySet().iterator();
			String prefix = engClass + ".";
			
			while( i.hasNext() )
			{
				Map.Entry e = (Map.Entry)i.next();
				String key = (String)e.getKey();
				
				if (key.startsWith(prefix))
				{
					alg = key.substring(prefix.length());
					name = (String)e.getValue();
					break;
				}
			}
		}
		if (name == null)
		{
			throw new NoSuchAlgorithmException(
			  "\"" + alg + "\" not supported by provider");
		}

		// Instantiate class
		try
		{
			return new Engine(p, alg,
					  Class.forName(name).newInstance());
		}
		catch (ClassNotFoundException e)
		{
			throw new NoSuchAlgorithmException("class "
				+ name + " not found");
		}
		catch (Exception e)
		{
			throw new NoSuchAlgorithmException("can't instantiate"
				+ " class " + name + ": " + e);
		}
	}
}
