/*
 *	Port.java
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


package	javax.sound.sampled;


import org.tritonus.share.TDebug;



public interface Port
	extends		Line
{

	public static class Info
		extends	Line.Info
	{
		public static Class getPortClass()
		{
			try
			{
				return Class.forName("javax.sound.sampled.Port");
			}
			catch (ClassNotFoundException e)
			{
				if (TDebug.TraceAllExceptions)
				{
					TDebug.out(e);
				}
			}
			return null;
		}



		public static final Info	MICROPHONE = new Info(getPortClass(), "MICROPHONE", true);
		public static final Info	LINE_IN = new Info(getPortClass(), "LINE_IN", true);
		public static final Info	COMPACT_DISC = new Info(getPortClass(), "COMPACT_DISC", true);
		public static final Info	SPEAKER = new Info(getPortClass(), "SPEAKER", false);
		public static final Info	HEADPHONE = new Info(getPortClass(), "HEADPHONE", false);
		public static final Info	LINE_OUT = new Info(getPortClass(), "LINE_OUT", false);


		private String		m_strName;
		private boolean		m_bIsSource;
	



		public Info(Class lineClass,
			    String strName,
			    boolean bIsSource)
		{
			super(lineClass);
			m_strName = strName;
			m_bIsSource = bIsSource;
		}



		public String getName()
		{
			return m_strName;
		}



		public boolean isSource()
		{
			return m_bIsSource;
		}



		public boolean matches(Line.Info info)
		{
			return super.matches(info) &&
				this.getName().equals(((Port.Info) info).getName()) &&
				this.isSource() == ((Port.Info) info).isSource();
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
			return super.toString() + "[name=" + getName() + ", source = " + isSource() + "]";
		}
	}
}



/*** Port.java ***/
