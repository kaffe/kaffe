/*
 *	SysexMessage.java
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

package	javax.sound.midi;



/**	Holds the data of a MIDI System Exclusive event.
	Sysex event are tricking me over and over again. I give my
	understanding of the expected behaviour for
	review:
*/
public class SysexMessage
extends MidiMessage
{
	/**	TODO:
	*/
	public static final int		SYSTEM_EXCLUSIVE = 0xF0;

	/**	TODO:
	*/
	public static final int		SPECIAL_SYSTEM_EXCLUSIVE = 0xF7;


	/**	Create a container for a MIDI system exclusive message.
		This constructor does not create an object containing a legal
		MIDI message. You have to use one of the setMessage() methods.
		Before calling one of these methods, calling retrieval methods
		(getStatus(), getLength(), getMessage(), getData()) may have
		undesired results.

		@see #setMessage(byte[], int)
		@see #setMessage(int, byte[], int)
	*/
	public SysexMessage()
	{
		this(null);
	}



	/**	TODO:
	*/
	protected SysexMessage(byte[] abData)
	{
		super(abData);
	}



	/**	TODO:
	*/
	public void setMessage(byte[] abData, int nLength)
		throws	InvalidMidiDataException
	{
		super.setMessage(abData, nLength);
	}



	/**	TODO:
	*/
	public void setMessage(int nStatus, byte[] abData, int nLength)
		throws	InvalidMidiDataException
	{
		byte[]	abCompleteData = new byte[nLength + 1];
		abCompleteData[0] = (byte) nStatus;
		System.arraycopy(abData, 0, abCompleteData, 1, nLength);
		setMessage(abCompleteData, abCompleteData.length);
	}



	/**	TODO:
	*/
	public byte[] getData()
	{
		byte[] abData = new byte[getLength() - 1];
		System.arraycopy(getMessage(), 1, abData, 0, abData.length);
		return abData;
	}



	/**	TODO:
	*/
	public Object clone()
	{
		byte[]	abData = getMessage();
		SysexMessage	message = new SysexMessage(abData);
		return message;
	}
}



/*** SysexMessage.java ***/
