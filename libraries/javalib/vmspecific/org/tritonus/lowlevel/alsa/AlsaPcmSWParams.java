/*
 *	AlsaPcmSWParams.java
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
public class AlsaPcmSWParams
{
	/**
	 *	Holds the pointer to snd_pcm_sw_params_t
	 *	for the native code.
	 *	This must be long to be 64bit-clean.
	 */
	private long	m_lNativeHandle;



	public AlsaPcmSWParams()
	{
		if (TDebug.TraceAlsaPcmNative) { TDebug.out("AlsaPcmSWParams.<init>(): begin"); }
		int	nReturn = malloc();
		if (nReturn < 0)
		{
			throw new RuntimeException("malloc of hw_params failed");
		}
		if (TDebug.TraceAlsaPcmNative) { TDebug.out("AlsaPcmSWParams.<init>(): end"); }
	}



	public void finalize()
	{
		// TODO: call free()
		// call super.finalize() first or last?
		// and introduce a flag if free() has already been called?
	}



	private native int malloc();
	public native void free();

	public native int getStartMode();
	public native int getXrunMode();
	public native int getTStampMode();
	public native int getSleepMin();
	public native int getAvailMin();
	public native int getXferAlign();
	public native int getStartThreshold();
	public native int getStopThreshold();
	public native int getSilenceThreshold();
	public native int getSilenceSize();
}



/*** AlsaPcmSWParams.java ***/
