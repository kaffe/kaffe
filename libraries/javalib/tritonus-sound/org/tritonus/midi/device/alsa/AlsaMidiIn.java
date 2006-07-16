/*
 *	AlsaMidiIn.java
 */

/*
 *  Copyright (c) 1999 - 2001 by Matthias Pfisterer <Matthias.Pfisterer@gmx.de>
 *
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

package	org.tritonus.midi.device.alsa;


import javax.sound.midi.InvalidMidiDataException;
import javax.sound.midi.MetaMessage;
import javax.sound.midi.MidiEvent;
import javax.sound.midi.MidiMessage;
import javax.sound.midi.ShortMessage;
import javax.sound.midi.SysexMessage;

import org.tritonus.lowlevel.alsa.AlsaSeq;
import org.tritonus.lowlevel.alsa.AlsaSeqEvent;
import org.tritonus.lowlevel.alsa.AlsaSeqPortSubscribe;
import org.tritonus.share.TDebug;



/**	Handles input from an ALSA port.
 */
public class AlsaMidiIn
	extends		Thread
{
	/**	ALSA client used to receive events.
	 */
	private AlsaSeq			m_alsaSeq;

	/**	ALSA port number (belonging to the client represented be
		m_alsaSeq) used to receive events.
	*/
	private int			m_nDestPort;

	/**	ALSA client number to subscribe to to receive events.
	 */
	private int			m_nSourceClient;

	/**	ALSA port number (belonging to m_nSourceClient) to
		subscribe to to receive events.
	*/
	private int			m_nSourcePort;

	private AlsaMidiInListener	m_listener;

	private AlsaSeqEvent		m_event = new AlsaSeqEvent();

	// used to query event for detailed information
	private int[]			m_anValues = new int[5];
	private long[]			m_alValues = new long[1];



	/**	Receives events without timestamping them.
		Does establish a subscription where events are routed directely
		(not getting a timestamp).

		@param alsaSeq The client that should be used to receive
		events.

		@param nDestPort The port number that should be used to receive
		events. This port has to exist on the client represented by
		alsaSeq.

		@param nSourceClient The client number that should be listened
		to. This and nSourcePort must exist prior to calling this
		constructor. The port has to allow read subscriptions.

		@param nSourcePort  The port number that should be listened
		to. This and nSourceClient must exist prior to calling this
		constructor. The port has to allow read subscriptions.

		@param listener The listener that should receive the
		MidiMessage objects created here from received events.
	*/
	public AlsaMidiIn(AlsaSeq alsaSeq,
			  int nDestPort,
			  int nSourceClient,
			  int nSourcePort,
			  AlsaMidiInListener listener)
	{
		this(alsaSeq,
		     nDestPort,
		     nSourceClient,
		     nSourcePort,
		     -1, false,		// signals: do not do timestamping
		     listener);
	}



	/**
	   Does establish a subscription where events are routed through
	   a queue to get a timestamp.
	*/
	public AlsaMidiIn(AlsaSeq alsaSeq,
			  int nDestPort,
			  int nSourceClient,
			  int nSourcePort,
			  int nTimestampingQueue,
			  boolean bRealtime,
			  AlsaMidiInListener listener)
	{
		m_nSourceClient = nSourceClient;
		m_nSourcePort = nSourcePort;
		m_listener = listener;
		m_alsaSeq = alsaSeq;
		m_nDestPort = nDestPort;
		if (nTimestampingQueue >= 0)
		{
			AlsaSeqPortSubscribe	portSubscribe = new AlsaSeqPortSubscribe();
			portSubscribe.setSender(nSourceClient, nSourcePort);
			portSubscribe.setDest(getAlsaSeq().getClientId(), nDestPort);
			portSubscribe.setQueue(nTimestampingQueue);
			portSubscribe.setExclusive(false);
			portSubscribe.setTimeUpdate(true);
			portSubscribe.setTimeReal(bRealtime);
			getAlsaSeq().subscribePort(portSubscribe);
			portSubscribe.free();
		}
		else
		{
			AlsaSeqPortSubscribe	portSubscribe = new AlsaSeqPortSubscribe();
			portSubscribe.setSender(nSourceClient, nSourcePort);
			portSubscribe.setDest(getAlsaSeq().getClientId(), nDestPort);
			getAlsaSeq().subscribePort(portSubscribe);
			portSubscribe.free();
		}
		setDaemon(true);
	}



	private AlsaSeq getAlsaSeq()
	{
		return m_alsaSeq;
	}



	/**	The working part of the class.
		Here, the thread repeats in blocking in a call to
		getEvent() and calling the listener's
		dequeueEvent() method.
	*/
	public void run()
	{
		// TODO: recheck interupt mechanism
		while (!interrupted())
		{
			MidiEvent	event = getEvent();
			if (TDebug.TraceAlsaMidiIn) { TDebug.out("AlsaMidiIn.run(): got event: " + event); }
			if (event != null)
			{
				MidiMessage	message = event.getMessage();
				long		lTimestamp = event.getTick();
				if (message instanceof MetaMessage)
				{
					MetaMessage	me = (MetaMessage) message;
					if (TDebug.TraceAlsaMidiIn) { TDebug.out("AlsaMidiIn.run(): MetaMessage.getData().length: " + me.getData().length); }
				}
				m_listener.dequeueEvent(message, lTimestamp);
			}
			else
			{
				if (TDebug.TraceAlsaMidiIn || TDebug.TraceAllWarnings) { TDebug.out("AlsaMidiIn.run(): received null from getEvent()"); }
			}
		}
	}



	private MidiEvent getEvent()
	{
		if (TDebug.TraceAlsaMidiIn) { TDebug.out("AlsaMidiIn.getEvent(): before eventInput()"); }
		while (true)
		{
			int	nReturn = getAlsaSeq().eventInput(m_event);
			if (nReturn >= 0)
			{
				break;
			}

			/*
			 *	Sleep for 1 ms to enable scheduling.
			 */
			if (TDebug.TraceAlsaMidiIn || TDebug.TraceAllWarnings) { TDebug.out("AlsaMidiIn.getEvent(): sleeping because got no event"); }
			try
			{
				Thread.sleep(1);
			}
			catch (InterruptedException e)
			{
				if (TDebug.TraceAllExceptions) { TDebug.out(e); }
			}
		}
		MidiMessage	message = null;
		int	nType = m_event.getType();
		switch (nType)
		{
		case AlsaSeq.SND_SEQ_EVENT_NOTEON:
		case AlsaSeq.SND_SEQ_EVENT_NOTEOFF:
		case AlsaSeq.SND_SEQ_EVENT_KEYPRESS:
		{
			if (TDebug.TraceAlsaMidiIn) { TDebug.out("AlsaMidiIn.getEvent(): note/aftertouch event"); }
			m_event.getNote(m_anValues);
			ShortMessage	shortMessage = new ShortMessage();
			int	nCommand = -1;
			switch (nType)
			{
			case AlsaSeq.SND_SEQ_EVENT_NOTEON:
				nCommand = ShortMessage.NOTE_ON;
				break;

			case AlsaSeq.SND_SEQ_EVENT_NOTEOFF:
				nCommand = ShortMessage.NOTE_OFF;
				break;

			case AlsaSeq.SND_SEQ_EVENT_KEYPRESS:
				nCommand = ShortMessage.POLY_PRESSURE;
				break;
			}
			int	nChannel = m_anValues[0] & 0xF;
			int	nKey = m_anValues[1] & 0x7F;
			int	nVelocity = m_anValues[2] & 0x7F;
			try
			{
				shortMessage.setMessage(nCommand, nChannel, nKey, nVelocity);
			}
			catch (InvalidMidiDataException e)
			{
				if (TDebug.TraceAlsaMidiIn || TDebug.TraceAllExceptions) { TDebug.out(e); }
			}
			message = shortMessage;
			break;
		}

		// all event types that use snd_seq_ev_ctrl_t
		// TODO: more
		case AlsaSeq.SND_SEQ_EVENT_CONTROLLER:
		case AlsaSeq.SND_SEQ_EVENT_PGMCHANGE:
		case AlsaSeq.SND_SEQ_EVENT_CHANPRESS:
		case AlsaSeq.SND_SEQ_EVENT_PITCHBEND:
		case AlsaSeq.SND_SEQ_EVENT_QFRAME:
		case AlsaSeq.SND_SEQ_EVENT_SONGPOS:
		case AlsaSeq.SND_SEQ_EVENT_SONGSEL:
		{
			m_event.getControl(m_anValues);
			int	nCommand = -1;
			int	nChannel = m_anValues[0] & 0xF;
			int	nData1 = -1;
			int	nData2 = -1;
			switch (nType)
			{
			case AlsaSeq.SND_SEQ_EVENT_CONTROLLER:
				if (TDebug.TraceAlsaMidiIn) { TDebug.out("AlsaMidiIn.getEvent(): controller event"); }
				nCommand = ShortMessage.CONTROL_CHANGE;
				nData1 = m_anValues[1] & 0x7F;
				nData2 = m_anValues[2] & 0x7F;
				break;

			case AlsaSeq.SND_SEQ_EVENT_PGMCHANGE:
				if (TDebug.TraceAlsaMidiIn) { TDebug.out("AlsaMidiIn.getEvent(): program change event"); }
				nCommand = ShortMessage.PROGRAM_CHANGE;
				nData1 = m_anValues[2] & 0x7F;
				nData2 = 0;
				break;

			case AlsaSeq.SND_SEQ_EVENT_CHANPRESS:
				if (TDebug.TraceAlsaMidiIn) { TDebug.out("AlsaMidiIn.getEvent(): channel pressure event"); }
				nCommand = ShortMessage.CHANNEL_PRESSURE;
				nData1 = m_anValues[2] & 0x7F;
				nData2 = 0;
				break;

			case AlsaSeq.SND_SEQ_EVENT_PITCHBEND:
				if (TDebug.TraceAlsaMidiIn) { TDebug.out("AlsaMidiIn.getEvent(): pitchbend event"); }
				nCommand = ShortMessage.PITCH_BEND;
				nData1 = m_anValues[2] & 0x7F;
				nData2 = (m_anValues[2] >> 7) & 0x7F;
				break;

			case AlsaSeq.SND_SEQ_EVENT_QFRAME:
				if (TDebug.TraceAlsaMidiIn) { TDebug.out("AlsaMidiIn.getEvent(): MTC event"); }
				nCommand = ShortMessage.MIDI_TIME_CODE;
				nData1 = m_anValues[2] & 0x7F;
				nData2 = 0;
				break;

			case AlsaSeq.SND_SEQ_EVENT_SONGPOS:
				if (TDebug.TraceAlsaMidiIn) { TDebug.out("AlsaMidiIn.getEvent(): song position event"); }
				nCommand = ShortMessage.SONG_POSITION_POINTER;
				nData1 = m_anValues[2] & 0x7F;
				nData2 = (m_anValues[2] >> 7) & 0x7F;
				break;

			case AlsaSeq.SND_SEQ_EVENT_SONGSEL:
				if (TDebug.TraceAlsaMidiIn) { TDebug.out("AlsaMidiIn.getEvent(): song select event"); }
				nCommand = ShortMessage.SONG_SELECT;
				nData1 = m_anValues[2] & 0x7F;
				nData2 = 0;
				break;
			}
			ShortMessage	shortMessage = new ShortMessage();
			try
			{
				shortMessage.setMessage(nCommand, nChannel, nData1, nData2);
			}
			catch (InvalidMidiDataException e)
			{
				if (TDebug.TraceAlsaMidiIn || TDebug.TraceAllExceptions) { TDebug.out(e); }
			}
			message = shortMessage;
		}
		break;

		// status-only events
		case AlsaSeq.SND_SEQ_EVENT_TUNE_REQUEST:
		case AlsaSeq.SND_SEQ_EVENT_CLOCK:
		case AlsaSeq.SND_SEQ_EVENT_START:
		case AlsaSeq.SND_SEQ_EVENT_CONTINUE:
		case AlsaSeq.SND_SEQ_EVENT_STOP:
		case AlsaSeq.SND_SEQ_EVENT_SENSING:
		case AlsaSeq.SND_SEQ_EVENT_RESET:
		{
			int	nStatus = -1;
			switch (nType)
			{
			case AlsaSeq.SND_SEQ_EVENT_TUNE_REQUEST:
				nStatus = ShortMessage.TUNE_REQUEST;
				break;

			case AlsaSeq.SND_SEQ_EVENT_CLOCK:
				if (TDebug.TraceAlsaMidiIn) { TDebug.out("AlsaMidiIn.getEvent(): clock event"); }
				nStatus = ShortMessage.TIMING_CLOCK;
				break;

			case AlsaSeq.SND_SEQ_EVENT_START:
				nStatus = ShortMessage.START;
				break;

			case AlsaSeq.SND_SEQ_EVENT_CONTINUE:
				nStatus = ShortMessage.CONTINUE;
				break;

			case AlsaSeq.SND_SEQ_EVENT_STOP:
				nStatus = ShortMessage.STOP;
				break;

			case AlsaSeq.SND_SEQ_EVENT_SENSING:
				nStatus = ShortMessage.ACTIVE_SENSING;
				break;

			case AlsaSeq.SND_SEQ_EVENT_RESET:
				nStatus = ShortMessage.SYSTEM_RESET;
				break;
			}
			ShortMessage	shortMessage = new ShortMessage();
			try
			{
				shortMessage.setMessage(nStatus);
			}
			catch (InvalidMidiDataException e)
			{
				if (TDebug.TraceAlsaMidiIn || TDebug.TraceAllExceptions) { TDebug.out(e); }
			}
			message = shortMessage;
			break;
		}

		case AlsaSeq.SND_SEQ_EVENT_USR_VAR4:
		{
			if (TDebug.TraceAlsaMidiIn) { TDebug.out("AlsaMidiIn.getEvent(): meta event"); }
			MetaMessage	metaMessage = new MetaMessage();
			byte[]	abTransferData = m_event.getVar();
			int	nMetaType = abTransferData[0];
			byte[]	abData = new byte[abTransferData.length - 1];
			System.arraycopy(abTransferData, 1, abData, 0, abTransferData.length - 1);
			try
			{
				metaMessage.setMessage(nMetaType, abData, abData.length);
			}
			catch (InvalidMidiDataException e)
			{
				if (TDebug.TraceAlsaMidiIn || TDebug.TraceAllExceptions) { TDebug.out(e); }
			}
			message = metaMessage;
			break;
		}

		case AlsaSeq.SND_SEQ_EVENT_SYSEX:
		{
			if (TDebug.TraceAlsaMidiIn) { TDebug.out("AlsaMidiIn.getEvent(): sysex event"); }
			SysexMessage	sysexMessage = new SysexMessage();
			byte[]	abData = m_event.getVar();
			try
			{
				sysexMessage.setMessage(abData, abData.length);
			}
			catch (InvalidMidiDataException e)
			{
				if (TDebug.TraceAlsaMidiIn || TDebug.TraceAllExceptions) { TDebug.out(e); }
			}
			message = sysexMessage;
			break;
		}

		default:
			if (TDebug.TraceAlsaMidiIn || TDebug.TraceAllWarnings) { TDebug.out("AlsaMidiIn.getEvent(): unknown event"); }

		}
		if (message != null)
		{
			/*
			  If the timestamp is in ticks, ticks in the MidiEvent
			  gets this value.
			  Otherwise, if the timestamp is in realtime (ns),
			  we put us in the tick value.
			*/
			long	lTimestamp = m_event.getTimestamp();
			if ((m_event.getFlags() & AlsaSeq.SND_SEQ_TIME_STAMP_MASK) == AlsaSeq.SND_SEQ_TIME_STAMP_REAL)
			{
				// ns -> micros
				lTimestamp /= 1000;
			}
			MidiEvent	event = new MidiEvent(message, lTimestamp);
			return event;
		}
		else
		{
			return null;
		}
	}



	/**
	 */
	public static interface AlsaMidiInListener
	{
		public void dequeueEvent(MidiMessage message, long lTimestamp);
	}
}



/*** AlsaMidiIn.java ***/
