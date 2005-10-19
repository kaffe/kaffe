/*
 *	TMidiConfig.java
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

package	org.tritonus.core;

import java.util.Iterator;
import java.util.Set;

import javax.sound.midi.MidiDevice;
import javax.sound.midi.Sequencer;
import javax.sound.midi.Synthesizer;
import javax.sound.midi.spi.MidiDeviceProvider;
import javax.sound.midi.spi.MidiFileReader;
import javax.sound.midi.spi.MidiFileWriter;
import javax.sound.midi.spi.SoundbankReader;

import org.tritonus.core.TInit.ProviderRegistrationAction;
import org.tritonus.share.ArraySet;
import org.tritonus.share.TDebug;


/** TODO:
 */
public class TMidiConfig
{
	private static Set		sm_midiDeviceProviders = null;
	private static Set		sm_midiFileReaders = null;
	private static Set		sm_midiFileWriters = null;
	private static Set		sm_soundbankReaders = null;

	private static MidiDevice.Info	sm_defaultMidiDeviceInfo = null;
	private static MidiDevice.Info	sm_defaultSequencerInfo = null;
	private static MidiDevice.Info	sm_defaultSynthesizerInfo = null;


	static
	{
		init();
	}

	/** Constructor to prevent instantiation.
	 */
	private TMidiConfig()
	{
	}


	/** Initialize the collections of providers and the default devices.
	 */
	private static void init()
	{
		// init providers from scanning the class path
		// note: this already sets the default devices
		getMidiDeviceProvidersImpl();
		getMidiFileReadersImpl();
		getMidiFileWritersImpl();
		getSoundbankReadersImpl();
		// now check properties for default devices
		// ... TODO:
	}



	private static void registerMidiDeviceProviders()
	{
		ProviderRegistrationAction	action = null;
		action = new ProviderRegistrationAction()
			{
				public void register(Object obj)
					throws	Exception
				{
					MidiDeviceProvider	midiDeviceProvider = (MidiDeviceProvider) obj;
					TMidiConfig.addMidiDeviceProvider(midiDeviceProvider);
				}
			};
		TInit.registerClasses(MidiDeviceProvider.class, action);
	}


	
	private static void registerMidiFileReaders()
	{
		ProviderRegistrationAction	action = null;
		action = new ProviderRegistrationAction()
			{
				public void register(Object obj)
					throws	Exception
				{
					MidiFileReader	provider = (MidiFileReader) obj;
					TMidiConfig.addMidiFileReader(provider);
				}
			};
		TInit.registerClasses(MidiFileReader.class, action);
	}



	private static void registerMidiFileWriters()
	{
		ProviderRegistrationAction	action = null;
		action = new ProviderRegistrationAction()
			{
				public void register(Object obj)
					throws	Exception
				{
					MidiFileWriter	provider = (MidiFileWriter) obj;
					TMidiConfig.addMidiFileWriter(provider);
				}
			};
		TInit.registerClasses(MidiFileWriter.class, action);
	}



	private static void registerSoundbankReaders()
	{
		ProviderRegistrationAction	action = null;
		action = new ProviderRegistrationAction()
			{
				public void register(Object obj)
					throws	Exception
				{
					SoundbankReader	provider = (SoundbankReader) obj;
					TMidiConfig.addSoundbankReader(provider);
				}
			};
		TInit.registerClasses(SoundbankReader.class, action);
	}


	//////////////////////////////////////////////////////////////////


	public static synchronized void addMidiDeviceProvider(MidiDeviceProvider provider)
	{
		// TDebug.out("MidiDeviceProvider: " + provider);
		getMidiDeviceProvidersImpl().add(provider);
		if (getDefaultMidiDeviceInfo() == null ||
		    getDefaultSynthesizerInfo() == null ||
		    getDefaultSequencerInfo() == null)
		{
			// TDebug.out("hello");
			MidiDevice.Info[]	infos = provider.getDeviceInfo();
			// TDebug.out("#infos: " + infos.length);
			for (int i = 0; i < infos.length; i++)
			{
				MidiDevice	device = null;
				try
				{
					device = provider.getDevice(infos[i]);
				}
				catch (IllegalArgumentException e)
				{
					if (TDebug.TraceAllExceptions)
					{
						TDebug.out(e);
					}
				}
				if (device instanceof Synthesizer)
				{
					if (getDefaultSynthesizerInfo() == null)
					{
						sm_defaultSynthesizerInfo = infos[i];
					}
				}
				else if (device instanceof Sequencer)
				{
					if (getDefaultSequencerInfo() == null)
					{
						sm_defaultSequencerInfo = infos[i];
					}
				}
				else
				{
					if (getDefaultMidiDeviceInfo() == null)
					{
						sm_defaultMidiDeviceInfo = infos[i];
					}
				}
			}
		}
	}



	public static synchronized void removeMidiDeviceProvider(MidiDeviceProvider provider)
	{
		getMidiDeviceProvidersImpl().remove(provider);
		// TODO: change default infos
	}



	public static synchronized Iterator getMidiDeviceProviders()
	{
		return getMidiDeviceProvidersImpl().iterator();
	}




	private static synchronized Set getMidiDeviceProvidersImpl()
	{
		if (sm_midiDeviceProviders == null)
		{
			sm_midiDeviceProviders = new ArraySet();
			registerMidiDeviceProviders();
		}
		return sm_midiDeviceProviders;
	}




	public static synchronized void addMidiFileReader(MidiFileReader reader)
	{
		if (TDebug.TraceMidiConfig)
		{
			TDebug.out("TMidiConfig.addMidiFileReader(): adding " + reader);
		}
		getMidiFileReadersImpl().add(reader);
		if (TDebug.TraceMidiConfig)
		{
			TDebug.out("TMidiConfig.addMidiFileReader(): size " + sm_midiFileReaders.size());
		}
	}



	public static synchronized void removeMidiFileReader(MidiFileReader reader)
	{
		getMidiFileReadersImpl().remove(reader);
	}



	public static synchronized Iterator getMidiFileReaders()
	{
		return getMidiFileReadersImpl().iterator();
	}



	private static synchronized Set getMidiFileReadersImpl()
	{
		if (sm_midiFileReaders == null)
		{
			sm_midiFileReaders = new ArraySet();
			registerMidiFileReaders();
		}
		return sm_midiFileReaders;
	}



	public static synchronized void addMidiFileWriter(MidiFileWriter reader)
	{
		getMidiFileWritersImpl().add(reader);
	}



	public static synchronized void removeMidiFileWriter(MidiFileWriter reader)
	{
		getMidiFileWritersImpl().remove(reader);
	}



	public static synchronized Iterator getMidiFileWriters()
	{
		return getMidiFileWritersImpl().iterator();
	}


	private static synchronized Set getMidiFileWritersImpl()
	{
		if (sm_midiFileWriters == null)
		{
			sm_midiFileWriters = new ArraySet();
			registerMidiFileWriters();
		}
		return sm_midiFileWriters;
	}




	public static synchronized void addSoundbankReader(SoundbankReader reader)
	{
		getSoundbankReadersImpl().add(reader);
	}



	public static synchronized void removeSoundbankReader(SoundbankReader reader)
	{
		getSoundbankReadersImpl().remove(reader);
	}



	public static synchronized Iterator getSoundbankReaders()
	{
		return getSoundbankReadersImpl().iterator();
	}



	private static synchronized Set getSoundbankReadersImpl()
	{
		if (sm_soundbankReaders == null)
		{
			sm_soundbankReaders = new ArraySet();
			registerSoundbankReaders();
		}
		return sm_soundbankReaders;
	}



	public static MidiDevice.Info getDefaultMidiDeviceInfo()
	{
		return sm_defaultMidiDeviceInfo;
	}



	public static MidiDevice.Info getDefaultSynthesizerInfo()
	{
		return sm_defaultSynthesizerInfo;
	}



	public static MidiDevice.Info getDefaultSequencerInfo()
	{
		return sm_defaultSequencerInfo;
	}
}


/*** TMidiConfig.java ***/
