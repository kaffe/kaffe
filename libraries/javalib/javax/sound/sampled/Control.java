/*
 *	Control.java
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


import org.tritonus.share.TDebug;



public abstract class Control
{
	private Type	m_type;



	protected Control(Type type)
	{
		if (TDebug.TraceControl)
		{
			TDebug.out("Control.<init>: begin");
		}
		m_type = type;
		if (TDebug.TraceControl)
		{
			TDebug.out("Control.<init>: end");
		}
	}



	public Type getType()
	{
		return m_type;
	}


	public String toString()
	{
		return super.toString() + "[type = " + getType() + "]";
	}




	public static class Type
	{
		private String	m_strName;



		protected Type(String strName)
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



		public final String toString()
		{
			return m_strName;
		}
	}
}



/*** Control.java ***/

