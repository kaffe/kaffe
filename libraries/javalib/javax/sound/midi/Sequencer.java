/*
 *	Sequencer.java
 */

/*
 *  Copyright (c) 1999 by Matthias Pfisterer <Matthias.Pfisterer@gmx.de>
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


package	javax.sound.midi;

import java.io.IOException;
import java.io.InputStream;


public interface Sequencer
    extends	MidiDevice
{
	public void setSequence(Sequence sequence)
		throws	InvalidMidiDataException;




	public void setSequence(InputStream inputStream)
		throws	InvalidMidiDataException, IOException;



	public Sequence getSequence();


	public void start();



	// public void start(Track track);



	public void stop();


	public boolean isRunning();

	public void startRecording();

	public void stopRecording();

	public boolean isRecording();

	// name should be: enableRecording
	public void recordEnable(Track track, int nChannel);

	// name should be: disableRecording
	public void recordDisable(Track track);


	public float getTempoInBPM();

	public void setTempoInBPM(float fBPM);


	public float getTempoInMPQ();

	public void setTempoInMPQ(float fMPQ);


	public float getTempoFactor();

	public void setTempoFactor(float fFactor);


	public long getTickLength();

	public long getTickPosition();

	public void setTickPosition(long lTick);
    

	public long getMicrosecondLength();

	public long getMicrosecondPosition();

	public void setMicrosecondPosition(long lMicroseconds);


	public Sequencer.SyncMode getMasterSyncMode();

	public void setMasterSyncMode(Sequencer.SyncMode syncMode);

	public Sequencer.SyncMode[] getMasterSyncModes();


	public Sequencer.SyncMode getSlaveSyncMode();

	public void setSlaveSyncMode(Sequencer.SyncMode syncMode);

	public Sequencer.SyncMode[] getSlaveSyncModes();


	public void setTrackMute(int nTrack, boolean bMute);

	public boolean getTrackMute(int nTrack);

	public void setTrackSolo(int nTrack, boolean bSolo);

	public boolean getTrackSolo(int nTrack);


	public boolean addMetaEventListener(MetaEventListener listener);

	public void removeMetaEventListener(MetaEventListener listener);


	public int[] addControllerEventListener(ControllerEventListener listener, int[] anControllers);
	public int[] removeControllerEventListener(ControllerEventListener listener, int[] anControllers);


////////////////////////// INNER CLASSES //////////////////////////////

	public static class SyncMode
	{
		public static final SyncMode	INTERNAL_CLOCK = new SyncMode("Internal Clock");
		public static final SyncMode	MIDI_SYNC = new SyncMode("MIDI Sync");
		public static final SyncMode	MIDI_TIME_CODE = new SyncMode("MIDI Time Code");
		public static final SyncMode	NO_SYNC = new SyncMode("No Timing");


		private String		m_strName;




		protected SyncMode(String strName)
		{
			m_strName = strName;
		}



		public final boolean equals(Object obj)
		{
			return super.equals(obj);
		}



		public final int hashCode()
		{
			return super.hashCode();
		}



		public final String toString()
		{
			return super.toString() + "[name=" + m_strName + "]";
		}



	}
}



/*** Sequencer.java ***/
