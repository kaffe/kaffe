/*
 *	AlsaSeqPortInfo.java
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



public class AlsaSeqPortInfo
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
	 *	Holds the pointer to snd_seq_port_info_t
	 *	for the native code.
	 *	This must be long to be 64bit-clean.
	 */
	/*private*/ long	m_lNativeHandle;



	public AlsaSeqPortInfo()
	{
		if (TDebug.TraceAlsaSeqNative) { TDebug.out("AlsaSeq.PortInfo.<init>(): begin"); }
		int	nReturn = malloc();
		if (nReturn < 0)
		{
			throw new RuntimeException("malloc of port_info failed");
		}
		if (TDebug.TraceAlsaSeqNative) { TDebug.out("AlsaSeq.PortInfo.<init>(): end"); }
	}



	public void finalize()
	{
		// TODO: call free()
		// call super.finalize() first or last?
		// and introduce a flag if free() has already been called?
	}



	private native int malloc();
	public native void free();



	public native int getClient();



	public native int getPort();


	/**	Returns the name of the port.
		Calls snd_seq_port_info_get_name().
	*/
	public native String getName();



	public native int getCapability();



	public native int getType();



	public native int getMidiChannels();



	public native int getMidiVoices();



	public native int getSynthVoices();



	public native int getReadUse();



	public native int getWriteUse();



	public native int getPortSpecified();

	private static native void setTrace(boolean bTrace);
}




/*** AlsaSeqPortInfo.java ***/
