/*
 *	MidiChannel.java
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



public interface MidiChannel
{
	public void noteOn(int nNoteNumber, int nVelocity);
	public void noteOff(int nNoteNumber, int nVelocity);
	public void noteOff(int nNoteNumber);
	public void setPolyPressure(int nNoteNumber, int nPressure);
	public int getPolyPressure(int nNoteNumber);
	public void setChannelPressure(int nPressure);
	public int getChannelPressure();
	public void controlChange(int nController, int nValue);
	public int getController(int nController);
	public void programChange(int nProgram);
	public void programChange(int nBank, int nProgram);
	public int getProgram();
	public void setPitchBend(int nBend);
	public int getPitchBend();
	public void resetAllControllers();
	public void allNotesOff();
	public void allSoundOff();
	public boolean localControl(boolean bOn);
	public void setMono(boolean bMono);
	public boolean getMono();
	public void setOmni(boolean bOmni);
	public boolean getOmni();
	public void setMute(boolean bMute);
	public boolean getMute();
	public void setSolo(boolean bSolo);
	public boolean getSolo();
}



/*** MidiChannel.java ***/
