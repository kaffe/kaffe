/*
 *	EsdMixerProvider.java
 */

/*
 *  Copyright (c) 1999 - 2002 by Matthias Pfisterer <Matthias.Pfisterer@gmx.de>
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


package	org.tritonus.sampled.mixer.esd;


import org.tritonus.lowlevel.esd.Esd;
import org.tritonus.share.TDebug;
import org.tritonus.share.sampled.mixer.TMixerProvider;



public class EsdMixerProvider
	extends	TMixerProvider
{
	private static boolean	sm_bInitialized = false;



	public EsdMixerProvider()
	{
		super();
		if (TDebug.TraceMixerProvider) { TDebug.out("EsdMixerProvider.<init>(): begin"); }
		if (! sm_bInitialized && ! isDisabled())
		{
			/// TODO: adapt!
			if (! Esd.isLibraryAvailable())
			{
				disable();
			}
			else
			{
				staticInit();
				sm_bInitialized = true;
			}
		}
		else
		{
			if (TDebug.TraceMixerProvider) { TDebug.out("EsdMixerProvider.<init>(): already initialized or disabled"); }
		}

		if (TDebug.TraceMixerProvider) { TDebug.out("EsdMixerProvider.<init>(): end"); }
	}



	protected void staticInit()
	{
		if (TDebug.TraceMixerProvider) { TDebug.out("EsdMixerProvider.staticInit(): begin"); }
		addMixer(new EsdMixer());
		if (TDebug.TraceMixerProvider) { TDebug.out("EsdMixerProvider.staticInit(): end"); }
	}
}



/*** EsdMixerProvider.java ***/
