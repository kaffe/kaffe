/*
 *	TSeekableDataOutputStream.java
 */

/*
 *  Copyright (c) 1999 by Florian Bomers <florian@bome.com>
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


package	org.tritonus.share.sampled.file;


import java.io.File;
import java.io.IOException;
import java.io.RandomAccessFile;


/**
 * A TDataOutputStream that allows seeking.
 *
 * @author Florian Bomers
 * @author Matthias Pfisterer
 */
	
public class TSeekableDataOutputStream
	extends RandomAccessFile
	implements TDataOutputStream
{
	public TSeekableDataOutputStream(File file)
		throws IOException
	{
		super(file, "rw");
	}



	public boolean supportsSeek()
	{
		return true;
	}



	public void writeLittleEndian32(int value)
		throws IOException
	{
		writeByte(value & 0xFF);
    		writeByte((value >> 8) & 0xFF);
    		writeByte((value >> 16) & 0xFF);
	    	writeByte((value >> 24) & 0xFF);
	}



	public void writeLittleEndian16(short value)
		throws IOException
	{
		writeByte(value & 0xFF);
		writeByte((value >> 8) & 0xFF);
	}
}



/*** TSeekableDataOutputStream.java ***/
