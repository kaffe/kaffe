/*
 *	ShortMessage.java
 */

/*
 *  Copyright (c) 1999 -2002 by Matthias Pfisterer <Matthias.Pfisterer@gmx.de>
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



/**	Simple MIDI message.
	This class is used to represent channel voice, control change,
	channel mode, system real time and system common messages. These
	are the messages that can be represented in at most three bytes
	(the status byte plus two data bytes). System exclusive and meta
	messages are stored in {@link SysexMessage SysexMessage} and
	{@link MetaMessage MetaMessage}, respective.<p>

	The way
	these messages are separated into bytes for storing in
	{@link MidiMessage#data data} is the same as specified in the
	MIDI wire protocol.
*/
public class ShortMessage
extends MidiMessage
{
	/**
		If true, for messages with one data byte it is checked that
		the second one is zero. This is some kind of debugging flag.
	*/
	private static final boolean	CHECK_SECOND_BYTE = false;


	/**	Status byte for note off messages (value 128, 0x80).
	 */
	public static final int	NOTE_OFF = 0x80;


	/**	Status byte for note on messages (value 144, 0x90).
	 */
	public static final int	NOTE_ON = 0x90;


	/**	Status byte for polyphonic key pressure (aftertouch) messages (value 160, 0xA0).
	 */
	public static final int	POLY_PRESSURE = 0xA0;


	/**	Status byte for controller cange messages (value 176, 0xB0).
	 */
	public static final int	CONTROL_CHANGE = 0xB0;


	/**	Status byte for program change messages (value 192, 0xC0).
	 */
	public static final int	PROGRAM_CHANGE = 0xC0;


	/**	Status byte for channel key pressure (aftertouch) messages (value 208, 0xD0).
	 */
	public static final int	CHANNEL_PRESSURE = 0xD0;


	/**	Status byte for pitch bend messages (value 224, 0xE0).
	 */
	public static final int	PITCH_BEND = 0xE0;

	// system

	// in class SysexMessage:
	// public static final int	SYSTEM_EXCLUSIVE = 0xF0;


	/**	Status byte for MIDI time code quarter frame messages (value 241, 0xF1).
	 */
	public static final int	MIDI_TIME_CODE = 0xF1;


	/**	Status byte for song position pointer messages (value 242, 0xF2).
	 */
	public static final int	SONG_POSITION_POINTER = 0xF2;


	/**	Status byte for song select messages (value 243, 0xF3).
	 */
	public static final int	SONG_SELECT = 0xF3;

	// not defined: 0xF4
	// not defined: 0xF5


	/**	Status byte for tune request messages (value 246, 0xF6).
	 */
	public static final int	TUNE_REQUEST = 0xF6;


	/**	Status byte for end of system excusive messages (value 247, 0xF7).
	 */
	public static final int	END_OF_EXCLUSIVE = 0xF7;

	// real-time messages


	/**	Status byte for timing clock messages (value 248, 0xF8).
	 */
	public static final int	TIMING_CLOCK = 0xF8;

	// not defined: 0xF9


	/**	Status byte for start messages (value 250, 0xFA).
	 */
	public static final int	START = 0xFA;


	/**	Status byte for continue messages (value 251, 0xFB).
	 */
	public static final int	CONTINUE = 0xFB;


	/**	Status byte for stop messages (value 252, 0xFC).
	 */
	public static final int	STOP = 0xFC;

	// not defined: 0xFD


	/**	Status byte for active sensing messages (value 254, 0xFE).
	 */
	public static final int	ACTIVE_SENSING = 0xFE;


	/**	Status byte for system reset messages (value 255, 0xFF).
	 */
	public static final int	SYSTEM_RESET = 0xFF;


	/**	Table for length of channel messages.
		This table is used by {@link #getDataLength() getDataLength()}
		to calculate the length of messages.
		The table is indexed with the high nibble of the status byte, if
		the status byte is < 0xF0.
		The values in the
		table are the number of data bytes (not including the status byte)
		or -1 if the index does not represent a status byte that is usable
		for ShortMessages.

		@see #getDataLength()
	*/
	private static final int	sm_anChannelMessageLength[] =
	{ -1, -1, -1, -1, -1, -1, -1, -1, 2, 2, 2, 2, 1, 1, 2 };
	/*                               8x 9x Ax Bx Cx Dx Ex */


	/**	Table for length of system messages.
		This table is used by {@link #getDataLength() getDataLength()}
		to calculate the length of messages.
		The table is indexed with the low nibble of the status byte, if
		the status byte is >= 0xF0.
		The values in the
		table are the number of data bytes (not including the status byte)
		or -1 if the index does not represent a status byte that is usable
		for ShortMessages.

		@see #getDataLength()
	*/
	private static final int	sm_anSystemMessageLength[] = 
	{  -1, 2, 3, 2, -1, -1, 1, 0, 1, -1, 1, 1, 1, -1, 1, 1 };
	/* F0 F1 F2 F3  F4  F5 F6 F7 F8  F9 FA FB FC  FD FE FF */


// 	/**	Default ShortMessage.
// 		This are the data bytes used to construct a ShortMessage
// 		with the default constructor.
// 		The message is chosen arbitrarily. Actually, it
// 		not so much arbitrary: it is the same as in the Sun
// 		implementation.
// 	 */
// 	private static final byte[]	DEFAULT_MESSAGE = new byte[]{(byte) 144, (byte) 255, 0};




	/**	Create a container for a "short" MIDI message.
		This constructor does not create an object containing a legal
		MIDI message. You have to use one of the setMessage() methods.
		Before calling one of these methods, calling retrieval methods
		(getLength(), getMessage(), getStatus(), getChannel(),
		getCommand(), getData1(), getData2()) may have
		undesired results.

		@see #setMessage(int)
		@see #setMessage(int, int, int)
		@see #setMessage(int, int, int, int)
	*/
	public ShortMessage()
	{
		this(null);
	}



	/**	Constructs a ShortMessage with a message set.
		does not check content of passed array!!
	   TODO:
	 */
	protected ShortMessage(byte[] abData)
	{
		super(abData);
	}




	/**
	   TODO:
	*/
	public Object clone()
	{
		byte[] abData = getMessage();
		ShortMessage	message = new ShortMessage(abData);
		return message;
	}




	/**
	   TODO:
	*/
	public void setMessage(int nStatus)
		throws	InvalidMidiDataException
	{
		if (getDataLength(nStatus) != 0)
		{
			throw new InvalidMidiDataException("not a one-byte command");
		}
		byte[]	abData = new byte[1];
		abData[0] = (byte) nStatus;
		setMessage(abData, 1);
	}




	/**
	   TODO:
	*/
	public void setMessage(int nStatus, int nData1, int nData2)
		throws	InvalidMidiDataException
	{
		int	nDataLength = getDataLength(nStatus);
		if (nDataLength < 1)
		{
			throw new InvalidMidiDataException("not a two- or three-byte command");
		}
		byte[]	abData = new byte[nDataLength + 1];
		abData[0] = (byte) nStatus;
		abData[1] = (byte) nData1;
		if (nDataLength == 2)
		{
			abData[2] = (byte) nData2;
		}
		else if (nData2 != 0 && CHECK_SECOND_BYTE)
		{
			throw new InvalidMidiDataException("data2 should be 0 for a two-byte command");
		}
		setMessage(abData, nDataLength + 1);
	}



	/**
	   TODO:
	*/
	public void setMessage(int nCommand, int nChannel, int nData1, int nData2)
		throws	InvalidMidiDataException
	{
		setMessage((nCommand & 0xF0) | (nChannel & 0x0F), nData1, nData2);
	}




	/**
	   TODO:
	*/
	public int getChannel()
	{
		return (getStatus() & 0xF);
	}




	/**
	   TODO:
	*/
	public int getCommand()
	{
		return getStatus() & 0xF0;
	}




	/**
	   TODO:
	*/
	public int getData1()
	{
		return getDataByte(1);
	}




	/**
	   TODO:
	*/
	public int getData2()
	{
		return getDataByte(2);
	}




	/**
	   TODO:
	*/
	private int getDataByte(int nNumber)
	{
		int	nDataByte = 0;
		if (getLength() >= nNumber)
		{
			nDataByte = getMessage()[nNumber];
		}
		return nDataByte;
	}



	/**
	   TODO:
	*/
	protected final int getDataLength(int nStatus)
		throws	InvalidMidiDataException
	{
		int	nDataLength = 0;
		if (nStatus < 0xF0)	// channel voice message
		{
			nDataLength = sm_anChannelMessageLength[(nStatus >> 4) & 0xF];
		}
		else
		{
			nDataLength = sm_anSystemMessageLength[nStatus & 0xF];
		}
		if (nDataLength == -1)
		{
			throw new InvalidMidiDataException("status byte not usable for ShortMessage");
		}
		return nDataLength;
	}
}



/*** ShortMessage.java ***/
