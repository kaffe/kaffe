/*
 *	Track.java
 */

/*
 *  Copyright (c) 1999, 2000 by Matthias Pfisterer <Matthias.Pfisterer@gmx.de>
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


import java.util.ArrayList;
import java.util.List;
import java.util.Vector;



public class Track
{
	// not used; only to confirm with official API
	protected Vector	events;
	// this is the actual store
	private List		m_events;



	public Track()
	{
		m_events = new ArrayList();
	}



	public synchronized boolean add(MidiEvent event)
	{
		if (!m_events.contains(event))
		{
			int	nIndex = size() - 1;
			for (nIndex = size() - 1;
			     nIndex >= 0 && get(nIndex).getTick() > event.getTick();
			     nIndex--)
			{
			}
			m_events.add(nIndex + 1, event);
			return true;
		}
		else
		{
			return false;
		}
	}



	public synchronized boolean remove(MidiEvent event)
	{
		return m_events.remove(event);
	}



	public synchronized MidiEvent get(int nIndex)
		throws ArrayIndexOutOfBoundsException
	{
		return (MidiEvent) m_events.get(nIndex);
	}



	public synchronized int size()
	{
		return m_events.size();
	}




	public long ticks()
	{
		/*
		 *	Since ordering by tick value is guaranteed, we can
		 *	simply pick the last event and return its tick value.
		 */
		return get(size() - 1).getTick();
	}

}



/*** Track.java ***/
