/*
 *	AlsaSequencer.java
 */

/*
 *  Copyright (c) 1999 - 2003 by Matthias Pfisterer
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


package	org.tritonus.midi.device.alsa;


import	java.util.Arrays;

import	javax.sound.midi.InvalidMidiDataException;
import	javax.sound.midi.MetaMessage;
import	javax.sound.midi.MidiDevice;
import	javax.sound.midi.MidiEvent;
import	javax.sound.midi.MidiMessage;
import	javax.sound.midi.MidiUnavailableException;
import	javax.sound.midi.Receiver;
import	javax.sound.midi.ShortMessage;
import	javax.sound.midi.Sequence;
import	javax.sound.midi.Sequencer;
import	javax.sound.midi.Track;
import	javax.sound.midi.Transmitter;

import	org.tritonus.lowlevel.alsa.AlsaSeq;
import	org.tritonus.lowlevel.alsa.AlsaSeqPortSubscribe;
import	org.tritonus.lowlevel.alsa.AlsaSeqQueueInfo;
import	org.tritonus.lowlevel.alsa.AlsaSeqEvent;
import	org.tritonus.lowlevel.alsa.AlsaSeqQueueStatus;
import	org.tritonus.lowlevel.alsa.AlsaSeqQueueTempo;
import	org.tritonus.share.TDebug;
import	org.tritonus.share.midi.MidiUtils;
import	org.tritonus.share.midi.TMidiDevice.TReceiver;
import	org.tritonus.share.midi.TMidiDevice.TTransmitter;
import	org.tritonus.share.midi.TSequencer;



public class AlsaSequencer
// TODO: derive from TPreloadingSequencer
extends TSequencer
{
	/**	The syncronization modes the sequencer can sync to.
	 */
	private static final SyncMode[]	MASTER_SYNC_MODES = {SyncMode.INTERNAL_CLOCK};

	/**	The syncronization modes the sequencer can send.
	 */
	private static final SyncMode[]	SLAVE_SYNC_MODES = {SyncMode.NO_SYNC, SyncMode.MIDI_SYNC};

	/**	The ALSA event tag used for MIDI clock events
	 */
	private static final int	CLOCK_EVENT_TAG = 255;


	private AlsaSeq				m_playbackAlsaSeq;
	private AlsaSeq				m_recordingAlsaSeq;
	private int					m_nRecordingPort;
	private int					m_nPlaybackPort;
	private int					m_nQueue;
	private AlsaSeqQueueInfo	m_queueInfo;
	private AlsaSeqQueueStatus	m_queueStatus;
	private AlsaSeqQueueTempo	m_queueTempo;
	private AlsaMidiIn			m_playbackAlsaMidiIn;
	private AlsaMidiOut			m_playbackAlsaMidiOut;
	private AlsaMidiIn			m_recordingAlsaMidiIn;
	private Thread				m_loaderThread;
	private Thread				m_syncThread;
	private AlsaSeqEvent		m_queueControlEvent;
	private AlsaSeqEvent		m_clockEvent;
	private boolean				m_bRecording;
	private Track				m_track;
	private AlsaSeqEvent		m_allNotesOffEvent;
	private Sequencer.SyncMode	m_oldSlaveSyncMode;
	private float				m_fCachedRealMPQ;


	public AlsaSequencer(MidiDevice.Info info)
	{
		super(info,
		      Arrays.asList(MASTER_SYNC_MODES),
		      Arrays.asList(SLAVE_SYNC_MODES));
		if (TDebug.TraceSequencer) { TDebug.out("AlsaSequencer.<init>(): begin"); }
		m_fCachedRealMPQ = -1.0F;
		if (TDebug.TraceSequencer) { TDebug.out("AlsaSequencer.<init>(): end"); }
	}



	private int getPlaybackClient()
	{
		int	nClient = getPlaybackAlsaSeq().getClientId();
		return nClient;
	}



	private int getPlaybackPort()
	{
		return m_nPlaybackPort;
	}



	private int getRecordingClient()
	{
		int	nClient = getRecordingAlsaSeq().getClientId();
		return nClient;
	}



	private int getRecordingPort()
	{
		return m_nRecordingPort;
	}



	private int getQueue()
	{
		return m_nQueue;
	}



	private AlsaSeqQueueStatus getQueueStatus()
	{
		return m_queueStatus;
	}



	private AlsaSeqQueueTempo getQueueTempo()
	{
		return m_queueTempo;
	}



	private AlsaSeq getPlaybackAlsaSeq()
	{
		return m_playbackAlsaSeq;
	}



	private AlsaSeq getRecordingAlsaSeq()
	{
		return m_recordingAlsaSeq;
	}



	private void updateQueueStatus()
	{
		if (TDebug.TraceSequencer) { TDebug.out("AlsaSequencer.updateQueueStatus(): begin"); }
		// TODO: error handling
		// getRecordingAlsaSeq().getQueueStatus(getQueue(), getQueueStatus());
		getPlaybackAlsaSeq().getQueueStatus(getQueue(), getQueueStatus());
		if (TDebug.TraceSequencer) { TDebug.out("AlsaSequencer.updateQueueStatus(): end"); }
	}



	protected void openImpl()
	{
		if (TDebug.TraceSequencer) { TDebug.out("AlsaSequencer.openImpl(): begin"); }
		m_recordingAlsaSeq = new AlsaSeq("Tritonus ALSA Sequencer (recording/synchronization)");
		m_nRecordingPort = getRecordingAlsaSeq().createPort("recording/synchronization port", AlsaSeq.SND_SEQ_PORT_CAP_WRITE | AlsaSeq.SND_SEQ_PORT_CAP_SUBS_WRITE | AlsaSeq.SND_SEQ_PORT_CAP_READ | AlsaSeq.SND_SEQ_PORT_CAP_SUBS_READ, 0, AlsaSeq.SND_SEQ_PORT_TYPE_APPLICATION, 0, 0, 0);

		m_playbackAlsaSeq = new AlsaSeq("Tritonus ALSA Sequencer (playback)");
		m_nPlaybackPort = getPlaybackAlsaSeq().createPort("playback port", AlsaSeq.SND_SEQ_PORT_CAP_WRITE | AlsaSeq.SND_SEQ_PORT_CAP_SUBS_WRITE | AlsaSeq.SND_SEQ_PORT_CAP_READ | AlsaSeq.SND_SEQ_PORT_CAP_SUBS_READ, 0, AlsaSeq.SND_SEQ_PORT_TYPE_APPLICATION, 0, 0, 0);

		m_nQueue = getPlaybackAlsaSeq().allocQueue();
		m_queueInfo = new AlsaSeqQueueInfo();
		m_queueStatus = new AlsaSeqQueueStatus();
		m_queueTempo = new AlsaSeqQueueTempo();
		getPlaybackAlsaSeq().getQueueInfo(getQueue(), m_queueInfo);
		m_queueInfo.setLocked(false);
		getPlaybackAlsaSeq().setQueueInfo(getQueue(), m_queueInfo);
		m_playbackAlsaMidiOut = new AlsaMidiOut(getPlaybackAlsaSeq(), getPlaybackPort(), getQueue());
		m_playbackAlsaMidiOut.setHandleMetaMessages(true);
		getRecordingAlsaSeq().setQueueUsage(getQueue(), true);

		// this establishes the subscription, too
		AlsaMidiIn.AlsaMidiInListener	playbackListener = new PlaybackAlsaMidiInListener();
		m_playbackAlsaMidiIn = new AlsaMidiIn(getPlaybackAlsaSeq(), getPlaybackPort(), getPlaybackClient(), getPlaybackPort(), playbackListener);
		// start the receiving thread
		m_playbackAlsaMidiIn.start();
		if (TDebug.TraceSequencer) { TDebug.out("AlsaSequencer.openImpl(): end"); }
		m_queueControlEvent = new AlsaSeqEvent();
		m_clockEvent = new AlsaSeqEvent();
		m_clockEvent.setCommon(
			AlsaSeq.SND_SEQ_EVENT_CLOCK,	// type
			AlsaSeq.SND_SEQ_TIME_STAMP_TICK | AlsaSeq.SND_SEQ_TIME_MODE_ABS,
			CLOCK_EVENT_TAG,	// tag
			getQueue(),
			0L,	// timestamp; not yet known
			0,				// source client
			getRecordingPort(),		// source port
			AlsaSeq.SND_SEQ_ADDRESS_SUBSCRIBERS,	// dest client
			AlsaSeq.SND_SEQ_ADDRESS_UNKNOWN);	// dest port
		m_allNotesOffEvent = new AlsaSeqEvent();
		m_oldSlaveSyncMode = getSlaveSyncMode();
		if (m_fCachedRealMPQ != -1.0F)
		{
			setTempoImpl(m_fCachedRealMPQ);
			m_fCachedRealMPQ = -1.0F;
		}
		m_loaderThread = new LoaderThread();
		m_loaderThread.start();
		// this is for sending clock events
		// m_syncThread = new MasterSynchronizer();
		// m_syncThread.start();
	}



	protected void closeImpl()
	{
		if (TDebug.TraceSequencer) { TDebug.out("AlsaSequencer.closeImpl(): begin"); }
		m_playbackAlsaMidiIn.interrupt();
		m_playbackAlsaMidiIn = null;
		getQueueStatus().free();
		m_queueStatus = null;
		getQueueTempo().free();
		m_queueTempo = null;
		// TODO:
		// m_aSequencer.releaseQueue(getQueue());
		// m_aSequencer.destroyPort(getPort());
		getRecordingAlsaSeq().close();
		m_recordingAlsaSeq = null;
		getPlaybackAlsaSeq().close();
		m_playbackAlsaSeq = null;
		m_queueControlEvent.free();
		m_queueControlEvent = null;
		m_clockEvent.free();
		m_clockEvent = null;
		m_allNotesOffEvent.free();
		m_allNotesOffEvent = null;
		if (TDebug.TraceSequencer) { TDebug.out("AlsaSequencer.closeImpl(): end"); }
	}



	protected void startImpl()
	{
		if (TDebug.TraceSequencer) { TDebug.out("AlsaSequencer.startImpl(): begin"); }
		if (getTickPosition() == 0)
		{
			startQueue();
		}
		else
		{
			continueQueue();
		}
		synchronized (m_loaderThread)
		{
			if (TDebug.TraceSequencer) { TDebug.out("AlsaSequencer.startImpl(): notifying loader thread"); }
			m_loaderThread.notify();
		}
		// TODO: should depend on sync mode
// 		synchronized (m_syncThread)
// 		{
// 			if (TDebug.TraceSequencer) { TDebug.out("AlsaSequencer.startImpl(): notifying synchronizer thread"); }
// 			m_syncThread.notify();
// 		}
		if (! getSlaveSyncMode(). equals(Sequencer.SyncMode.NO_SYNC))
		{
			sendStartEvent();
		}
		if (TDebug.TraceSequencer) { TDebug.out("AlsaSequencer.startImpl(): end"); }
	}



	protected void stopImpl()
	{
		if (TDebug.TraceSequencer) { TDebug.out("AlsaSequencer.stopImpl(): begin"); }
		stopQueue();
		sendAllNotesOff();
		// should be in base class?
		stopRecording();
		if (! getSlaveSyncMode(). equals(Sequencer.SyncMode.NO_SYNC))
		{
			sendStopEvent();
		}
		if (TDebug.TraceSequencer) { TDebug.out("AlsaSequencer.stopImpl(): end"); }
	}



	public boolean isRunning()
	{
		if (TDebug.TraceSequencer) { TDebug.out("AlsaSequencer.isRunning(): begin"); }
		boolean bRunning = false;
		if (isOpen())
		{
			updateQueueStatus();
			int	nStatus = getQueueStatus().getStatus();
			if (TDebug.TraceSequencer) { TDebug.out("AlsaSequencer.isRunning(): queue status: " + nStatus); }
			bRunning = (nStatus != 0);
		}
		if (TDebug.TraceSequencer) { TDebug.out("AlsaSequencer.isRunning(): end"); }
		return bRunning;
	}



	public void startRecording()
	{
		checkOpen(); // may throw IllegalStateException
		m_bRecording = true;
		start();
	}



	public void stopRecording()
	{
		checkOpen(); // may throw IllegalStateException
		m_bRecording = false;
	}



	public boolean isRecording()
	{
		return m_bRecording;
	}



	// name should be: enableRecording
	public void recordEnable(Track track, int nChannel)
	{
		// TODO: hacky
		m_track = track;
	}



	// name should be: disableRecording
	public void recordDisable(Track track)
	{
		// TODO:
	}



	protected void setTempoImpl(float fRealMPQ)
	{
		if (TDebug.TraceSequencer) { TDebug.out("AlsaSequencer.setTempoImpl(): begin"); }
		if (isOpen())
		{
			if (TDebug.TraceSequencer) { TDebug.out("AlsaSequencer.setTempoImpl(): setting tempo to " + (int) fRealMPQ); }
			getQueueTempo().setTempo((int) fRealMPQ);
			getQueueTempo().setPpq(getResolution());
			getPlaybackAlsaSeq().setQueueTempo(getQueue(), getQueueTempo());
		}
		else
		{
			if (TDebug.TraceSequencer) { TDebug.out("AlsaSequencer.setTempoImpl(): ignoring because sequencer is not opened"); }
			m_fCachedRealMPQ = fRealMPQ;
		}
		if (TDebug.TraceSequencer) { TDebug.out("AlsaSequencer.setTempoImpl(): end"); }
	}



	public long getTickPosition()
	{
		if (TDebug.TraceSequencer) { TDebug.out("AlsaSequencer.getTickPosition(): begin"); }
		long	lPosition;
		if (isOpen())
		{
			updateQueueStatus();
			lPosition = getQueueStatus().getTickTime();
		}
		else
		{
			if (TDebug.TraceSequencer) { TDebug.out("AlsaSequencer.getTickPosition(): sequencer not open, returning 0"); }
			lPosition = 0;
		}
		if (TDebug.TraceSequencer) { TDebug.out("AlsaSequencer.getTickPosition(): end"); }
		return lPosition;
	}



	public void setTickPosition(long lTick)
	{
		if (TDebug.TraceSequencer) { TDebug.out("AlsaSequencer.setTickPosition(): begin"); }
		if (isOpen())
		{
			int	nSourcePort = getRecordingPort();
			int	nQueue = getQueue();
			long	lTime = lTick;
			sendQueueControlEvent(
				AlsaSeq.SND_SEQ_EVENT_SETPOS_TICK,
				AlsaSeq.SND_SEQ_TIME_STAMP_REAL | AlsaSeq.SND_SEQ_TIME_MODE_REL, 0, AlsaSeq.SND_SEQ_QUEUE_DIRECT, 0L,
				nSourcePort, AlsaSeq.SND_SEQ_CLIENT_SYSTEM, AlsaSeq.SND_SEQ_PORT_SYSTEM_TIMER,
				nQueue, 0, lTime);
		}
		else
		{
			if (TDebug.TraceSequencer) { TDebug.out("AlsaSequencer.setTickPosition(): ignored because sequencer is not open"); }
		}
		if (TDebug.TraceSequencer) { TDebug.out("AlsaSequencer.setTickPosition(): end"); }
	}



	public long getMicrosecondPosition()
	{
		if (TDebug.TraceSequencer) { TDebug.out("AlsaSequencer.getMicrosecondPosition(): begin"); }
		long	lPosition;
		if (isOpen())
		{
			updateQueueStatus();
			long	lNanoSeconds = getQueueStatus().getRealTime();
			lPosition = lNanoSeconds / 1000;
		}
		else
		{
			if (TDebug.TraceSequencer) { TDebug.out("AlsaSequencer.getMicrosecondPosition(): sequencer not open, returning 0"); }
			lPosition = 0;
		}
		if (TDebug.TraceSequencer) { TDebug.out("AlsaSequencer.getMicrosecondPosition(): end"); }
		return lPosition;
	}



	public void setMicrosecondPosition(long lMicroseconds)
	{
		if (TDebug.TraceSequencer) { TDebug.out("AlsaSequencer.setMicrosecondPosition(): begin"); }
		if (isOpen())
		{
			long	lNanoSeconds = lMicroseconds * 1000;
			int	nSourcePort = getRecordingPort();
			int	nQueue = getQueue();
			long	lTime = lNanoSeconds;
			sendQueueControlEvent(
				AlsaSeq.SND_SEQ_EVENT_SETPOS_TIME,
				AlsaSeq.SND_SEQ_TIME_STAMP_REAL | AlsaSeq.SND_SEQ_TIME_MODE_REL, 0, AlsaSeq.SND_SEQ_QUEUE_DIRECT, 0L,
				nSourcePort, AlsaSeq.SND_SEQ_CLIENT_SYSTEM, AlsaSeq.SND_SEQ_PORT_SYSTEM_TIMER,
				nQueue, 0, lTime);
		}
		else
		{
			if (TDebug.TraceSequencer) { TDebug.out("AlsaSequencer.setMicrosecondPosition(): ignoring because sequencer is not open"); }
		}
		if (TDebug.TraceSequencer) { TDebug.out("AlsaSequencer.setMicrosecondPosition(): end"); }
	}





	protected void setMasterSyncModeImpl(SyncMode syncMode)
	{
		// TODO:
	}



	protected void setSlaveSyncModeImpl(SyncMode syncMode)
	{
		if (isRunning())
		{
			if (m_oldSlaveSyncMode.equals(Sequencer.SyncMode.NO_SYNC) && (syncMode.equals(Sequencer.SyncMode.MIDI_SYNC) || syncMode.equals(Sequencer.SyncMode.MIDI_TIME_CODE)) )
			{
				sendStartEvent();
				// TODO: notify sync thread 
			}
			else if ((m_oldSlaveSyncMode.equals(Sequencer.SyncMode.MIDI_SYNC) || m_oldSlaveSyncMode.equals(Sequencer.SyncMode.MIDI_TIME_CODE)) && syncMode.equals(Sequencer.SyncMode.NO_SYNC) )
			{
				sendStopEvent();
				// TODO: remove enqueued messages from queue (and buffer). perhaps do this by putting the code to do so after the main loop of the sync thread.
			}
		}
	}



	protected void setTrackEnabledImpl(int nTrack, boolean bEnabled)
	{
		/**	If the enabled state changes to true, the events
			between	the current playback position and the current
			loading position are enqueued.
			If the enabled state of a track changed to false,
			the events belonging to this track are removed from
			the queue, besides the 'off'-events.
		*/
		if (bEnabled)
		{
			// TODO: reload events
		}
		else
		{
			// TODO: remove events
		}
	}



	/*
	  This method has to be synchronized because it is called
	  from sendMessageTick() as well as from loadSequenceToNative().
	*/
	private synchronized void enqueueMessage(MidiMessage message, long lTick)
	{
		m_playbackAlsaMidiOut.enqueueMessage(message, lTick);
	}



	/**	Put a message into the queue.
		This is Claus-Dieter's special method: it puts the message to
		the ALSA queue for delivery at the specified time.
		The time has to be given in ticks according to the resolution
		of the currently active Sequence. For this method to work,
		the Sequencer has to be started. The message is delivered
		the same way as messages from a Sequence, i.e. to all
		registered Transmitters. If the current queue position (as
		returned by getTickPosition()) is
		already behind the desired schedule time, the message is
		ignored.

		@param message the MidiMessage to put into the queue.

		@param lTick the desired schedule time in ticks.
	*/
	public void sendMessageTick(MidiMessage message, long lTick)
	{
		enqueueMessage(message, lTick);
	}



	private void startQueue()
	{
		controlQueue(AlsaSeq.SND_SEQ_EVENT_START);
	}



	private void continueQueue()
	{
		controlQueue(AlsaSeq.SND_SEQ_EVENT_CONTINUE);
	}



	private void stopQueue()
	{
		controlQueue(AlsaSeq.SND_SEQ_EVENT_STOP);
	}



	private void controlQueue(int nType)
	{
		int	nSourcePort = getPlaybackPort();
		int	nQueue = getQueue();
		sendQueueControlEvent(
			nType,
			AlsaSeq.SND_SEQ_TIME_STAMP_REAL | AlsaSeq.SND_SEQ_TIME_MODE_REL,
			0,
			AlsaSeq.SND_SEQ_QUEUE_DIRECT,
			0L,
			nSourcePort,
			AlsaSeq.SND_SEQ_CLIENT_SYSTEM,
			AlsaSeq.SND_SEQ_PORT_SYSTEM_TIMER,
			nQueue, 0, 0);
	}



	/**	Send a real time START  event to the subscribers immediately.
	 */
	private void sendStartEvent()
	{
		sendRealtimeEvent(AlsaSeq.SND_SEQ_EVENT_START);
	}



	/**	Send a real time STOP  event to the subscribers immediately.
	 */
	private void sendStopEvent()
	{
		sendRealtimeEvent(AlsaSeq.SND_SEQ_EVENT_STOP);
	}



	private void sendRealtimeEvent(int nType)
	{
		sendQueueControlEvent(
			nType,
			AlsaSeq.SND_SEQ_TIME_STAMP_REAL | AlsaSeq.SND_SEQ_TIME_MODE_REL,
			0,				// tag
			AlsaSeq.SND_SEQ_QUEUE_DIRECT,	// queue
			0L,				// time
			getPlaybackPort(),			// source
			AlsaSeq.SND_SEQ_ADDRESS_SUBSCRIBERS,	// dest client
			AlsaSeq.SND_SEQ_ADDRESS_UNKNOWN,	// dest port
			0, 0, 0);
	}



	// NOTE: also used for setting position and start/stop RT
	private void sendQueueControlEvent(
		int nType, int nFlags, int nTag, int nQueue, long lTime,
		int nSourcePort, int nDestClient, int nDestPort,
		int nControlQueue, int nControlValue, long lControlTime)
	{
		m_queueControlEvent.setCommon(nType, nFlags, nTag, nQueue, lTime,
					      0, nSourcePort, nDestClient, nDestPort);
		m_queueControlEvent.setQueueControl(nControlQueue, nControlValue, lControlTime);
		getPlaybackAlsaSeq().eventOutputDirect(m_queueControlEvent);
	}



	private void sendAllNotesOffEvent(int nChannel)
	{
		int	nSourcePort = getPlaybackPort();
		m_allNotesOffEvent.setCommon(
			AlsaSeq.SND_SEQ_EVENT_CONTROLLER,
			AlsaSeq.SND_SEQ_TIME_STAMP_REAL | AlsaSeq.SND_SEQ_TIME_MODE_REL,
			0,				// tag
			AlsaSeq.SND_SEQ_QUEUE_DIRECT,	// queue
			0L,				// time
			0, nSourcePort,			// source
			AlsaSeq.SND_SEQ_ADDRESS_SUBSCRIBERS,	// dest client
			AlsaSeq.SND_SEQ_ADDRESS_UNKNOWN);	// dest port
		m_allNotesOffEvent.setControl(nChannel, 0x78, 0);
		getPlaybackAlsaSeq().eventOutputDirect(m_allNotesOffEvent);
	}



	private void sendAllNotesOff()
	{
		// TODO: check if [0..15] or [1..16]
		for (int nChannel = 0; nChannel < 16; nChannel++)
		{
			sendAllNotesOffEvent(nChannel);
		}
	}



	/**	Receive a correctely timestamped event.
		This method expects that the timestamp is in ticks,
		appropriate for the Sequence currently running.
	*/
	private void receiveTimestamped(MidiMessage message, long lTimestamp)
	{
		if (isRecording())
		{
			// TODO: this is hacky; should implement correct track mapping
			Track	track = m_track;
			MidiEvent	event = new MidiEvent(message, lTimestamp);
			track.add(event);
		}
		// TODO: entering an event into the sequence
	}



	/**	Receive an event from a Receiver.
	 	This method is called by AlsaSequencer.AlsaSequencerReceiver
		on receipt of a MidiMessage.
	*/
	protected void receive(MidiMessage message, long lTimestamp)
	{
		lTimestamp = getTickPosition();
		receiveTimestamped(message, lTimestamp);
	}



	///////////////////////////////////////////////////


	public Receiver getReceiver()
		throws	MidiUnavailableException
	{
		return new AlsaSequencerReceiver();
	}



	public Transmitter getTransmitter()
		throws	MidiUnavailableException
	{
		return new AlsaSequencerTransmitter();
	}


/////////////////// INNER CLASSES //////////////////////////////////////



	private class PlaybackAlsaMidiInListener
		implements AlsaMidiIn.AlsaMidiInListener
	{
		public void dequeueEvent(MidiMessage message, long lTimestamp)
		{
			if (TDebug.TraceSequencer) { TDebug.out("AlsaSequencer.PlaybackAlsaMidiInListener.dequeueEvent(): begin"); }
			if (TDebug.TraceSequencer) { TDebug.out("AlsaSequencer.PlaybackAlsaMidiInListener.dequeueEvent(): message: " + message); }
			if (message instanceof MetaMessage)
			{
				MetaMessage	metaMessage = (MetaMessage) message;
				if (metaMessage.getType() == 0x51)	// set tempo
				{
					byte[]	abData = metaMessage.getData();
					int	nTempo = MidiUtils.getUnsignedInteger(abData[0]) * 65536 +
						MidiUtils.getUnsignedInteger(abData[1]) * 256 +
						MidiUtils.getUnsignedInteger(abData[2]);
					setTempoInMPQ((float) nTempo);
				}
			}
			// passes events to the receivers
			sendImpl(message, -1L);
			// calls control and meta listeners
			notifyListeners(message);
			if (TDebug.TraceSequencer) { TDebug.out("AlsaSequencer.PlaybackAlsaMidiInListener.dequeueEvent(): end"); }
		}
	}



	private class RecordingAlsaMidiInListener
		implements AlsaMidiIn.AlsaMidiInListener
	{
		public void dequeueEvent(MidiMessage message, long lTimestamp)
		{
			if (TDebug.TraceSequencer) { TDebug.out("AlsaSequencer.RecordingAlsaMidiInListener.dequeueEvent(): begin"); }
			if (TDebug.TraceSequencer) { TDebug.out("AlsaSequencer.RecordingAlsaMidiInListener.dequeueEvent(): message: " + message); }
			AlsaSequencer.this.receiveTimestamped(message, lTimestamp);
			if (TDebug.TraceSequencer) { TDebug.out("AlsaSequencer.RecordingAlsaMidiInListener.dequeueEvent(): end"); }
		}
	}




	private class AlsaSequencerReceiver
		extends		TReceiver
		implements	AlsaReceiver
	{



		public AlsaSequencerReceiver()
		{
			super();
		}



		/**	Subscribe to the passed port.
		 *	This establishes a subscription in the ALSA sequencer
		 *	so that the device this Receiver belongs to receives
		 *	event from the client:port passed as parameters.
		 *
		 *	@return true if subscription was established,
		 *		false otherwise
		 */
		public boolean subscribeTo(int nClient, int nPort)
		{
			try
			{
				AlsaSeqPortSubscribe	portSubscribe = new AlsaSeqPortSubscribe();
				portSubscribe.setSender(nClient, nPort);
				portSubscribe.setDest(AlsaSequencer.this.getRecordingClient(), AlsaSequencer.this.getRecordingPort());
				portSubscribe.setQueue(AlsaSequencer.this.getQueue());
				portSubscribe.setExclusive(false);
				portSubscribe.setTimeUpdate(true);
				portSubscribe.setTimeReal(false);
				AlsaSequencer.this.getRecordingAlsaSeq().subscribePort(portSubscribe);
				portSubscribe.free();
				return true;
			}
			catch (RuntimeException e)
			{
				if (TDebug.TraceAllExceptions) { TDebug.out(e); }
				return false;
			}
		}


	}



	private class AlsaSequencerTransmitter
		extends		TTransmitter
	{
		private boolean		m_bReceiverSubscribed;



		public AlsaSequencerTransmitter()
		{
			super();
			m_bReceiverSubscribed = false;
		}



		public void setReceiver(Receiver receiver)
		{
			super.setReceiver(receiver);
			/*
			 *	Try to establish a subscription of the Receiver
			 *	to the ALSA seqencer client of the device this
			 *	Transmitter belongs to.
			 */
			if (receiver instanceof AlsaReceiver)
			{
				// TDebug.out("AlsaSequencer.AlsaSequencerTransmitter.setReceiver(): trying to establish subscription");
				m_bReceiverSubscribed = ((AlsaReceiver) receiver).subscribeTo(getPlaybackClient(), getPlaybackPort());
				// TODO: similar subscription for the sequencer's own midi in listener!!
				// this is necessary because sync messages are sent via the recording port
				m_bReceiverSubscribed = ((AlsaReceiver) receiver).subscribeTo(getRecordingClient(), getRecordingPort());
				// TDebug.out("AlsaSequencer.AlsaSequencerTransmitter.setReceiver(): subscription established: " + m_bReceiverSubscribed);
			}
		}



		public void send(MidiMessage message, long lTimeStamp)
		{
			/*
			 *	Send message via Java methods only if no
			 *	subscription was established. If there is a
			 *	subscription, the message is routed inside of
			 *	the ALSA sequencer.
			 */
			if (! m_bReceiverSubscribed)
			{
				super.send(message, lTimeStamp);
			}
		}



		public void close()
		{
			super.close();
			// TODO: remove subscription
		}
	}




	private class LoaderThread
		extends	Thread
	{
		private long	m_lLoadingPosition;



		public void run()
		{
			if (TDebug.TraceSequencer) { TDebug.out("AlsaSequencer.run(): begin"); }
			while (isOpen())
			{
				do
				{
					synchronized (this)
					{
						try
						{
							this.wait();
						}
						catch (InterruptedException e)
						{
							if (TDebug.TraceAllExceptions) { TDebug.out(e); }
						}
					}
				}
				while (! isRunning());
				loadSequenceToNative();
			}
			if (TDebug.TraceSequencer) { TDebug.out("AlsaSequencer.run(): end"); }
		}



		private void loadSequenceToNative()
		{
			if (TDebug.TraceSequencer) { TDebug.out("AlsaSequencer.loadSequenceToNative(): begin"); }
			Sequence	sequence = getSequence();
			Track[]	aTracks = sequence.getTracks();
			int[]	anTrackPositions = new int[aTracks.length];
			for (int i = 0; i < aTracks.length; i++)
			{
				anTrackPositions[i] = 0;
			}
			// this is used to get a useful tick value for the end of track message
			m_lLoadingPosition = 0;
			while (isRunning())
			{
				boolean		bTrackPresent = false;
				long		lBestTick = Long.MAX_VALUE;
				int		nBestTrack = -1;
				for (int nTrack = 0; nTrack < aTracks.length; nTrack++)
				{
					if (anTrackPositions[nTrack] < aTracks[nTrack].size())
					{
						bTrackPresent = true;
						MidiEvent	event = aTracks[nTrack].get(anTrackPositions[nTrack]);
						long		lTick = event.getTick();
						if (lTick < lBestTick)
						{
							lBestTick = lTick;
							nBestTrack = nTrack;
						}
					}
				}
				if (!bTrackPresent)
				{
					/*	No more events; send
						end-of-track event.
					*/
					MetaMessage	metaMessage = new MetaMessage();
					try
					{
						metaMessage.setMessage(0x2F, new byte[0], 0);
					}
					catch (InvalidMidiDataException e)
					{
						if (TDebug.TraceAllExceptions) { TDebug.out(e); }
					}
					if (TDebug.TraceSequencer) { TDebug.out("AlsaSequencer.loadSequenceToNative(): sending End of Track message with tick " + (m_lLoadingPosition + 1)); }
					enqueueMessage(metaMessage, m_lLoadingPosition + 1);
					// leave the while (isRunning())-loop
					break;
				}
				/**	The normal case: deliver the event
					found to be the next.
				*/
				MidiEvent	event = aTracks[nBestTrack].get(anTrackPositions[nBestTrack]);
				anTrackPositions[nBestTrack]++;
				MidiMessage	message = event.getMessage();
				long		lTick = event.getTick();
				m_lLoadingPosition = Math.max(m_lLoadingPosition, lTick);
				if (message instanceof MetaMessage && ((MetaMessage) message).getType() == 0x2F)
				{
					if (TDebug.TraceSequencer) { TDebug.out("AlsaSequencer.loadSequenceToNative(): ignoring End of Track message with tick " + lTick); }
				}
				else
				{
					if (TDebug.TraceSequencer) { TDebug.out("AlsaSequencer.loadSequenceToNative(): enqueueing event with tick " + lTick); }
					enqueueMessage(message, lTick);
				}
			}
			if (TDebug.TraceSequencer) { TDebug.out("AlsaSequencer.loadSequenceToNative(): end"); }
		}
	}



	// TODO: start/stop; on/off
	private class MasterSynchronizer
		extends	Thread
	{
		public void run()
		{
			if (TDebug.TraceSequencer) { TDebug.out("AlsaSequencer.MasterSynchronizer.run(): begin"); }
			while (isOpen())
			{
				do
				{
					synchronized (this)
					{
						try
						{
							this.wait();
						}
						catch (InterruptedException e)
						{
							if (TDebug.TraceAllExceptions) { TDebug.out(e); }
						}
					}
				}
				while (! isRunning());
				double	dTickMin = getTickPosition();
				double	dTickMax = getSequence().getTickLength();
				double	dTickStep = getSequence().getResolution() / 24.0;
				if (TDebug.TraceSequencer) { TDebug.out("MasterSynchronizer.run(): tick step: " + dTickStep); }
				double	dTick = dTickMin;
				// TODO: ... && getS.Mode().equals(...)
				while (dTick < dTickMax && isRunning())
				{
					long	lTick = Math.round(dTick);
					if (TDebug.TraceSequencer) { TDebug.out("MasterSynchronizer.run(): sending clock event with tick " + lTick); }
					m_clockEvent.setTimestamp(lTick);
					getRecordingAlsaSeq().eventOutput(m_clockEvent);
					getRecordingAlsaSeq().drainOutput();
					dTick += dTickStep;
				}
			}
			if (TDebug.TraceSequencer) { TDebug.out("AlsaSequencer.MasterSynchronizer.run(): end"); }
		}
	}
}



/*** AlsaSequencer.java ***/
