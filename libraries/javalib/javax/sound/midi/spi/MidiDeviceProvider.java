/*
 *	MidiDeviceProvider.java
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


package	javax.sound.midi.spi;


import javax.sound.midi.MidiDevice;




public abstract class MidiDeviceProvider
{
	public boolean isDeviceSupported(MidiDevice.Info info)
	{
		MidiDevice.Info[]	deviceInfos = getDeviceInfo();
		for (int i = 0; i < deviceInfos.length; i++)
		{
			if (deviceInfos[i].equals(info))
			{
				return true;
			}
		}
		return false;
	}

	public abstract MidiDevice.Info[] getDeviceInfo();

	public abstract MidiDevice getDevice(MidiDevice.Info info);

}



/*** MidiDeviceProvider.java ***/
