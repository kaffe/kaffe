/*
 *	MidiUtils.java
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


package	org.tritonus.share.midi;


import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.OutputStream;

import org.tritonus.share.TDebug;



/**	Helper methods for reading and writing MIDI files.
 */
public class MidiUtils
{
	public static int getUnsignedInteger(byte b)
	{
		return (b < 0) ? (int) b + 256 : b;
	}



	public static int get14bitValue(int nLSB, int nMSB)
	{
		return (nLSB & 0x7F) | ((nMSB & 0x7F) << 7);
	}



	public static int get14bitMSB(int nValue)
	{
		return (nValue >> 7) & 0x7F;
	}



	public static int get14bitLSB(int nValue)
	{
		return nValue & 0x7F;
	}



	public static byte[] getVariableLengthQuantity(long lValue)
	{
		ByteArrayOutputStream	data = new ByteArrayOutputStream();
		try
		{
			writeVariableLengthQuantity(lValue, data);
		}
		catch (IOException e)
		{
			if (TDebug.TraceAllExceptions) { TDebug.out(e); }
		}
		return data.toByteArray();
	}



	public static int writeVariableLengthQuantity(long lValue, OutputStream outputStream)
		throws	IOException
	{
		int	nLength = 0;
		// IDEA: use a loop
		boolean	bWritingStarted = false;
		int	nByte = (int) ((lValue >> 21) & 0x7f);
		if (nByte != 0)
		{
			if (outputStream != null)
			{
				outputStream.write(nByte | 0x80);
			}
			nLength++;
			bWritingStarted = true;
		}
		nByte = (int) ((lValue >> 14) & 0x7f);
		if (nByte != 0 || bWritingStarted)
		{
			if (outputStream != null)
			{
				outputStream.write(nByte | 0x80);
			}
			nLength++;
			bWritingStarted = true;
		}
		nByte = (int) ((lValue >> 7) & 0x7f);
		if (nByte != 0 || bWritingStarted)
		{
			if (outputStream != null)
			{
				outputStream.write(nByte | 0x80);
			}
			nLength++;
		}
		nByte = (int) (lValue & 0x7f);
		if (outputStream != null)
		{
			outputStream.write(nByte);
		}
		nLength++;
		return nLength;
	}
}



/*** MidiUtils.java ***/
