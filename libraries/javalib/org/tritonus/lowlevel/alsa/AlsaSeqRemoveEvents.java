/*
 *	AlsaSeqRemoveEvents.java
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
 *
 */

package	org.tritonus.lowlevel.alsa;

import org.tritonus.share.TDebug;



/** TODO:
 */
public class AlsaSeqRemoveEvents
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
	 *	Holds the pointer to snd_seq_queue_timer_t
	 *	for the native code.
	 *	This must be long to be 64bit-clean.
	 */
	/*private*/ long	m_lNativeHandle;



	static
	{
		Alsa.loadNativeLibrary();
		if (TDebug.TraceAlsaSeqNative)
		{
			setTrace(true);
		}
	}



	public AlsaSeqRemoveEvents()
	{
		if (TDebug.TraceAlsaSeqNative) { TDebug.out("AlsaSeqRemoveEvents.<init>(): begin"); }
		int	nReturn = malloc();
		if (nReturn < 0)
		{
			throw new RuntimeException("malloc of port_info failed");
		}
		if (TDebug.TraceAlsaSeqNative) { TDebug.out("AlsaSeqRemoveEvents.<init>(): end"); }
	}



	public void finalize()
	{
		// TODO: call free()
		// call super.finalize() first or last?
		// and introduce a flag if free() has already been called?
	}



	private native int malloc();
	public native void free();

	public native int getCondition();
	public native int getQueue();
	public native long getTime();
	public native int getDestClient();
	public native int getDestPort();
	public native int getChannel();
	public native int getEventType();
	public native int getTag();

	public native void setCondition(int nCondition);
	public native void setQueue(int nQueue);
	public native void setTime(long lTime);
	public native void setDest(int nClient, int nPort);
	public native void setChannel(int nChannel);
	public native void setEventType(int nEventType);
	public native void setTag(int nTag);

	private static native void setTrace(boolean bTrace);
}



/*** AlsaSeqRemoveEvents.java ***/
