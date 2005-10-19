/*
 *	AlsaSequencerProvider.java
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


package	org.tritonus.midi.device.alsa;


import javax.sound.midi.MidiDevice;
import javax.sound.midi.spi.MidiDeviceProvider;

import org.tritonus.share.GlobalInfo;
import org.tritonus.share.TDebug;
import org.tritonus.share.midi.TMidiDevice;




public class AlsaSequencerProvider
	extends		MidiDeviceProvider
{
	private static MidiDevice.Info		sm_info;



	public AlsaSequencerProvider()
	{
		if (TDebug.TraceMidiDeviceProvider) { TDebug.out("AlsaSequencerProvider.<init>(): begin"); }
		synchronized (AlsaSequencerProvider.class)
		{
			if (sm_info == null)
			{
				sm_info = new TMidiDevice.Info(
					"Tritonus ALSA Sequencer",
					GlobalInfo.getVendor(),
					"this sequencer uses the ALSA sequencer",
					GlobalInfo.getVersion());
			}
		}
		if (TDebug.TraceMidiDeviceProvider) { TDebug.out("AlsaSequencerProvider.<init>(): end"); }
	}



	public MidiDevice.Info[] getDeviceInfo()
	{
		if (TDebug.TraceMidiDeviceProvider) { TDebug.out("AlsaSequencerProvider.getDeviceInfo(): begin"); }
		MidiDevice.Info[]	infos = new MidiDevice.Info[1];
		infos[0] = sm_info;
		if (TDebug.TraceMidiDeviceProvider) { TDebug.out("AlsaSequencerProvider.getDeviceInfo(): end"); }
		return infos;
	}



	public MidiDevice getDevice(MidiDevice.Info info)
	{
		if (TDebug.TraceMidiDeviceProvider) { TDebug.out("AlsaSequencerProvider.getDevice(): begin"); }
		MidiDevice	device = null;
		if (info != null && info.equals(sm_info))
		{
			device = new AlsaSequencer(sm_info);
		}
		if (device == null)
		{
			throw new IllegalArgumentException("no device for " + info);
		}
		if (TDebug.TraceMidiDeviceProvider) { TDebug.out("AlsaSequencerProvider.getDevice(): end"); }
		return device;
	}
}



/*** AlsaSequencerProvider.java ***/
