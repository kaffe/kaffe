/*
 *	MidiDevice.java
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



public interface MidiDevice
{
	public MidiDevice.Info getDeviceInfo();



	public void open()
		throws	MidiUnavailableException;



	public void close();



	public boolean isOpen();



	public long getMicrosecondPosition();



	public int getMaxReceivers();



	public int getMaxTransmitters();



	public Receiver getReceiver()
		throws	MidiUnavailableException;



	public Transmitter getTransmitter()
		throws	MidiUnavailableException;



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


		public final boolean equals(Object obj)
		{
			return super.equals(obj);
		}



		public final int hashCode()
		{
			return super.hashCode();
		}



		public final String getName()
		{
			return m_strName;
		}



		public final String getVendor()
		{
			return m_strVendor;
		}



		public final String getDescription()
		{
			return m_strDescription;
		}



		public final String getVersion()
		{
			return m_strVersion;
		}



		public final String toString()
		{
			return super.toString() + "[name=" + getName() + ", vendor=" + getVendor() + ", description=" + getDescription() + ", version=" + getVersion() + "]";
		}
	}
}



/*** MidiDevice.java ***/
