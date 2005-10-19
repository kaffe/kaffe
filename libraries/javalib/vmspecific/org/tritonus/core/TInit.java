/*
 *	TInit.java
 */

/*
 *  Copyright (c) 1999 - 2001 by Matthias Pfisterer <Matthias.Pfisterer@gmx.de>
 *
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

import java.util.Iterator;

import org.tritonus.share.TDebug;



/** Helper methods for provider registration.
 */
public class TInit
{
	/** Constructor to prevent instantiation.
	 */
	private TInit()
	{
	}



	/** Register all service providers of a certain type.
	    This method retrieves instances of all service providers of
	    the type given as providerClass. It registers them by
	    calling action with the provider instance as actual parameter.

	    @param providerClass Type of the service providers that should
	    be registered. For instance, this could be the class object for
	    javax.sound.sampled.spi.MixerProvider. However, the mechanism
	    is not restricted to the Java Sound types of service providers.

	    @param action A ProviderRegistrationAction that should to be
	    called to register the service providers. Typically, this is
	    something like adding the provider to a collection, but in
	    theorie, could be anything.
	*/
	public static void registerClasses(Class providerClass,
					    ProviderRegistrationAction action)
	{
		if (TDebug.TraceInit) { TDebug.out("TInit.registerClasses(): registering for: " + providerClass); }
		Iterator	providers = Service.providers(providerClass);
		if (providers != null)
		{
			while (providers.hasNext())
			{
				Object	provider = providers.next();
				try
				{
					action.register(provider);
				}
				catch (Throwable e)
				{
					if (TDebug.TraceInit || TDebug.TraceAllExceptions) { TDebug.out(e); }
				}
			}
		}
	}



	/** Action to be taken on registration of a provider.
	    Strategy objects of this type has to be passed to
	    {@link #registerClasses registerClasses}. The implementation
	    is called for each provider that has to be registered.
	 */
	public static interface ProviderRegistrationAction
	{
		public void register(Object provider)
			throws Exception;
	}
}



/*** TInit.java ***/
