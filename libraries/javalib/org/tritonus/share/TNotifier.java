/*
 *	TNotifier.java
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


package	org.tritonus.share;


import java.util.ArrayList;
import java.util.Collection;
import java.util.EventObject;
import java.util.Iterator;
import java.util.List;



public class TNotifier
	extends	Thread
{
	public static class NotifyEntry
	{
		private EventObject	m_event;
		private List		m_listeners;



		public NotifyEntry(EventObject event, Collection listeners)
		{
			m_event = event;
			m_listeners = new ArrayList(listeners);
		}


		public void deliver()
		{
			// TDebug.out("%% TNotifier.NotifyEntry.deliver(): called.");
			Iterator	iterator = m_listeners.iterator();
			while (iterator.hasNext())
			{
				Object	listener = iterator.next();
				if (listener instanceof javax.sound.sampled.LineListener)
				{
					((javax.sound.sampled.LineListener) listener).update((javax.sound.sampled.LineEvent) m_event);
				}
				else
				{
					// TODO: error message
				}
			}
		}
	}


	public static TNotifier	notifier = null;

	static
	{
		notifier = new TNotifier();
		notifier.setDaemon(true);
		notifier.start();
	}



	/**	The queue of events to deliver.
	 *	The entries are of class NotifyEntry.
	 */
	private List	m_entries;


	public TNotifier()
	{
		super("Tritonus Notifier");
		m_entries = new ArrayList();
	}



	public void addEntry(EventObject event, Collection listeners)
	{
		// TDebug.out("%% TNotifier.addEntry(): called.");
		synchronized (m_entries)
		{
			m_entries.add(new NotifyEntry(event, listeners));
			m_entries.notifyAll();
		}
		// TDebug.out("%% TNotifier.addEntry(): completed.");
	}


	public void run()
	{
		while (true)
		{
			NotifyEntry	entry = null;
			synchronized (m_entries)
			{
				while (m_entries.size() == 0)
				{
					try
					{
						m_entries.wait();
					}
					catch (InterruptedException e)
					{
						if (TDebug.TraceAllExceptions)
						{
							TDebug.out(e);
						}
					}
				}
				entry = (NotifyEntry) m_entries.remove(0);
			}
			entry.deliver();
		}
	}
}


/*** TNotifier.java ***/
