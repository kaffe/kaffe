/*
 *	AlsaPcmHWParamsFormatMask.java
 */

/*
 *  Copyright (c) 2000 - 2001 by Matthias Pfisterer <Matthias.Pfisterer@gmx.de>
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


/** TODO:
 */
public class AlsaPcmHWParamsFormatMask
{
	/**
	 *	Holds the pointer to snd_pcm_format_mask_t
	 *	for the native code.
	 *	This must be long to be 64bit-clean.
	 */
	private long	m_lNativeHandle;



	public AlsaPcmHWParamsFormatMask()
	{
		if (TDebug.TraceAlsaPcmNative) { TDebug.out("AlsaPcmHWParamsFormatMask.<init>(): begin"); }
		int	nReturn = malloc();
		if (nReturn < 0)
		{
			throw new RuntimeException("malloc of format_mask failed");
		}
		if (TDebug.TraceAlsaPcmNative) { TDebug.out("AlsaPcmHWParamsFormatMask.<init>(): end"); }
	}



	public void finalize()
	{
		// TODO: call free()
		// call super.finalize() first or last?
		// and introduce a flag if free() has already been called?
	}


	/**
	 *	Calls snd_pcm_format_mask_malloc().
	 */
	private native int malloc();



	/**
	 *	Calls snd_pcm_format_mask_free().
	 */
	public native void free();

	/**
	 *	Calls snd_pcm_format_mask_none().
	 */
	public native void none();

	/**
	 *	Calls snd_pcm_format_mask_any().
	 */
	public native void any();

	/**
	 *	Calls snd_pcm_format_mask_test().
	 */
	public native boolean test(int nFormat);


	/**
	 *	Calls snd_pcm_format_mask_set().
	 */
	public native void set(int nFormat);


	/**
	 *	Calls snd_pcm_format_mask_reset().
	 */
	public native void reset(int nFormat);


}



/*** AlsaPcmHWParamsFormatMask.java ***/
