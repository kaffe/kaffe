/*
 *	AlsaMixer.java
 */

/*
 *  Copyright (c) 2001 by Matthias Pfisterer <Matthias.Pfisterer@gmx.de>
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



/**	Object carrying a snd_mixer_t.
 */
public class AlsaMixer
{
	/*
	  not private because needed to be accessed by AlsaMixerElement.
	  (Better solution: inner classes)
	*/
	/*private*/ long	m_lNativeHandle;



	static
	{
		Alsa.loadNativeLibrary();
		if (TDebug.TraceAlsaMixerNative)
		{
			setTrace(true);
		}
	}



	public AlsaMixer(String strMixerName)
		throws	Exception
	{
		if (open(0) < 0)
		{
			throw new Exception();
		}
		if (attach(strMixerName) < 0)
		{
			close();
			throw new Exception();
		}
		if (register() < 0)
		{
			close();
			throw new Exception();
		}
		if (load() < 0)
		{
			close();
			throw new Exception();
		}
	}


	/**	Calls snd_mixer_open().
	 */
	private native int open(int nMode);


	/**	Calls snd_mixer_attach().
	 */
	private native int attach(String strCardName);

	/**	Calls snd_mixer_selem_register(.., NULL, NULL).
		This is a hack, taken over from alsamixer.
	*/
	private native int register();

	/**	Calls snd_mixer_load().
	 */
	private native int load();


	/**	Calls snd_mixer_free().
	 */
	private native int free();


	/**	Calls snd_mixer_close().
	 */
	public native int close();


	// getCount() ??

	/**
	   The caller has to allocate the indices and names arrays big
	   enough to hold information on all controls. If the retrieving
	   of controls is successful, a positive number (the number of
	   controls) is returned. If the arrays are not big enough, -1
	   is returned. In this case, it's the task of the caller to allocate
	   bigger arrays and try again.
	   Both arrays must be of the same size.

	   Calls snd_mixer_first_elem() and snd_mixer_elem_next().
	 */
	public native int readControlList(int[] anIndices, String[] astrNames);



	public static native void setTrace(boolean bTrace);
}



/*** AlsaMixer.java ***/
