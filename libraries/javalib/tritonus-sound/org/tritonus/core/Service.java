/*
 *	Service.java
 */

/*
 *  Copyright (c) 2000 by Matthias Pfisterer <Matthias.Pfisterer@gmx.de>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as published
 *   by the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details.
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */


package	org.tritonus.core;


import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.URL;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.Iterator;
import java.util.List;
import java.util.Set;

import org.tritonus.share.ArraySet;
import org.tritonus.share.TDebug;



public class Service
{
	private static final String	BASE_NAME = "META-INF/services/";


	/**	Determines if the order of service providers is reversed.
		If this is true, the Iterator returned by providers(Class)
		iterates through the service provider classes backwards.
		This means that service providers that are in the user class
		path are first, then service providers in the extension class
		path, then those in the boot class path.
		This behaviour has the advantage that 'built-in' providers
		(those in the boot class path) can be 'shadowed' by
		providers in the extension and user class path.
	 */
	private static final boolean	REVERSE_ORDER = true;



	public static Iterator providers(Class cls)
	{
		if (TDebug.TraceService) { TDebug.out("Service.providers(): begin"); }
		String	strFullName = BASE_NAME + cls.getName();
		if (TDebug.TraceService) { TDebug.out("Service.providers(): full name: " + strFullName); }
		List	instancesList = createInstancesList(strFullName);
		Iterator	iterator = instancesList.iterator();
		if (TDebug.TraceService) { TDebug.out("Service.providers(): end"); }
		return iterator;
	}



	private static List createInstancesList(String strFullName)
	{
		if (TDebug.TraceService) { TDebug.out("Service.createInstancesList(): begin"); }
		List	providers = new ArrayList();
		Iterator	classNames = createClassNames(strFullName);
		if (classNames != null)
		{
			while (classNames.hasNext())
			{
				String	strClassName = (String) classNames.next();
				if (TDebug.TraceService) { TDebug.out("Service.createInstancesList(): Class name: " + strClassName); }
				try
				{
					ClassLoader	systemClassLoader = ClassLoader.getSystemClassLoader();
					Class	cls = Class.forName(strClassName, true, systemClassLoader);
					if (TDebug.TraceService) { TDebug.out("Service.createInstancesList(): now creating instance of " + cls); }
					Object	instance = cls.newInstance();
					if (REVERSE_ORDER)
					{
						providers.add(0, instance);
					}
					else
					{
						providers.add(instance);
					}
				}
				catch (ClassNotFoundException e)
				{
					if (TDebug.TraceService || TDebug.TraceAllExceptions) { TDebug.out(e); }
				}
				catch (InstantiationException e)
				{
					if (TDebug.TraceService || TDebug.TraceAllExceptions) { TDebug.out(e); }
				}
				catch (IllegalAccessException e)
				{
					if (TDebug.TraceService || TDebug.TraceAllExceptions) { TDebug.out(e); }
				}
				catch (Throwable e)
				{
					if (TDebug.TraceService || TDebug.TraceAllExceptions) { TDebug.out(e); }
				}
			}
		}
		if (TDebug.TraceService) { TDebug.out("Service.createInstancesList(): end"); }
		return providers;
	}



	private static Iterator createClassNames(String strFullName)
	{
		if (TDebug.TraceService) { TDebug.out("Service.createClassNames(): begin"); }
		Set	providers = new ArraySet();
		Enumeration	configs = null;
		try
		{
			configs = ClassLoader.getSystemResources(strFullName);
		}
		catch (IOException e)
		{
			if (TDebug.TraceService || TDebug.TraceAllExceptions) { TDebug.out(e); }
		}
		if (configs != null)
		{
			while (configs.hasMoreElements())
			{
				URL	configFileUrl = (URL) configs.nextElement();
				if (TDebug.TraceService) { TDebug.out("Service.createClassNames(): config: " + configFileUrl); }
				InputStream	input = null;
				try
				{
					input = configFileUrl.openStream();
				}
				catch (IOException e)
				{
					if (TDebug.TraceService || TDebug.TraceAllExceptions) { TDebug.out(e); }
				}
				if (input != null)
				{
					BufferedReader	reader = new BufferedReader(new InputStreamReader(input));
					try
					{
						String	strLine = reader.readLine();
						while (strLine != null)
						{
							strLine = strLine.trim();
							int	nPos = strLine.indexOf('#');
							if (nPos >= 0)
							{
								strLine = strLine.substring(0, nPos);
							}
							if (strLine.length() > 0)
							{
								providers.add(strLine);
								if (TDebug.TraceService) { TDebug.out("Service.createClassNames(): adding class name: " + strLine); }
							}
							strLine = reader.readLine();
						}
					}
					catch (IOException e)
					{
						if (TDebug.TraceService || TDebug.TraceAllExceptions) { TDebug.out(e); }
					}
				}
			}
		}
		Iterator	iterator = providers.iterator();
		if (TDebug.TraceService) { TDebug.out("Service.createClassNames(): end"); }
		return iterator;
	}
}



/*** Service.java ***/
