/*
 *	AlsaSeqPortSubscribe.java
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



public class AlsaSeqPortSubscribe
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



	public AlsaSeqPortSubscribe()
	{
		if (TDebug.TraceAlsaSeqNative) { TDebug.out("AlsaSeq.PortSubscribe.<init>(): begin"); }
		int	nReturn = malloc();
		if (nReturn < 0)
		{
			throw new RuntimeException("malloc of port_info failed");
		}
		if (TDebug.TraceAlsaSeqNative) { TDebug.out("AlsaSeq.PortSubscribe.<init>(): end"); }
	}



	public void finalize()
	{
		// TODO: call free()
		// call super.finalize() first or last?
		// and introduce a flag if free() has already been called?
	}



	private native int malloc();
	public native void free();



	public native int getSenderClient();
	public native int getSenderPort();
	public native int getDestClient();
	public native int getDestPort();

	public native int getQueue();

	public native boolean getExclusive();
	public native boolean getTimeUpdate();
	public native boolean getTimeReal();

	public native void setSender(int nClient, int nPort);
	public native void setDest(int nClient, int nPort);

	public native void setQueue(int nQueue);

	public native void setExclusive(boolean bExclusive);
	public native void setTimeUpdate(boolean bUpdate);
	public native void setTimeReal(boolean bReal);

	private static native void setTrace(boolean bTrace);
}




/*** AlsaSeqPortSubscribe.java ***/
