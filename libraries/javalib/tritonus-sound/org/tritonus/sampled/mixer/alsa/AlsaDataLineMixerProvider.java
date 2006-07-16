/*
 *	AlsaDataLineMixerProvider.java
 */

/*
 *  Copyright (c) 1999 - 2002 by Matthias Pfisterer <Matthias.Pfisterer@gmx.de>
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
 */

package	org.tritonus.sampled.mixer.alsa;


import org.tritonus.lowlevel.alsa.Alsa;
import org.tritonus.lowlevel.alsa.AlsaCtl;
import org.tritonus.lowlevel.alsa.AlsaCtlCardInfo;
import org.tritonus.share.TDebug;
import org.tritonus.share.sampled.mixer.TMixerProvider;



public class AlsaDataLineMixerProvider
	extends	TMixerProvider
{
	private static boolean	sm_bInitialized = false;


	public AlsaDataLineMixerProvider()
	{
		super();
		if (TDebug.TraceMixerProvider) { TDebug.out("AlsaDataLineMixerProvider.<init>(): begin"); }
		if (! sm_bInitialized && ! isDisabled())
		{
			if (! Alsa.isLibraryAvailable())
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
			if (TDebug.TraceMixerProvider) { TDebug.out("AlsaDataLineMixerProvider.<init>(): already initialized or disabled"); }
		}

		if (TDebug.TraceMixerProvider) { TDebug.out("AlsaDataLineMixerProvider.<init>(): end"); }
	}



	protected void staticInit()
	{
		if (TDebug.TraceMixerProvider) { TDebug.out("AlsaDataLineMixerProvider.staticInit(): begin"); }
		int[]	anCards = AlsaCtl.getCards();
		if (TDebug.TraceMixerProvider) { TDebug.out("AlsaDataLineMixerProvider.staticInit(): num cards: " + anCards.length); }
		for (int i = 0; i < anCards.length; i++)
		{
			if (TDebug.TraceMixerProvider) { TDebug.out("AlsaDataLineMixerProvider.staticInit():card #" + i + ": " + anCards[i]); }
			if (TDebug.TraceMixerProvider) { TDebug.out("AlsaDataLineMixerProvider.staticInit(): creating Ctl object..."); }
			String	strPcmName = "hw:" + anCards[i];
			// String	strPcmName = AlsaDataLineMixer.getPcmName(anCards[i]);
			AlsaCtl	ctl = null;
			try
			{
				ctl = new AlsaCtl(strPcmName, 0);
			}
			catch (Exception e)
			{
				if (TDebug.TraceMixerProvider || TDebug.TraceAllExceptions) { TDebug.out(e); }
				continue;
			}
			if (TDebug.TraceMixerProvider) { TDebug.out("AlsaDataLineMixerProvider.staticInit(): calling getCardInfo()..."); }
			AlsaCtlCardInfo	cardInfo = new AlsaCtlCardInfo();
			ctl.getCardInfo(cardInfo);
			if (TDebug.TraceMixerProvider)
			{
				TDebug.out("AlsaDataLineMixerProvider.staticInit(): ALSA sound card:");
				TDebug.out("AlsaDataLineMixerProvider.staticInit(): card: " + cardInfo.getCard());
				TDebug.out("AlsaDataLineMixerProvider.staticInit(): id: " + cardInfo.getId());
			}
			int[]	anDevices = ctl.getPcmDevices();
			if (TDebug.TraceMixerProvider) { TDebug.out("AlsaDataLineMixerProvider.staticInit(): num devices: " + anDevices.length); }
			// TODO: combine devices into one AlsaDataLineMixer?
			// pass device number to AlsaDataLineMixer constructor?
			for (int nDevice = 0; nDevice < anDevices.length; nDevice++)
			{
				if (TDebug.TraceMixerProvider) { TDebug.out("AlsaDataLineMixerProvider.staticInit(): device #" + nDevice + ": " +  anDevices[nDevice]); }
			}
			// ctl.close();

			/*
			  We do not use strPcmName because the mixer may choose to open as 'plughw', while for ctl, the device name always has to be 'hw'.
			*/
			AlsaDataLineMixer	mixer = new AlsaDataLineMixer(anCards[i]);
			super.addMixer(mixer);
		}
		if (TDebug.TraceMixerProvider) { TDebug.out("AlsaDataLineMixerProvider.staticInit(): end"); }
	}
}



/*** AlsaDataLineMixerProvider.java ***/
