/*
 *	Clip.java
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


package	javax.sound.sampled;


import java.io.IOException;



public interface Clip
	extends		DataLine
{
	// TODO: check value against sun implementation
	public static final int	LOOP_CONTINUOUSLY = -1;


	public void open(AudioFormat audioFormat,
			 byte[] abData,
			 int nOffset,
			 int nBufferSize)
		throws	LineUnavailableException;



	public void open(AudioInputStream audioInputStream)
		throws	LineUnavailableException, IOException;


	public int getFrameLength();


	public long getMicrosecondLength();


	public void setFramePosition(int nFrames);


	public void setMicrosecondPosition(long lMicroseconds);



	public void setLoopPoints(int nStart, int nEnd);


	public void loop(int nCount);
}



/*** Clip.java ***/

