/*
 *	MidiMessage.java
 */

/*
 *  Copyright (c) 1999 - 2002 by Matthias Pfisterer <Matthias.Pfisterer@gmx.de>
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

import org.tritonus.share.TDebug;
import org.tritonus.share.midi.MidiUtils;



/**	Base class for MIDI messages.

	@author Matthias Pfisterer
*/
public abstract class MidiMessage
implements Cloneable
{
	/**	The data of the message.
		This array contains the message separated into
		bytes.
		The storage format used for this array follows the way messages
		are sent via a MIDI cable or stored in a standard MIDI file.
		See the subclasses of MidiMessage to get more information on
		the required storage format for each type.<p>

		At any time after the execution of this class'
		constructor,
		this value must refer to an array with at least one element.
		Especially, <code>null</code> is not allowed.
		The array may contain more bytes than used to store the MIDI
		message. The variable {@link #length length} indicates the number
		of valid bytes. The content of the additional bytes has to be
		considered implementation-specific.<p>

		In this implementation, the length of data is always equal to the
		value of {@link #length length}. However, this is not a
		specification requirement.

		Accessing this member from subclasses is highly discouraged.
		Subclasses should use the constructor or
		{@link #setMessage(byte[], int) setMessage} to modify the content and
		{@link #getMessage() getMessage()} to retrieve it.

		@see #setMessage(byte[], int)
		@see #getMessage()
	 */
	protected byte[]		data;


	/**	The length of the message.
		This value represents the number of bytes in the array
		{@link #data data} that are valid.
		At any time after the execution of this class'
		constructor,
		this value is guaranteed to be
		greater or equal to one.
		Additinally,  with the exception of while the control flow is inside
		this class' method {@link #setMessage(byte[], int) setMessage()}
		the value is guaranteed to be as least as big as the length of
		{@link #data data}. It may be less than the length of
		{@link #data data} in cases where {@link #data data}
		contains additional (invalid) bytes.<p>

		Accessing this member from subclasses is highly discouraged.
		Subclasses should use the constructor or
		{@link #setMessage(byte[], int) setMessage()} to modify the length
		and
		{@link #getLength() getLength()} to retrieve it.

		@see #getLength()
	*/
	protected int			length;



	/** Constructs a MidiMessage.

	The passed array is used to initialize {@link #data data}.
	{@link #length length} is initialized to the length of the
	passed array. Note that a copy of the passed array is made.<p>

	This constructor does not use {@link #setMessage(byte[], int)
	setMessage()}. So it is unaffected by overriding setMessage().<p>

	It is unclear if the argument is allowed to be null. This
	implementation allows it, resulting in {@link #data data}
	being null, too.

	@param abData The data to use to construct the MIDI message.

	 */
	protected MidiMessage(byte[] abData)
	{
		if (abData != null)
		{
			copyToData(abData, abData.length);
		}
	}



	/**	Initializes the data of the message.
		This method sets the data of the MIDI message.
		<code>nLength</code> bytes of <code>abData</code> are copied to
		a newly created array, which is set as {@link #data data}.
		{@link #length length} is initialized to the value of
		<code>nLength</code>.<p>

		Open points: should we specify that this class'
		version of setMessage() (not the ones of derived
		classes) never throws an exception? Otherwise, we have
		to specify the cases where exceptions are thrown. For
		instance: abData == null, abData.length < nLength, ...

		@param abData The bytes to use as MIDI message. This array may
		contain trailing invalid bytes. <code>nLength</code> gives the
		number of valid bytes. This means that the length of
		<code>abData</code> has to be equal or greater than
		<code>nLength</code>.

		@param nLength The length of the MIDI message. This is equal to
		the number of valid bytes in <code>abData</code>. This value
		may be less than the length of <code>abData</code>. In this case,
		the remaining bytes have to be considered invalid. This means that
		the value of <code>nLength</code> has to be equal or less than
		the length of <code>abData</code>.
	 */
	protected void setMessage(byte[] abData, int nLength)
		throws InvalidMidiDataException
	{
		copyToData(abData, nLength);
	}



	/**
	   TODO:
	*/
	private void copyToData(byte[] abData, int nLength)
	{
		synchronized (this)
		{
			data = new byte[nLength];
			System.arraycopy(abData, 0, data, 0, nLength);
			length = nLength;
		}
	}



	/**	Return the complete message.
		This method makes a copy of {@link #data data} and returns a
		reference to the copy.
		The returned array contains only the bytes that form the MIDI
		message, even if {@link #data data} contains additional invalid
		bytes. This requirement also means that the length of the
		returned array is equal to {@link #length length} and equal
		to the value returned by {@link #getLength() getLength()}.

		@return An array of bytes representing the MIDI message.
	*/
	public byte[] getMessage()
	{
		byte[]	abData = new byte[length];
		System.arraycopy(data, 0, abData, 0, length);
		return abData;
	}



	/**	Returns the status byte of the message.
		This method returns the first byte of {@link #data data},
		which is always the status byte.

		@return The status byte of the MIDI message stored in this object.
	*/
	public int getStatus()
	{
		int	nStatus = MidiUtils.getUnsignedInteger(data[0]);
		return nStatus;
	}



	/**	Returns the length of the whole message in bytes.
		This returns the value of the member {@link #length length}.
		The value returned by this method
		is always equal to the length of the array returned by
		{@link #getMessage() getMessage()}.

		@return The length of the MIDI message in bytes.

		@see #length
		@see #getMessage()
	*/
	public int getLength()
	{
		return length;
	}


	/**	Create a copy of this MIDI message.
		@return A new MidiMessage object that is a copy of this one.
	*/
	public abstract Object clone();



	/**	Print content of data[].
		This method is used for debugging.
	*/
	private void outputData()
	{
		String	strMessage = "MidiMessage.outputData(): data: [";
		for (int i  = 0; i < data.length; i++)
		{
			strMessage += (data[i] + ", ");
		}
		strMessage += "]";
		TDebug.out(strMessage);
	}
}



/*** MidiMessage.java ***/
