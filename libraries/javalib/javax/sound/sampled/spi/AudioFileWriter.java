/*
 *	AudioFileWriter.java
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


package	javax.sound.sampled.spi;


import java.io.File;
import java.io.IOException;
import java.io.OutputStream;

import javax.sound.sampled.AudioFileFormat;
import javax.sound.sampled.AudioInputStream;



public abstract class AudioFileWriter
{
	public abstract AudioFileFormat.Type[] getAudioFileTypes();


	public boolean isFileTypeSupported(AudioFileFormat.Type fileType)
	{
		AudioFileFormat.Type[]	aFileTypes = getAudioFileTypes();
		return isFileTypeSupportedImpl(aFileTypes, fileType);
	}



	public abstract AudioFileFormat.Type[] getAudioFileTypes(AudioInputStream audioInputStream);



	public boolean isFileTypeSupported(
		AudioFileFormat.Type fileType,
		AudioInputStream audioInputStream)
	{
		AudioFileFormat.Type[]	aFileTypes = getAudioFileTypes(audioInputStream);
		return isFileTypeSupportedImpl(aFileTypes, fileType);
	}



	public abstract int write(AudioInputStream audioInputStream,
				  AudioFileFormat.Type fileType,
				  OutputStream outputStream)
		throws	IOException;



	public abstract int write(AudioInputStream audioInputStream,
				  AudioFileFormat.Type fileType,
				  File file)
		throws	IOException;



	private boolean isFileTypeSupportedImpl(AudioFileFormat.Type[] aFileTypes, AudioFileFormat.Type fileType)
	{
		for (int i = 0; i < aFileTypes.length; i++)
		{
			if (aFileTypes[i].equals(fileType))
			{
				return true;
			}
		}
		return false;
	}
}



/*** AudioFileWriter.java ***/
