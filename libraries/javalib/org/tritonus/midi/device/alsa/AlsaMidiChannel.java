/*
 *	AlsaMidiChannel.java
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


package	org.tritonus.midi.device.alsa;


import javax.sound.midi.InvalidMidiDataException;
import javax.sound.midi.MidiChannel;
import javax.sound.midi.MidiMessage;
import javax.sound.midi.Receiver;
import javax.sound.midi.ShortMessage;

import org.tritonus.share.TDebug;
import org.tritonus.share.midi.MidiUtils;



// idea: put things that can implemented with "pure MIDI" into a base class TMidiChannel
public class AlsaMidiChannel
	implements	MidiChannel
{
	private Receiver	m_receiver;
	private int		m_nChannel;



	public AlsaMidiChannel(Receiver receiver, int nChannel)
	{
		m_receiver = receiver;
		m_nChannel = nChannel;
	}



	protected int getChannel()
	{
		return m_nChannel;
	}



	protected void sendMessage(MidiMessage message)
	{
		m_receiver.send(message, -1);
	}



	public void noteOn(int nNoteNumber, int nVelocity)
	{
		ShortMessage	message = new ShortMessage();
		try
		{
			message.setMessage(ShortMessage.NOTE_ON, getChannel(), nNoteNumber, nVelocity);
		}
		catch (InvalidMidiDataException e)
		{
			if (TDebug.TraceAlsaMidiChannel || TDebug.TraceAllExceptions)
			{
				TDebug.out(e);
			}
		}
		sendMessage(message);
	}



	public void noteOff(int nNoteNumber, int nVelocity)
	{
		ShortMessage	message = new ShortMessage();
		try
		{
			message.setMessage(ShortMessage.NOTE_OFF, getChannel(), nNoteNumber, nVelocity);
		}
		catch (InvalidMidiDataException e)
		{
			if (TDebug.TraceAlsaMidiChannel || TDebug.TraceAllExceptions)
			{
				TDebug.out(e);
			}
		}
		sendMessage(message);
	}



	public void noteOff(int nNoteNumber)
	{
		ShortMessage	message = new ShortMessage();
		try
		{
			message.setMessage(ShortMessage.NOTE_OFF, getChannel(), nNoteNumber, 0);
		}
		catch (InvalidMidiDataException e)
		{
			if (TDebug.TraceAlsaMidiChannel || TDebug.TraceAllExceptions)
			{
				TDebug.out(e);
			}
		}
		sendMessage(message);
	}



	public void setPolyPressure(int nNoteNumber, int nPressure)
	{
		ShortMessage	message = new ShortMessage();
		try
		{
			message.setMessage(ShortMessage.POLY_PRESSURE, nPressure, 0);
		}
		catch (InvalidMidiDataException e)
		{
			if (TDebug.TraceAlsaMidiChannel || TDebug.TraceAllExceptions)
			{
				TDebug.out(e);
			}
		}
		sendMessage(message);
	}



	public int getPolyPressure(int nNoteNumber)
	{
		return -1;
	}



	public void setChannelPressure(int nPressure)
	{
		ShortMessage	message = new ShortMessage();
		try
		{
			message.setMessage(ShortMessage.CHANNEL_PRESSURE, getChannel(), nPressure, 0);
		}
		catch (InvalidMidiDataException e)
		{
			if (TDebug.TraceAlsaMidiChannel || TDebug.TraceAllExceptions)
			{
				TDebug.out(e);
			}
		}
		sendMessage(message);
	}



	public int getChannelPressure()
	{
		return -1;
	}



	public void controlChange(int nController, int nValue)
	{
		ShortMessage	message = new ShortMessage();
		try
		{
			message.setMessage(ShortMessage.CONTROL_CHANGE, getChannel(), nController, nValue);
		}
		catch (InvalidMidiDataException e)
		{
			if (TDebug.TraceAlsaMidiChannel || TDebug.TraceAllExceptions)
			{
				TDebug.out(e);
			}
		}
		sendMessage(message);
	}



	public int getController(int nController)
	{
		return -1;
	}



	public void programChange(int nProgram)
	{
		ShortMessage	message = new ShortMessage();
		try
		{
			message.setMessage(ShortMessage.PROGRAM_CHANGE, getChannel(), nProgram, 0);
		}
		catch (InvalidMidiDataException e)
		{
			if (TDebug.TraceAlsaMidiChannel || TDebug.TraceAllExceptions)
			{
				TDebug.out(e);
			}
		}
		sendMessage(message);
	}



	public void programChange(int nBank, int nProgram)
	{
		ShortMessage	message = new ShortMessage();
		try
		{
			// TODO: what about the bank?
			message.setMessage(ShortMessage.PROGRAM_CHANGE, getChannel(), nProgram, 0);
		}
		catch (InvalidMidiDataException e)
		{
			if (TDebug.TraceAlsaMidiChannel || TDebug.TraceAllExceptions)
			{
				TDebug.out(e);
			}
		}
		sendMessage(message);
	}



	public int getProgram()
	{
		return -1;
	}



	public void setPitchBend(int nBend)
	{
		ShortMessage	message = new ShortMessage();
		try
		{
			message.setMessage(ShortMessage.PITCH_BEND, MidiUtils.get14bitLSB(nBend), MidiUtils.get14bitMSB(nBend));
		}
		catch (InvalidMidiDataException e)
		{
			if (TDebug.TraceAlsaMidiChannel || TDebug.TraceAllExceptions)
			{
				TDebug.out(e);
			}
		}
		sendMessage(message);
	}



	public int getPitchBend()
	{
		return -1;
	}



	public void resetAllControllers()
	{
	}



	public void allNotesOff()
	{
	}



	public void allSoundOff()
	{
	}



	public boolean localControl(boolean bOn)
	{
		return false;
	}



	public void setMono(boolean bMono)
	{
	}



	public boolean getMono()
	{
		return false;
	}



	public void setOmni(boolean bOmni)
	{
	}



	public boolean getOmni()
	{
		return false;
	}



	public void setMute(boolean bMute)
	{
	}



	public boolean getMute()
	{
		return false;
	}



	public void setSolo(boolean bSolo)
	{
	}



	public boolean getSolo()
	{
		return false;
	}
}



/*** AlsaMidiChannel.java ***/

