/*
 *	TAudioFileFormat.java
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


package	org.tritonus.share.sampled.file;


import	javax.sound.sampled.AudioFileFormat;
import	javax.sound.sampled.AudioFormat;



/**
 * This class is just to have a public constructor taking the
 * number of bytes of the whole file. The public constructor of
 * AudioFileFormat doesn't take this parameter, the one who takes
 * it is protected.
 *
 * @author Matthias Pfisterer
 */
public class TAudioFileFormat
	extends	AudioFileFormat
{
	/*
	 *	Note that the order of the arguments is different from
	 *	the one in AudioFileFormat.
	 */
	public TAudioFileFormat(Type type, AudioFormat audioFormat, int nLengthInFrames, int nLengthInBytes)
	{
		super(type, nLengthInBytes, audioFormat, nLengthInFrames);
	}
}



/*** TAudioFileFormat.java ***/
