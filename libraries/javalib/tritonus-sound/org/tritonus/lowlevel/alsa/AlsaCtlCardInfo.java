/*
 *	AlsaCtlCardInfo.java
 */

/*
 *  Copyright (c) 2000 - 2002 by Matthias Pfisterer <Matthias.Pfisterer@web.de>
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
public class AlsaCtlCardInfo
{
	/**	Holds the pointer to snd_ctl_card_info_t.
		for the native code.
		This must be long to be 64bit-clean.
		The access modifier is not private because this
		variable has to be accessed from AlsaCtl.
	*/
	long	m_lNativeHandle;



	public AlsaCtlCardInfo()
	{
		if (TDebug.TraceAlsaCtlNative) { TDebug.out("AlsaPcm.CardInfo.<init>(): begin"); }
		int	nReturn = malloc();
		if (nReturn < 0)
		{
			throw new RuntimeException("malloc of card_info failed");
		}
		if (TDebug.TraceAlsaCtlNative) { TDebug.out("AlsaPcm.CardInfo.<init>(): end"); }
	}



	public void finalize()
	{
		// TODO: call free()
		// call super.finalize() first or last?
		// and introduce a flag if free() has already been called?
	}



	/**	Calls snd_ctl_card_info_malloc().
	 */
	private native int malloc();

	/**	Calls snd_ctl_card_info_free().
	 */
	public native void free();


	/**	Calls snd_ctl_card_info_get_card().
	 */
	public native int getCard();

	/**	Calls snd_ctl_card_info_get_id().
	 */
	public native String getId();

	/**	Calls snd_ctl_card_info_get_driver().
	 */
	public native String getDriver();

	/**	Calls snd_ctl_card_info_get_name().
	 */
	public native String getName();

	/**	Calls snd_ctl_card_info_get_longname().
	 */
	public native String getLongname();

	/**	Calls snd_ctl_card_info_get_mixername().
	 */
	public native String getMixername();

	/**	Calls snd_ctl_card_info_get_components().
	 */
	public native String getComponents();

	private static native void setTrace(boolean bTrace);
}



/*** AlsaCtlCardInfo.java ***/
