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

	protected int		type;
	protected float		divisionType;
	protected int		resolution;
	protected int		byteLength;
	protected long		microsecondLength;



	public MidiFileFormat(int nType,
			      float fDivisionType,
			      int nResolution,
			      int nByteLength,
			      long lMicrosecondLength)
	{
		type = nType;
		divisionType = fDivisionType;
		resolution = nResolution;
		byteLength = nByteLength;
		microsecondLength = lMicrosecondLength;
	}


	public int getType()
	{
		return type;
	}



	public float getDivisionType()
	{
		return divisionType;
	}



	public int getResolution()
	{
		return resolution;
	}



	public int getByteLength()
	{
		return byteLength;
	}



	public long getMicrosecondLength()
	{
		return microsecondLength;
	}
}



/*** MidiFileFormat.java ***/
