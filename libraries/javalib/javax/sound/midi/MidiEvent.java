/*
 *	MidiEvent.java
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




package	javax.sound.midi;


// NOTE: sun implementation is not serializable
public class MidiEvent
	implements	java.io.Serializable
{
	private MidiMessage	m_message;
	private long		m_lTick;



	public MidiEvent(MidiMessage message, long lTick)
	{
		m_message = message;
		m_lTick = lTick;
	}



	public MidiMessage getMessage()
	{
		return m_message;
	}



	public long getTick()
	{
		return m_lTick;
	}



	public void setTick(long lTick)
	{
		m_lTick = lTick;
	}
}



/*** MidiEvent.java ***/
