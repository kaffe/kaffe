/*
 *	AlsaSeqEvent.java
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



/**	Event for the sequencer.
 *	This class encapsulates an instance of
 *	snd_seq_event_t.
 */
public class AlsaSeqEvent
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
	 *	Holds the pointer to snd_seq_event_t
	 *	for the native code.
	 *	This must be long to be 64bit-clean.
	 */
	/*private*/ long	m_lNativeHandle;



	public AlsaSeqEvent()
	{
		if (TDebug.TraceAlsaSeqNative) { TDebug.out("AlsaSeq.Event.<init>(): begin"); }
		int	nReturn = malloc();
		if (nReturn < 0)
		{
			throw new RuntimeException("malloc of event failed");
		}
		if (TDebug.TraceAlsaSeqNative) { TDebug.out("AlsaSeq.Event.<init>(): end"); }
	}



	public void finalize()
	{
		// TODO: call free()
		// call super.finalize() first or last?
		// and introduce a flag if free() has already been called?
	}



	/** Allocates memory for a snd_seq_event_t.

	The native part of this method uses calloc() to
	allocate the memory (so the allocated memory is
	zero'ed).  The memory reference is stored in {@link
	#m_lNativeHandle m_lNativeHandle}.  Memory allocated
	with this call should be freed by calling {@link
	#free() free()}.

	*/
	private native int malloc();

	/** Frees memory for a snd_seq_event_t.
	 */
	public native void free();

	// TODO: implement natively
	public native int getLength();

	public native int getType();
	public native int getFlags();
	public native int getTag();
	public native int getQueue();
	public native long getTimestamp();
	public native int getSourceClient();
	public native int getSourcePort();
	public native int getDestClient();
	public native int getDestPort();



	/*	Retrieves the parameters of a note event.
	 *	This method is suitable for the following event types:
	 *	SND_SEQ_EVENT_NOTE
	 *	SND_SEQ_EVENT_NOTEON
	 *	SND_SEQ_EVENT_NOTEOFF
	 *	SND_SEQ_EVENT_KEYPRESS
	 *
	 *	After return, the array will contain:
	 *	anValues[0]	channel
	 *	anValues[1]	note
	 *	anValues[2]	velocity
	 *	anValues[3]	off_velocity
	 *	anValues[4]	duration
	 */
	public native void getNote(int[] anValues);



	/*	Retrieves the parameters of a control event.
	 *	This method is suitable for the following event types:
	 *	SND_SEQ_EVENT_CONTROLLER
	 *	SND_SEQ_EVENT_PGMCHANGE
	 *	SND_SEQ_EVENT_CHANPRESS
	 *	SND_SEQ_EVENT_PITCHBEND
	 *	SND_SEQ_EVENT_CONTROL14
	 *	SND_SEQ_EVENT_NONREGPARAM
	 *	SND_SEQ_EVENT_REGPARAM
	 *	SND_SEQ_EVENT_SONGPOS
	 *	SND_SEQ_EVENT_SONGSEL
	 *	SND_SEQ_EVENT_QFRAME
	 *	SND_SEQ_EVENT_TIMESIGN
	 *	SND_SEQ_EVENT_KEYSIGN
	 *
	 *	After return, the array will contain:
	 *	anValues[0]	channel
	 *	anValues[1]	param
	 *	anValues[2]	value
	 */
	public native void getControl(int[] anValues);



	/*	Retrieves the parameters of a queue control event.
	 *	This method is suitable for the following event types:
	 *	SND_SEQ_EVENT_START
	 *	SND_SEQ_EVENT_CONTINUE
	 *	SND_SEQ_EVENT_STOP
	 *	SND_SEQ_EVENT_SETPOS_TICK
	 *	SND_SEQ_EVENT_SETPOS_TIME
	 *	SND_SEQ_EVENT_TEMPO
	 *	SND_SEQ_EVENT_CLOCK
	 *	SND_SEQ_EVENT_TICK
	 *	SND_SEQ_EVENT_SYNC
	 *	SND_SEQ_EVENT_SYNC_POS
	 *
	 *	After return, the array will contain:
	 *	anValues[0]	queue
	 *	anValues[1]	value
	 *	alValues[0]	time
	 */
	public native void getQueueControl(int[] anValues, long[] alValues);



	/*	Retrieves the parameters of a variable-length event.
	 *	This method is suitable for the following event types:
	 *	SND_SEQ_EVENT_SYSEX
	 *	SND_SEQ_EVENT_BOUNCE
	 *	SND_SEQ_EVENT_USR_VAR0
	 *	SND_SEQ_EVENT_USR_VAR1
	 *	SND_SEQ_EVENT_USR_VAR2
	 *	SND_SEQ_EVENT_USR_VAR3
	 *	SND_SEQ_EVENT_USR_VAR4
	 *
	 */
	public native byte[] getVar();


	public native void setCommon(int nType, int nFlags, int nTag, int nQueue, long lTimestamp, int nSourceClient, int nSourcePort, int nDestClient, int nDestPort);

	public native void setTimestamp(long lTimestamp);

	public native void setNote(int nChannel, int nKey, int nVelocity, int nOffVelocity, int nDuration);
	public native void setControl(int nChannel, int nParam, int nValue);
	public native void setQueueControl(int nControlQueue, int nControlValue, long lControlTime);
	public native void setVar(byte[] abData, int nOffset, int nLength);

	private static native void setTrace(boolean bTrace);
}





/*** AlsaSeqEvent.java ***/
