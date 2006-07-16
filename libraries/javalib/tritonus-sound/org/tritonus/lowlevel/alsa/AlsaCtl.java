/*
 *	AlsaCtl.java
 */

/*
 *  Copyright (c) 2000 - 2001 by Matthias Pfisterer <Matthias.Pfisterer@web.de>
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

package	org.tritonus.lowlevel.alsa;

import org.tritonus.share.TDebug;



/**	TODO:
 */
public class AlsaCtl
{
	/** contains a pointer to snd_ctl_t
	 */
	private long	m_lNativeHandle;



	static
	{
		Alsa.loadNativeLibrary();
		if (TDebug.TraceAlsaCtlNative)
		{
			setTrace(true);
		}
	}



	public static native int loadCard(int nCard);
	// this encapsulates snd_card_next()
	public static native int[] getCards();
	public static native int getCardIndex(String strName);
	public static native String getCardName(int nCard);
	public static native String getCardLongName(int nCard);



	/**	Open a ctl.

		Objects created with this constructor have to be
		closed by calling {@link #close() close()}. This is
		necessary to free native resources.

		@param strName The name of the sound card. For
		instance, "hw:0", or an identifier you gave the
		card ("CARD1").

		@param nMode Special modes for the low-level opening
		like SND_CTL_NONBLOCK, SND_CTL_ASYNC. Normally, set
		this to 0.

	*/
	public AlsaCtl(String strName, int nMode)
		throws	Exception
	{
		if (open(strName, nMode) < 0)
		{
			throw new Exception();
		}
	}



	public AlsaCtl(int nCard)
		throws	Exception
	{
		this("hw:" + nCard, 0);
	}


	/**	Calls snd_ctl_open().
	 */
	private native int open(String strName, int nMode);

	/**	Calls snd_ctl_close().
	 */
	public native int close();

	/**	Calls snd_ctl_card_info().
	 */
	public native int getCardInfo(AlsaCtlCardInfo cardInfo);


	// TODO: ??
	public native int[] getPcmDevices();

	// TODO: remove
	/**
	 *	anValues[0]	device (inout)
	 *	anValues[1]	subdevice (inout)
	 *	anValues[2]	stream (inout)
	 *	anValues[3]	card (out)
	 *	anValues[4]	class (out)
	 *	anValues[5]	subclass (out)
	 *	anValues[6]	subdevice count (out)
	 *	anValues[7]	subdevice available (out)
	 *
	 *	astrValues[0]	id (out)
	 *	astrValues[1]	name (out)
	 *	astrValues[2]	subdevice name (out)
	 */
	public native int getPcmInfo(int[] anValues, String[] astrValues);


	private static native void setTrace(boolean bTrace);
}



/*** AlsaCtl.java ***/
