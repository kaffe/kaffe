/*
 *	SourceDataLine.java
 */

/*
 *  Copyright (c) 1999 by Matthias Pfisterer <Matthias.Pfisterer@gmx.de>
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

package	javax.sound.sampled;




public interface SourceDataLine
extends DataLine
{
	public void open(AudioFormat audioFormat,
			 int nBufferSize)
		throws	LineUnavailableException;


	public void open(AudioFormat audioFormat)
		throws	LineUnavailableException;


	/** Write data to the line.

	@param abData The buffer to use.

	@param nOffset

	@param nLength The length of the data that should be written,
	in bytes. Can be less that the length of abData.

	@return The number of bytes written. May be less than nLength.

	*/
	public int write(byte[] abData,
			 int nOffset,
			 int nLength);
}



/*** SourceDataLine.java ***/

