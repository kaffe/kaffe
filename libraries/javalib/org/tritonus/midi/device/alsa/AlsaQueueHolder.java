/*
 *	AlsaQueueHolder.java
 */

/*
 *  Copyright (c) 2001 by Matthias Pfisterer <Matthias.Pfisterer@gmx.de>
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


import org.tritonus.lowlevel.alsa.AlsaSeq;



/**	A representation of a physical MIDI port based on the ALSA sequencer.
 */
public class AlsaQueueHolder
{
	/**	The object interfacing to the ALSA sequencer.
	 */
	private AlsaSeq	m_aSequencer;

	/**	ALSA queue number.
	 */
	private int		m_nQueue;




	/**
	 */
	public AlsaQueueHolder(AlsaSeq aSequencer)
	{
		m_aSequencer = aSequencer;
		m_nQueue = m_aSequencer.allocQueue();
		if (m_nQueue < 0)
		{
			throw new RuntimeException("can't get ALSA sequencer queue");
		}
	}


	/**	Returns the allocated queue
		@return the queue number.
	 */
	public int getQueue()
	{
		return m_nQueue;
	}



	/**	Frees the queue.
	 */
	public void close()
	{
		m_aSequencer.freeQueue(getQueue());
	}



	public void finalize()
	{
		close();
	}
}



/*** AlsaQueueHolder.java ***/

