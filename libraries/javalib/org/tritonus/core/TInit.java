/*
 *	TInit.java
 */

/*
 *  Copyright (c) 1999 - 2001 by Matthias Pfisterer <Matthias.Pfisterer@gmx.de>
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


import	java.util.Iterator;

import	javax.sound.midi.spi.MidiDeviceProvider;
import	javax.sound.midi.spi.MidiFileReader;
import	javax.sound.midi.spi.MidiFileWriter;
import	javax.sound.midi.spi.SoundbankReader;

import	javax.sound.sampled.spi.AudioFileWriter;
import	javax.sound.sampled.spi.AudioFileReader;
import	javax.sound.sampled.spi.FormatConversionProvider;
import	javax.sound.sampled.spi.MixerProvider;

import	org.tritonus.core.TMidiConfig;
import	org.tritonus.core.TAudioConfig;
import	org.tritonus.core.Service;
import	org.tritonus.share.TDebug;



public class TInit
{
	private static interface ProviderRegistrationAction
	{
		public void register(Object provider)
			throws	Exception;
	}



	public static void registerMidiDeviceProviders()
	{
		if (TDebug.TraceInit)
		{
			TDebug.out("TInit.registerMidiDeviceProviders(): begin");
		}
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
		registerClasses(MidiDeviceProvider.class, action);
		if (TDebug.TraceInit)
		{
			TDebug.out("TInit.registerMidiDeviceProviders(): end");
		}
	}


	
	public static void registerMidiFileReaders()
	{
		if (TDebug.TraceInit)
		{
			TDebug.out("TInit.registerMidiDeviceProviders(): begin");
		}
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
		registerClasses(MidiFileReader.class, action);
		if (TDebug.TraceInit)
		{
			TDebug.out("TInit.registerMidiDeviceProviders(): end");
		}
	}



	public static void registerMidiFileWriters()
	{
		if (TDebug.TraceInit)
		{
			TDebug.out("TInit.registerMidiDeviceProviders(): begin");
		}
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
		registerClasses(MidiFileWriter.class, action);
		if (TDebug.TraceInit)
		{
			TDebug.out("TInit.registerMidiDeviceProviders(): end");
		}
	}



	public static void registerSoundbankReaders()
	{
		if (TDebug.TraceInit)
		{
			TDebug.out("TInit.registerMidiDeviceProviders(): begin");
		}
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
		registerClasses(SoundbankReader.class, action);
		if (TDebug.TraceInit)
		{
			TDebug.out("TInit.registerMidiDeviceProviders(): end");
		}
	}



	public static void registerAudioFileReaders()
	{
		if (TDebug.TraceInit)
		{
			TDebug.out("TInit.registerMidiDeviceProviders(): begin");
		}
		ProviderRegistrationAction	action = null;
		action = new ProviderRegistrationAction()
			{
				public void register(Object obj)
					throws	Exception
					{
						AudioFileReader	provider = (AudioFileReader) obj;
						TAudioConfig.addAudioFileReader(provider);
					}
			};
		registerClasses(AudioFileReader.class, action);
		if (TDebug.TraceInit)
		{
			TDebug.out("TInit.registerMidiDeviceProviders(): end");
		}
	}



	public static void registerAudioFileWriters()
	{
		if (TDebug.TraceInit)
		{
			TDebug.out("TInit.registerMidiDeviceProviders(): begin");
		}
		ProviderRegistrationAction	action = null;
		action = new ProviderRegistrationAction()
			{
				public void register(Object obj)
					throws	Exception
					{
						AudioFileWriter	provider = (AudioFileWriter) obj;
						TAudioConfig.addAudioFileWriter(provider);
					}
			};
		registerClasses(AudioFileWriter.class, action);
		if (TDebug.TraceInit)
		{
			TDebug.out("TInit.registerMidiDeviceProviders(): end");
		}
	}



	public static void registerFormatConversionProviders()
	{
		if (TDebug.TraceInit)
		{
			TDebug.out("TInit.registerMidiDeviceProviders(): begin");
		}
		ProviderRegistrationAction	action = null;
		action = new ProviderRegistrationAction()
			{
				public void register(Object obj)
					throws	Exception
					{
						FormatConversionProvider	provider = (FormatConversionProvider) obj;
						TAudioConfig.addFormatConversionProvider(provider);
					}
			};
		registerClasses(FormatConversionProvider.class, action);
		if (TDebug.TraceInit)
		{
			TDebug.out("TInit.registerMidiDeviceProviders(): end");
		}
	}



	public static void registerMixerProviders()
	{
		if (TDebug.TraceInit)
		{
			TDebug.out("TInit.registerMidiDeviceProviders(): begin");
		}
		ProviderRegistrationAction	action = null;
		action = new ProviderRegistrationAction()
			{
				public void register(Object obj)
					throws	Exception
					{
						MixerProvider	provider = (MixerProvider) obj;
						TAudioConfig.addMixerProvider(provider);
					}
			};
		registerClasses(MixerProvider.class, action);
		if (TDebug.TraceInit)
		{
			TDebug.out("TInit.registerMidiDeviceProviders(): end");
		}
	}



	private static void registerClasses(Class providerClass,
					    ProviderRegistrationAction action)
	{
		if (TDebug.TraceInit)
		{
			TDebug.out("TInit.registerClasses(): begin" );
		}
		if (TDebug.TraceInit)
		{
			TDebug.out("TInit.registerClasses(): registering for: " + providerClass);
		}
		Iterator	providers = Service.providers(providerClass);
		if (providers != null)
		{
			while (providers.hasNext())
			{
				Object	provider = providers.next();
				try
				{
					action.register(provider);
				}
				catch (Throwable e)
				{
					if (TDebug.TraceInit || TDebug.TraceAllExceptions)
					{
						TDebug.out(e);
					}
				}
			}
		}
		if (TDebug.TraceInit)
		{
			TDebug.out("TInit.registerClasses(): end");
		}
	}

}


/*** TInit.java ***/
