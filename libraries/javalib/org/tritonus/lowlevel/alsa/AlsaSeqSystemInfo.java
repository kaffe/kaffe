/*
 *	AlsaSeqSystemInfo.java
 */

/*
 *  Copyright (c) 1999 - 2001 by Matthias Pfisterer <Matthias.Pfisterer@gmx.de>
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

package	org.tritonus.lowlevel.alsa;

import org.tritonus.share.TDebug;



/**	General information about the sequencer.
 *	This class encapsulates the information of
 *	snd_seq_system_info_t.
 */
public class AlsaSeqSystemInfo
{
	static
	{
		Alsa.loadNativeLibrary();
		if (TDebug.TraceAlsaSeqNative)
		{
			setTrace(true);
		}
	}



	/**
	 *	Holds the pointer to snd_seq_system_info_t
	 *	for the native code.
	 *	This must be long to be 64bit-clean.
	 */
	/*private*/ long	m_lNativeHandle;



	public AlsaSeqSystemInfo()
	{
		if (TDebug.TraceAlsaSeqNative) { TDebug.out("AlsaSeq.SystemInfo.<init>(): begin"); }
		int	nReturn = malloc();
		if (nReturn < 0)
		{
			throw new RuntimeException("malloc of system_info failed");
		}
		if (TDebug.TraceAlsaSeqNative) { TDebug.out("AlsaSeq.SystemInfo.<init>(): end"); }
	}



	public void finalize()
	{
		// TODO: call free()
		// call super.finalize() first or last?
		// and introduce a flag if free() has already been called?
	}



	private native int malloc();
	public native void free();



	public native int getQueues();



	public native int getClients();



	public native int getPorts();



	public native int getChannels();



	public native int getCurrentClients();



	public native int getCurrentQueues();

	private static native void setTrace(boolean bTrace);
}




/*** AlsaSeqSystemInfo.java ***/
