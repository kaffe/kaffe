/*
 *	TMidiFileFormat.java
 */

/*
 *  Copyright (c) 2000 by Matthias Pfisterer <Matthias.Pfisterer@gmx.de>
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


package	org.tritonus.share.midi;

import javax.sound.midi.MidiFileFormat;



/**	A MidiFileFormat that has information about the number of tracks.
 *	This class is used by org.tritonus.midi.file.StandardMidiFileReader.
 *	Its purpose is to carry the number of tracks from
 *	getMidiFileFormat() to getSequence().
 */
public class TMidiFileFormat
	extends		MidiFileFormat
{
	private int		m_nTrackCount;



	public TMidiFileFormat(int nType,
			      float fDivisionType,
			      int nResolution,
			      int nByteLength,
			      long lMicrosecondLength,
			       int nTrackCount)
	{
		super(nType,
		      fDivisionType,
		      nResolution,
		      nByteLength,
		      lMicrosecondLength);
		m_nTrackCount = nTrackCount;
	}



	public int getTrackCount()
	{
		return m_nTrackCount;
	}
}



/*** TMidiFileFormat.java ***/
