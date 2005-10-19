/*
 *	LineEvent.java
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


package	javax.sound.sampled;

import java.util.EventObject;



public class LineEvent
extends EventObject
{
	static final long serialVersionUID = -1274246333383880410L;

	private Type	m_type;
	private long	m_lPosition;



	public LineEvent(Line line,
			 Type type,
			 long lPosition)
	{
		super(line);
		m_type = type;
		m_lPosition = lPosition;
	}



	public Line getLine()
	{
		return (Line) getSource();
	}



	public Type getType()
	{
		return m_type;
	}



	public long getFramePosition()
	{
		return m_lPosition;
	}



	public String toString()
	{
		return super.toString() + "[type=" + getType() + ", framePosition=" + getFramePosition() + "]";
	}





	public static class Type
	{
		public static final Type	OPEN = new Type("OPEN");
		public static final Type	CLOSE = new Type("CLOSE");
		public static final Type	START = new Type("START");
		public static final Type	STOP = new Type("STOP");


		private String	m_strName;



		public Type(String strName)
		{
			m_strName = strName;
		}



		public final boolean equals(Object obj)
		{
			return super.equals(obj);
		}



		public final int hashCode()
		{
			return super.hashCode();
		}



		public String toString()
		{
			return m_strName;
		}
	}
}



/*** LineEvent.java ***/
