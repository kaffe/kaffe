/*
 *	MidiFileFormat.java
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




public class MidiFileFormat
{
	public static final int		UNKNOWN_LENGTH = -1;

	private int		m_nType;
	private float		m_fDivisionType;
	private int		m_nResolution;
	private int		m_nByteLength;
	private long		m_lMicrosecondLength;



	public MidiFileFormat(int nType,
			      float fDivisionType,
			      int nResolution,
			      int nByteLength,
			      long lMicrosecondLength)
	{
		m_nType = nType;
		m_fDivisionType = fDivisionType;
		m_nResolution = nResolution;
		m_nByteLength = nByteLength;
		m_lMicrosecondLength = lMicrosecondLength;
	}


	public int getType()
	{
		return m_nType;
	}



	public float getDivisionType()
	{
		return m_fDivisionType;
	}



	public int getResolution()
	{
		return m_nResolution;
	}



	public int getByteLength()
	{
		return m_nByteLength;
	}



	public long getMicrosecondLength()
	{
		return m_lMicrosecondLength;
	}
}



/*** MidiFileFormat.java ***/
