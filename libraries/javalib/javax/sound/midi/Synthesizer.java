/*
 *	Synthesizer.java
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




public interface Synthesizer
	extends	MidiDevice
{
	public int getMaxPolyphony();

	public long getLatency();

	public MidiChannel[] getChannels();

	public VoiceStatus[] getVoiceStatus();

	public boolean isSoundbankSupported(Soundbank soundbank);

	public boolean loadInstrument(Instrument instrument);

	public void unloadInstrument(Instrument instrument);

	public boolean remapInstrument(Instrument from, Instrument to);

	public Soundbank getDefaultSoundbank();

	public Instrument[] getAvailableInstruments();

	public Instrument[] getLoadedInstruments();

	public boolean loadAllInstruments(Soundbank soundbank);

	public void unloadAllInstruments(Soundbank soundbank);

	public boolean loadInstruments(Soundbank soundbank, Patch[] aPatches);

	public void unloadInstruments(Soundbank soundbank, Patch[] aPatches);
}



/*** Synthesizer.java ***/
