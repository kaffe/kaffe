/*
 *	Mixer.java
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




public interface Mixer
	extends		Line
{
	public Mixer.Info getMixerInfo();

	public Line.Info[] getSourceLineInfo();

	public Line.Info[] getTargetLineInfo();

	public Line.Info[] getSourceLineInfo(Line.Info info);

	public Line.Info[] getTargetLineInfo(Line.Info info);

	public boolean isLineSupported(Line.Info info);

	public Line getLine(Line.Info info)
		throws	LineUnavailableException;

	public int getMaxLines(Line.Info info);

	public Line[] getSourceLines();

	public Line[] getTargetLines();

	public void synchronize(Line[] aLines,
				boolean bMaintainSync);


	public void unsynchronize(Line[] aLines);


	public boolean isSynchronizationSupported(Line[] aLines,
				boolean bMaintainSync);



	public static class Info
	{
		private String		m_strName;
		private String		m_strVendor;
		private String		m_strDescription;
		private String		m_strVersion;



		protected Info(String strName,
			    String strVendor,
			    String strDescription,
			    String strVersion)
		{
			m_strName = strName;
			m_strVendor = strVendor;
			m_strDescription = strDescription;
			m_strVersion = strVersion;
		}



		public boolean equals(Object obj)
		{
			return super.equals(obj);
		}



		public int hashCode()
		{
			return super.hashCode();
		}



		public String getName()
		{
			return m_strName;
		}



		public String getVendor()
		{
			return m_strVendor;
		}



		public String getDescription()
		{
			return m_strDescription;
		}



		public String getVersion()
		{
			return m_strVersion;
		}



		public String toString()
		{
			return super.toString() /* + TODO: .... */;
		}

	}
}



/*** Mixer.java ***/
