/*
 *	MidiFileWriter.java
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


package	javax.sound.midi.spi;

import java.io.File;
import java.io.IOException;
import java.io.OutputStream;

import javax.sound.midi.Sequence;



public abstract class MidiFileWriter
{
	public abstract int[] getMidiFileTypes();

	public abstract int[] getMidiFileTypes(Sequence sequence);

	public boolean isFileTypeSupported(int nFileType)
	{
		int[]	anFileTypes = getMidiFileTypes();
		for (int i = 0; i < anFileTypes.length; i++)
		{
			if (anFileTypes[i] == nFileType)
			{
				return true;
			}
		}
		return false;
	}


	public boolean isFileTypeSupported(int nFileType, Sequence sequence)
	{
		int[]	anFileTypes = getMidiFileTypes(sequence);
		for (int i = 0; i < anFileTypes.length; i++)
		{
			if (anFileTypes[i] == nFileType)
			{
				return true;
			}
		}
		return false;
	}


	public abstract int write(Sequence sequence,
				  int nFileType,
				  OutputStream outputStream)
		throws	IOException;


	public abstract int write(Sequence sequence,
				  int nFileType,
				  File outputFile)
		throws	IOException;
}



/*** MidiFileWriter.java ***/
