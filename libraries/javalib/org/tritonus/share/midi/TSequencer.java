/*
 *	TSequencer.java
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

package	org.tritonus.share.midi;

import java.io.IOException;
import java.io.InputStream;
import java.util.BitSet;
import java.util.Collection;
import java.util.Iterator;
import java.util.Set;

import javax.sound.midi.ControllerEventListener;
import javax.sound.midi.InvalidMidiDataException;
import javax.sound.midi.MetaEventListener;
import javax.sound.midi.MetaMessage;
import javax.sound.midi.MidiDevice;
import javax.sound.midi.MidiMessage;
import javax.sound.midi.MidiSystem;
import javax.sound.midi.Sequence;
import javax.sound.midi.Sequencer;
import javax.sound.midi.ShortMessage;

import org.tritonus.share.ArraySet;
import org.tritonus.share.TDebug;




public abstract class TSequencer
	extends		TMidiDevice
	implements	Sequencer
{
	private static final float	MPQ_BPM_FACTOR = 6.0E7F;
	// This is for use in Collection.toArray(Object[]).
	private static final SyncMode[]	EMPTY_SYNCMODE_ARRAY = new SyncMode[0];


	private boolean		m_bRunning;


	/**	The Sequence to play or to record to.
	 */
	private Sequence	m_sequence;

	/**	The listeners that want to be notified of MetaMessages.
	 */
	private Set		m_metaListeners;

	/**	The listeners that want to be notified of control change events.
	 *	They are organized as follows: this array is indexed with
	 *	the number of the controller change events listeners are
	 *	interested in. If there is any interest, the array element
	 *	contains a reference to a Set containing the listeners.
	 *	These sets are allocated on demand.
	 */
	private Set[]		m_aControllerListeners;

	private float		m_fNominalTempoInMPQ;
	private float		m_fTempoFactor;
	private Collection	m_masterSyncModes;
	private Collection	m_slaveSyncModes;
	private SyncMode	m_masterSyncMode;
	private SyncMode	m_slaveSyncMode;
	private BitSet		m_muteBitSet;
	private BitSet		m_soloBitSet;

	/**	Contains the enabled state of the tracks.
		This BitSet holds the pre-calculated effect of mute and
		solo status.
	*/
	private BitSet		m_enabledBitSet;


	protected TSequencer(MidiDevice.Info info,
			     Collection masterSyncModes,
			     Collection slaveSyncModes)
	{
		super(info);
		if (TDebug.TraceSequencer) { TDebug.out("TSequencer.<init>(): begin"); }
		m_bRunning = false;
		m_sequence = null;
		m_metaListeners = new ArraySet();
		m_aControllerListeners = new Set[128];
		setTempoInMPQ(500000);
		setTempoFactor(1.0F);
		m_masterSyncModes = masterSyncModes;
		m_slaveSyncModes = slaveSyncModes;
		if (getMasterSyncModes().length > 0)
		{
			m_masterSyncMode = getMasterSyncModes()[0];
		}
		if (getSlaveSyncModes().length > 0)
		{
			m_slaveSyncMode = getSlaveSyncModes()[0];
		}
		m_muteBitSet = new BitSet();
		m_soloBitSet = new BitSet();
		m_enabledBitSet = new BitSet();
		updateEnabled();
		if (TDebug.TraceSequencer) { TDebug.out("TSequencer.<init>(): end"); }
	}



	public void setSequence(Sequence sequence)
		throws	InvalidMidiDataException
	{
		if (TDebug.TraceSequencer) { TDebug.out("TSequencer.setSequence(Sequence): begin"); }
		// TODO: what if playing is in progress?
		m_sequence = sequence;
		// yes, this is required by the specification
		setTempoFactor(1.0F);
		if (TDebug.TraceSequencer) { TDebug.out("TSequencer.setSequence(Sequence): end"); }
	}



	public void setSequence(InputStream inputStream)
		throws	InvalidMidiDataException, IOException
	{
		if (TDebug.TraceSequencer) { TDebug.out("TSequencer.setSequence(InputStream): begin"); }
		Sequence	sequence = MidiSystem.getSequence(inputStream);
		setSequence(sequence);
		if (TDebug.TraceSequencer) { TDebug.out("TSequencer.setSequence(InputStream): end"); }
	}



	public Sequence getSequence()
	{
		if (TDebug.TraceSequencer) { TDebug.out("TSequencer.getSequence(): begin"); }
		if (TDebug.TraceSequencer) { TDebug.out("TSequencer.getSequence(): end"); }
		return m_sequence;
	}



	public synchronized void start()
	{
		if (TDebug.TraceSequencer) { TDebug.out("TSequencer.start(): begin"); }
		checkOpen();
		if (! isRunning())
		{
			m_bRunning = true;
			// TODO: perhaps check if sequence present
			startImpl();
		}
		if (TDebug.TraceSequencer) { TDebug.out("TSequencer.start(): end"); }
	}


	/**
	 *	Subclasses have to override this method to be notified of
	 *	starting.
	 */
	protected void startImpl()
	{
		if (TDebug.TraceMidiDevice) { TDebug.out("TSequencer.startImpl(): begin"); }
		if (TDebug.TraceMidiDevice) { TDebug.out("TSequencer.startImpl(): end"); }
	}



	public synchronized void stop()
	{
		if (TDebug.TraceSequencer) { TDebug.out("TSequencer.stop(): begin"); }
		checkOpen();
		if (isRunning())
		{
			stopImpl();
			m_bRunning = false;
		}
		if (TDebug.TraceSequencer) { TDebug.out("TSequencer.stop(): end"); }
	}



	/**
	 *	Subclasses have to override this method to be notified of
	 *	stopping.
	 */
	protected void stopImpl()
	{
		if (TDebug.TraceMidiDevice) { TDebug.out("TSequencer.stopImpl(): begin"); }
		if (TDebug.TraceMidiDevice) { TDebug.out("TSequencer.stopImpl(): end"); }
	}



	public synchronized boolean isRunning()
	{
		return m_bRunning;
	}



	/** Checks if the Sequencer is open.
		This method is intended to be called by
		{@link javax.sound.midi.Sequencer#start start},
		{@link javax.sound.midi.Sequencer#stop stop},
		{@link javax.sound.midi.Sequencer#startRecording startRecording}
		and {@link javax.sound.midi.Sequencer#stop stopRecording}.

		@throws IllegalStateException if the <code>Sequencer</code> is not open
	 */
	protected void checkOpen()
	{
		if (! isOpen())
		{
			throw new IllegalStateException("Sequencer is not open");
		}
	}



	/**	Returns the resolution (ticks per quarter) of the current sequence.
		If no sequence is set, a bogus default value != 0 is returned.
	*/
	protected int getResolution()
	{
		if (TDebug.TraceSequencer) { TDebug.out("TSequencer.getResolution(): begin"); }
		Sequence	sequence = getSequence();
		int		nResolution;
		if (sequence != null)
		{
			nResolution = sequence.getResolution();
		}
		else
		{
			nResolution = 1;
		}
		if (TDebug.TraceSequencer) { TDebug.out("TSequencer.getResolution(): end"); }
		return nResolution;
	}



	protected void setRealTempo()
	{
		if (TDebug.TraceSequencer) { TDebug.out("TSequencer.setRealTempo(): begin"); }
		float	fRealTempo = getTempoInMPQ() / getTempoFactor();
		if (TDebug.TraceSequencer) { TDebug.out("TSequencer.setRealTempo(): real tempo: " + fRealTempo); }
		setTempoImpl(fRealTempo);
		if (TDebug.TraceSequencer) { TDebug.out("TSequencer.setRealTempo(): end"); }
	}



	public float getTempoInBPM()
	{
		if (TDebug.TraceSequencer) { TDebug.out("TSequencer.getTempoInBPM(): begin"); }
		float	fBPM = MPQ_BPM_FACTOR / getTempoInMPQ();
		if (TDebug.TraceSequencer) { TDebug.out("TSequencer.getTempoInBPM(): end"); }
		return fBPM;
	}



	public void setTempoInBPM(float fBPM)
	{
		if (TDebug.TraceSequencer) { TDebug.out("TSequencer.setTempoInBPM(): begin"); }
		float	fMPQ = MPQ_BPM_FACTOR / fBPM;
		setTempoInMPQ(fMPQ);
		if (TDebug.TraceSequencer) { TDebug.out("TSequencer.setTempoInBPM(): end"); }
	}



	public float getTempoInMPQ()
	{
		if (TDebug.TraceSequencer) { TDebug.out("TSequencer.getTempoInMPQ(): begin"); }
		if (TDebug.TraceSequencer) { TDebug.out("TSequencer.getTempoInMPQ(): end"); }
		return m_fNominalTempoInMPQ;
	}


	/** Sets the tempo.
		Implementation classes are required to call this method for changing
		the tempo in reaction to a tempo change event.
	 */
	public void setTempoInMPQ(float fMPQ)
	{
		if (TDebug.TraceSequencer) { TDebug.out("TSequencer.setTempoInMPQ(): begin"); }
 		m_fNominalTempoInMPQ = fMPQ;
 		setRealTempo();
		if (TDebug.TraceSequencer) { TDebug.out("TSequencer.setTempoInMPQ(): end"); }
	}



	public void setTempoFactor(float fFactor)
	{
		if (TDebug.TraceSequencer) { TDebug.out("TSequencer.setTempoFactor(): begin"); }
		m_fTempoFactor = fFactor;
		setRealTempo();
		if (TDebug.TraceSequencer) { TDebug.out("TSequencer.setTempoFactor(): end"); }
	}



	public float getTempoFactor()
	{
		if (TDebug.TraceSequencer) { TDebug.out("TSequencer.getTempoFactor(): begin"); }
		if (TDebug.TraceSequencer) { TDebug.out("TSequencer.getTempoFactor(): end"); }
		return m_fTempoFactor;
	}



	/**	Change the tempo of the native sequencer part.
	 *	This method has to be defined by subclasses according
	 *	to the native facilities they use for sequenceing.
	 *	The implementation should not take into account the
	 *	tempo factor. This is handled elsewhere.
	 */
	protected abstract void setTempoImpl(float fMPQ);



	// NOTE: has to be redefined if recording is done natively
	public long getTickLength()
	{
		if (TDebug.TraceSequencer) { TDebug.out("TSequencer.getTickLength(): begin"); }
		long	lLength = 0;
		if (getSequence() != null)
		{
			lLength = getSequence().getTickLength();
		}
		if (TDebug.TraceSequencer) { TDebug.out("TSequencer.getTickLength(): end"); }
		return lLength;
	}



	// NOTE: has to be redefined if recording is done natively
	public long getMicrosecondLength()
	{
		if (TDebug.TraceSequencer) { TDebug.out("TSequencer.getMicrosecondLength(): begin"); }
		long	lLength = 0;
		if (getSequence() != null)
		{
			lLength = getSequence().getMicrosecondLength();
		}
		if (TDebug.TraceSequencer) { TDebug.out("TSequencer.getMicrosecondLength(): end"); }
		return lLength;
	}




	public boolean addMetaEventListener(MetaEventListener listener)
	{
		synchronized (m_metaListeners)
		{
			return m_metaListeners.add(listener);
		}
	}



	public void removeMetaEventListener(MetaEventListener listener)
	{
		synchronized (m_metaListeners)
		{
			m_metaListeners.remove(listener);
		}
	}


	protected Iterator getMetaEventListeners()
	{
		synchronized (m_metaListeners)
		{
			return m_metaListeners.iterator();
		}
	}



	protected void sendMetaMessage(MetaMessage message)
	{
		Iterator	iterator = getMetaEventListeners();
		while (iterator.hasNext())
		{
			MetaEventListener	metaEventListener = (MetaEventListener) iterator.next();
			MetaMessage	copiedMessage = (MetaMessage) message.clone();
			metaEventListener.meta(copiedMessage);
		}
	}



	public int[] addControllerEventListener(ControllerEventListener listener, int[] anControllers)
	{
		synchronized (m_aControllerListeners)
		{
			if (anControllers == null)
			{
				/*
				 *	Add to all controllers. NOTE: this
				 *	is an implementation-specific
				 *	semantic!
				 */
				for (int i = 0; i < 128; i++)
				{
					addControllerListener(i, listener);
				}
			}
			else
			{
				for (int i = 0; i < anControllers.length; i++)
				{
					addControllerListener(anControllers[i], listener);
				}
			}
		}
		return getListenedControllers(listener);
	}



	private void addControllerListener(int i,
					   ControllerEventListener listener)
	{
		if (m_aControllerListeners[i] == null)
		{
			m_aControllerListeners[i] = new ArraySet();
		}
		m_aControllerListeners[i].add(listener);
	}



	public int[] removeControllerEventListener(ControllerEventListener listener, int[] anControllers)
	{
		synchronized (m_aControllerListeners)
		{
			if (anControllers == null)
			{
				/*
				 *	Remove from all controllers. Unlike
				 *	above, this is specified semantics.
				 */
				for (int i = 0; i < 128; i++)
				{
					removeControllerListener(i, listener);
				}
			}
			else
			{
				for (int i = 0; i < anControllers.length; i++)
				{
					removeControllerListener(anControllers[i], listener);
				}
			}
		}
		return getListenedControllers(listener);
	}



	private void removeControllerListener(int i,
					      ControllerEventListener listener)
	{
		if (m_aControllerListeners[i] != null)
		{
			m_aControllerListeners[i].add(listener);
		}
	}



	private int[] getListenedControllers(ControllerEventListener listener)
	{
		int[]	anControllers = new int[128];
		int	nIndex = 0;	// points to the next position to use.
		for (int nController = 0; nController < 128; nController++)
		{
			if (m_aControllerListeners[nController] != null &&
			    m_aControllerListeners[nController].contains(listener))
			{
				anControllers[nIndex] = nController;
				nIndex++;
			}
		}
		int[]	anResultControllers = new int[nIndex];
		System.arraycopy(anControllers, 0, anResultControllers, 0, nIndex);
		return anResultControllers;
	}



	protected void sendControllerEvent(ShortMessage message)
	{
		// TDebug.out("TSequencer.sendControllerEvent(): called");
		int	nController = message.getData1();
		if (m_aControllerListeners[nController] != null)
		{
			Iterator	iterator = m_aControllerListeners[nController].iterator();
			while (iterator.hasNext())
			{
				ControllerEventListener	controllerEventListener = (ControllerEventListener) iterator.next();
				ShortMessage	copiedMessage = (ShortMessage) message.clone();
				controllerEventListener.controlChange(copiedMessage);
			}
		}
	}



	protected void notifyListeners(MidiMessage message)
	{
		if (TDebug.TraceSequencer) { TDebug.out("TSequencer.sendToListeners(): begin"); }
		if (message instanceof MetaMessage)
		{
			// IDEA: use extra thread for event delivery
			sendMetaMessage((MetaMessage) message);
		}
		else if (message instanceof ShortMessage && ((ShortMessage) message).getCommand() == ShortMessage.CONTROL_CHANGE)
		{
			sendControllerEvent((ShortMessage) message);
		}
		if (TDebug.TraceSequencer) { TDebug.out("TSequencer.sendToListeners(): end"); }
	}



	public SyncMode getMasterSyncMode()
	{
		if (TDebug.TraceSequencer) { TDebug.out("TSequencer.getMasterSyncMode(): begin"); }
		if (TDebug.TraceSequencer) { TDebug.out("TSequencer.getMasterSyncMode(): end"); }
		return m_masterSyncMode;
	}



	public void setMasterSyncMode(SyncMode syncMode)
	{
		if (TDebug.TraceSequencer) { TDebug.out("TSequencer.setMasterSyncMode(): begin"); }
		if (m_masterSyncModes.contains(syncMode))
		{
			if (! getMasterSyncMode().equals(syncMode))
			{
				m_masterSyncMode = syncMode;
				setMasterSyncModeImpl(syncMode);
			}
		}
		else
		{
			throw new IllegalArgumentException("sync mode not allowed: " + syncMode);
		}
		if (TDebug.TraceSequencer) { TDebug.out("TSequencer.setMasterSyncMode(): end"); }
	}


	/*
	  This method is guaranteed only to be called if the sync mode really changes.
	 */
	protected void setMasterSyncModeImpl(SyncMode syncMode)
	{
		if (TDebug.TraceSequencer) { TDebug.out("TSequencer.setMasterSyncModeImpl(): begin"); }
		// DO NOTHING
		if (TDebug.TraceSequencer) { TDebug.out("TSequencer.setMasterSyncModeImpl(): end"); }
	}



	public SyncMode[] getMasterSyncModes()
	{
		if (TDebug.TraceSequencer) { TDebug.out("TSequencer.getMasterSyncModes(): begin"); }
		SyncMode[]	syncModes = (SyncMode[]) m_masterSyncModes.toArray(EMPTY_SYNCMODE_ARRAY);
		if (TDebug.TraceSequencer) { TDebug.out("TSequencer.getMasterSyncModes(): end"); }
		return syncModes;
	}



	public SyncMode getSlaveSyncMode()
	{
		if (TDebug.TraceSequencer) { TDebug.out("TSequencer.getSlaveSyncMode(): begin"); }
		if (TDebug.TraceSequencer) { TDebug.out("TSequencer.getSlaveSyncMode(): end"); }
		return m_slaveSyncMode;
	}



	public void setSlaveSyncMode(SyncMode syncMode)
	{
		if (TDebug.TraceSequencer) { TDebug.out("TSequencer.setSlaveSyncMode(): begin"); }
		if (m_slaveSyncModes.contains(syncMode))
		{
			if (! getSlaveSyncMode().equals(syncMode))
			{
				m_slaveSyncMode = syncMode;
				setSlaveSyncModeImpl(syncMode);
			}
		}
		else
		{
			throw new IllegalArgumentException("sync mode not allowed: " + syncMode);
		}
		if (TDebug.TraceSequencer) { TDebug.out("TSequencer.setSlaveSyncMode(): end"); }
	}



	/*
	  This method is guaranteed only to be called if the sync mode really changes.
	 */
	protected void setSlaveSyncModeImpl(SyncMode syncMode)
	{
		if (TDebug.TraceSequencer) { TDebug.out("TSequencer.setSlaveSyncModeImpl(): begin"); }
		// DO NOTHING
		if (TDebug.TraceSequencer) { TDebug.out("TSequencer.setSlaveSyncModeImpl(): end"); }
	}



	public SyncMode[] getSlaveSyncModes()
	{
		if (TDebug.TraceSequencer) { TDebug.out("TSequencer.getSlaveSyncModes(): begin"); }
		SyncMode[]	syncModes = (SyncMode[]) m_slaveSyncModes.toArray(EMPTY_SYNCMODE_ARRAY);
		if (TDebug.TraceSequencer) { TDebug.out("TSequencer.getSlaveSyncModes(): end"); }
		return syncModes;
	}



	public boolean getTrackSolo(int nTrack)
	{
		boolean	bSoloed = false;
		if (getSequence() != null)
		{
			if (nTrack < getSequence().getTracks().length)
			{
				bSoloed = m_soloBitSet.get(nTrack);
			}
		}
		return bSoloed;
	}



	public void setTrackSolo(int nTrack, boolean bSolo)
	{
		if (getSequence() != null)
		{
			if (nTrack < getSequence().getTracks().length)
			{
				boolean	bOldState = m_soloBitSet.get(nTrack);
				if (bSolo != bOldState)
				{
					if (bSolo)
					{
						m_soloBitSet.set(nTrack);
					}
					else
					{
						m_soloBitSet.clear(nTrack);
					}
					updateEnabled();
					setTrackSoloImpl(nTrack, bSolo);
				}
			}
		}
	}



	protected void setTrackSoloImpl(int nTrack, boolean bSolo)
	{
	}



	public boolean getTrackMute(int nTrack)
	{
		boolean	bMuted = false;
		if (getSequence() != null)
		{
			if (nTrack < getSequence().getTracks().length)
			{
				bMuted = m_muteBitSet.get(nTrack);
			}
		}
		return bMuted;
	}



	public void setTrackMute(int nTrack, boolean bMute)
	{
		if (getSequence() != null)
		{
			if (nTrack < getSequence().getTracks().length)
			{
				boolean	bOldState = m_muteBitSet.get(nTrack);
				if (bMute != bOldState)
				{
					if (bMute)
					{
						m_muteBitSet.set(nTrack);
					}
					else
					{
						m_muteBitSet.clear(nTrack);
					}
					updateEnabled();
					setTrackMuteImpl(nTrack, bMute);
				}
			}
		}
	}


	protected void setTrackMuteImpl(int nTrack, boolean bMute)
	{
	}



	private void updateEnabled()
	{
		BitSet	oldEnabledBitSet = (BitSet) m_enabledBitSet.clone();
		boolean	bSoloExists = m_soloBitSet.length() > 0;
		if (bSoloExists)
		{
			m_enabledBitSet = (BitSet) m_soloBitSet.clone();
		}
		else
		{
			for (int i = 0; i < m_muteBitSet.size(); i++)
			{
				if (m_muteBitSet.get(i))
				{
					m_enabledBitSet.clear(i);
				}
				else
				{
					m_enabledBitSet.set(i);
				}
			}
		}
		oldEnabledBitSet.xor(m_enabledBitSet);
		/*	oldEnabledBitSet now has a bit set if the status for
			this bit changed.
		*/
		for (int i = 0; i < oldEnabledBitSet.size(); i++)
		{
			if (oldEnabledBitSet.get(i))
			{
				setTrackEnabledImpl(i, m_enabledBitSet.get(i));
			}
		}
	}



	/**	Shows that a track state has changed.
		This method is called for each track where the enabled
		state (calculated from mute and solo) has changed.
		The boolean value passed represents the new state.

		@param nTrack The track number for which the enabled status
		has changed.

		@param bEnabled The new enabled state for this track.
	 */
	protected void setTrackEnabledImpl(int nTrack, boolean bEnabled)
	{
	}



	protected boolean isTrackEnabled(int nTrack)
	{
		return m_enabledBitSet.get(nTrack);
	}



	/** Sets the preloading intervall.
		This is the time span between preloading events to an internal
		queue and playing them. This intervall should be kept constant
		by the implementation. However, this cannot be guaranteed.
	*/
	public void setLatency(int nMilliseconds)
	{
	}



	/** Get the preloading intervall.

	@return the preloading intervall in milliseconds, or -1 if the sequencer
	doesn't repond to changes in the <code>Sequence</code> at all.
	*/
	public int getLatency()
	{
		return -1;
	}
}



/*** TSequencer.java ***/
