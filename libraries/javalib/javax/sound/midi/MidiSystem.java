/*
 *	MidiSystem.java
 */

/*
 *  Copyright (c) 1999 - 2002 by Matthias Pfisterer <Matthias.Pfisterer@web.de>
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
 */

package	javax.sound.midi;


import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.URL;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Iterator;
import java.util.List;
import java.util.Set;

import javax.sound.midi.spi.MidiDeviceProvider;
import javax.sound.midi.spi.MidiFileReader;
import javax.sound.midi.spi.MidiFileWriter;

import org.tritonus.core.TMidiConfig;
import org.tritonus.share.ArraySet;
import org.tritonus.share.TDebug;



public class MidiSystem
{
	private static final MidiDevice.Info[]	EMPTY_MIDIDEVICE_INFO_ARRAY = new MidiDevice.Info[0];


	public static MidiDevice.Info[] getMidiDeviceInfo()
	{
		List	deviceInfos = new ArrayList();
		Iterator	providers = TMidiConfig.getMidiDeviceProviders();
		while (providers.hasNext())
		{
			MidiDeviceProvider	provider = (MidiDeviceProvider) providers.next();
			MidiDevice.Info[]	infos = provider.getDeviceInfo();
			deviceInfos.addAll(Arrays.asList(infos));
		}
		return (MidiDevice.Info[]) deviceInfos.toArray(EMPTY_MIDIDEVICE_INFO_ARRAY);
	}



	public static MidiDevice getMidiDevice(MidiDevice.Info info)
		throws	MidiUnavailableException
	{
		Iterator	providers = TMidiConfig.getMidiDeviceProviders();
		MidiDevice	device = null;
		while (providers.hasNext())
		{
			MidiDeviceProvider	provider = (MidiDeviceProvider) providers.next();
			try
			{
				return provider.getDevice(info);
			}
			catch (IllegalArgumentException e)
			{
				if (TDebug.TraceAllExceptions) { TDebug.out(e); }
			}
		}
		throw new MidiUnavailableException("no device found for " + info);
	}



	public static Receiver getReceiver()
		throws	MidiUnavailableException
	{
		MidiDevice.Info	info = TMidiConfig.getDefaultMidiDeviceInfo();
		MidiDevice	device = getMidiDevice(info);
		// to florian: try to comment out the following block
		if (!device.isOpen())
		{
			device.open();
		}
		// end of block to comment out
		Receiver	receiver = device.getReceiver();
		return receiver;
	}



	public static Transmitter getTransmitter()
		throws	MidiUnavailableException
	{
		MidiDevice.Info	info = TMidiConfig.getDefaultMidiDeviceInfo();
		MidiDevice	device = getMidiDevice(info);
		// to florian: try to comment out the following block
		if (!device.isOpen())
		{
			device.open();
		}
		// end of block to comment out
		Transmitter	transmitter = device.getTransmitter();
		return transmitter;
	}



	public static Synthesizer getSynthesizer()
		throws	MidiUnavailableException
	{
		MidiDevice.Info	info = TMidiConfig.getDefaultSynthesizerInfo();
		if (TDebug.TraceMidiSystem) { TDebug.out("MidiSystem.getSynthesizer(): using default synthesizer info: " + info); }
		Synthesizer	synthesizer = (Synthesizer) getMidiDevice(info);
		return synthesizer;
	}



	public static Sequencer getSequencer()
		throws	MidiUnavailableException
	{
		MidiDevice.Info	info = TMidiConfig.getDefaultSequencerInfo();
		if (TDebug.TraceMidiSystem) { TDebug.out("MidiSystem.getSequencer(): using: " + info); }
		Sequencer	sequencer = (Sequencer) getMidiDevice(info);
		return sequencer;
	}



	public static Soundbank getSoundbank(InputStream inputStream)
		throws	InvalidMidiDataException, IOException
	{
		return null; //TMidiConfig.getSoundbank(inputStream);
	}




	public static Soundbank getSoundbank(URL url)
		throws	InvalidMidiDataException, IOException
	{
		return null;	//TMidiConfig.getSoundbank(inputStream);
	}




	public static Soundbank getSoundbank(File file)
		throws	InvalidMidiDataException, IOException
	{
		return null;	//TMidiConfig.getSoundbank(inputStream);
	}



	public static MidiFileFormat getMidiFileFormat(InputStream inputStream)
		throws	InvalidMidiDataException, IOException
	{
		Iterator	readers = TMidiConfig.getMidiFileReaders();
		while (readers.hasNext())
		{
			MidiFileReader	reader = (MidiFileReader) readers.next();
			try
			{
				return reader.getMidiFileFormat(inputStream);
			}
			catch (IllegalArgumentException e)
			{
				if (TDebug.TraceAllExceptions) { TDebug.out(e); }
			}
		}
		throw new InvalidMidiDataException();
	}



	public static MidiFileFormat getMidiFileFormat(URL url)
		throws	InvalidMidiDataException, IOException
	{
		Iterator	readers = TMidiConfig.getMidiFileReaders();
		while (readers.hasNext())
		{
			MidiFileReader	reader = (MidiFileReader) readers.next();
			try
			{
				return reader.getMidiFileFormat(url);
			}
			catch (IllegalArgumentException e)
			{
				if (TDebug.TraceAllExceptions) { TDebug.out(e); }
			}
		}
		throw new InvalidMidiDataException();
	}



	public static MidiFileFormat getMidiFileFormat(File file)
		throws	InvalidMidiDataException, IOException
	{
		Iterator	readers = TMidiConfig.getMidiFileReaders();
		while (readers.hasNext())
		{
			MidiFileReader	reader = (MidiFileReader) readers.next();
			try
			{
				return reader.getMidiFileFormat(file);
			}
			catch (IllegalArgumentException e)
			{
				if (TDebug.TraceAllExceptions) { TDebug.out(e); }
			}
		}
		throw new InvalidMidiDataException();
	}



	public static Sequence getSequence(InputStream inputStream)
		throws	InvalidMidiDataException, IOException
	{
		Iterator	readers = TMidiConfig.getMidiFileReaders();
		while (readers.hasNext())
		{
			MidiFileReader	reader = (MidiFileReader) readers.next();
			try
			{
				return reader.getSequence(inputStream);
			}
			catch (IllegalArgumentException e)
			{
				if (TDebug.TraceAllExceptions) { TDebug.out(e); }
			}
		}
		throw new InvalidMidiDataException();
	}



	public static Sequence getSequence(URL url)
		throws	InvalidMidiDataException, IOException
	{
		Iterator	readers = TMidiConfig.getMidiFileReaders();
		while (readers.hasNext())
		{
			MidiFileReader	reader = (MidiFileReader) readers.next();
			try
			{
				return reader.getSequence(url);
			}
			catch (IllegalArgumentException e)
			{
				if (TDebug.TraceAllExceptions) { TDebug.out(e); }
			}
		}
		throw new InvalidMidiDataException();
	}



	public static Sequence getSequence(File file)
		throws	InvalidMidiDataException, IOException
	{
		Iterator	readers = TMidiConfig.getMidiFileReaders();
		if (TDebug.TraceMidiSystem) { TDebug.out("MidiSystem.getSequence(File): got readers: " + readers); }
		while (readers.hasNext())
		{
			MidiFileReader	reader = (MidiFileReader) readers.next();
			if (TDebug.TraceMidiSystem) { TDebug.out("MidiSystem.getSequence(File): Trying MidiFileReader " + reader); }
			try
			{
				return reader.getSequence(file);
			}
			catch (IllegalArgumentException e)
			{
				if (TDebug.TraceAllExceptions) { TDebug.out(e); }
			}
		}
		if (TDebug.TraceMidiSystem) { TDebug.out("MidiSystem.getSequence(File): no appropriate MidiFileReader found, throwing exception"); }
		throw new InvalidMidiDataException();
	}



	public static int[] getMidiFileTypes()
	{
		Set	fileTypes = new ArraySet();
		Iterator	writers = TMidiConfig.getMidiFileWriters();
		while (writers.hasNext())
		{
			MidiFileWriter	writer = (MidiFileWriter) writers.next();
			int[]	anTypes = writer.getMidiFileTypes();
			for (int i = 0; i < anTypes.length; i++)
			{
				fileTypes.add(new Integer(anTypes[i]));
			}
		}
		int[]	anTypes = new int[fileTypes.size()];
		Iterator	types = fileTypes.iterator();
		int	nIndex = 0;
		while (types.hasNext())
		{
			Integer	type = (Integer) types.next();
			anTypes[nIndex] = type.intValue();
			nIndex++;
		}
		return anTypes;
	}



	public static boolean isFileTypeSupported(int nFileType)
	{
		Iterator	writers = TMidiConfig.getMidiFileWriters();
		while (writers.hasNext())
		{
			MidiFileWriter	writer = (MidiFileWriter) writers.next();
			boolean	bIsSupported = writer.isFileTypeSupported(nFileType);
			if (bIsSupported)
			{
				return true;
			}
		}
		return false;
	}


	public static int[] getMidiFileTypes(Sequence sequence)
	{
		Set	fileTypes = new ArraySet();
		Iterator	writers = TMidiConfig.getMidiFileWriters();
		while (writers.hasNext())
		{
			MidiFileWriter	writer = (MidiFileWriter) writers.next();
			int[]	anTypes = writer.getMidiFileTypes(sequence);
			for (int i = 0; i < anTypes.length; i++)
			{
				fileTypes.add(new Integer(anTypes[i]));
			}
		}
		int[]	anTypes = new int[fileTypes.size()];
		Iterator	types = fileTypes.iterator();
		int	nIndex = 0;
		while (types.hasNext())
		{
			Integer	type = (Integer) types.next();
			anTypes[nIndex] = type.intValue();
			nIndex++;
		}
		return anTypes;
	}



	public static boolean isFileTypeSupported(int nFileType, Sequence sequence)
	{
		Iterator	writers = TMidiConfig.getMidiFileWriters();
		while (writers.hasNext())
		{
			MidiFileWriter	writer = (MidiFileWriter) writers.next();
			boolean	bIsSupported = writer.isFileTypeSupported(nFileType, sequence);
			if (bIsSupported)
			{
				return true;
			}
		}
		return false;
	}




	/**	MidiFileWriterAction for write().
	 */
	private static class WriteMidiFileWriterAction
		implements	MidiFileWriterAction
	{
		private Sequence	m_sequence;
		private int		m_fileType;
		private Object		m_destination;
		private int		m_nWritten;



		public WriteMidiFileWriterAction(
			Sequence sequence,
			int fileType,
			Object destination)
		{
			m_sequence = sequence;
			m_fileType = fileType;
			m_destination = destination;
			m_nWritten = -1;
		}

		public boolean handleMidiFileWriter(
			MidiFileWriter midiFileWriter)
			throws	IOException
		{
			int	nWritten = -1;
			// $$fb 2000-04-02: need to check whether this audioFileWriter is actually
			//                  capable of handling this file type !
			if (!midiFileWriter.isFileTypeSupported(m_fileType)) 
			{
				// do not interrupt the iteration
				return false;
			}
			try
			{
				if (m_destination instanceof OutputStream)
				{
					nWritten = midiFileWriter.write(
						m_sequence,
						m_fileType,
						(OutputStream) m_destination);
				}
				else if (m_destination instanceof File)
				{
					nWritten = midiFileWriter.write(
						m_sequence,
						m_fileType,
						(File) m_destination);
				}
				else
				{
					// TODO: debug message
				}
				m_nWritten = nWritten;
				// interrupt the iteration
				return true;
			}
			catch (IllegalArgumentException e)
			{
				if (TDebug.TraceMidiSystem || TDebug.TraceAllExceptions) { TDebug.out(e); }
			}
			// continue the iteration
			return false;
		}



		public int getWritten()
		{
			return m_nWritten;
		}
	}



	public static int write(Sequence sequence,
				int fileType,
				OutputStream outputStream)
		throws	IOException
	{
		return writeImpl(sequence,
				 fileType,
				 outputStream);
	}



	public static int write(Sequence sequence,
				int fileType,
				File file)
		throws	IOException
	{
		return writeImpl(sequence,
				 fileType,
				 file);
	}



	private static int writeImpl(Sequence sequence,
				     int fileType,
				     Object destination)
		throws	IOException
	{
		WriteMidiFileWriterAction	action = new WriteMidiFileWriterAction(
			sequence,
			fileType,
			destination);
		doMidiFileWriterIteration(action);
		int	nWritten = action.getWritten();
		if (nWritten == -1)
		{
			throw new IllegalArgumentException("format not  supported");
		}
		else
		{
			return nWritten;
		}
	}



/*
	public static int write(Sequence sequence,
				int nFileType,
				OutputStream outputStream)
		throws	IOException
	{
		Iterator	writers = TMidiConfig.getMidiFileWriters();
		while (writers.hasNext())
		{
			MidiFileWriter	writer = (MidiFileWriter) writers.next();
			try
			{
				return writer.write(sequence, nFileType, outputStream);
			}
			catch (IllegalArgumentException e)
			{
				if (TDebug.TraceAllExceptions) { TDebug.out(e); }
			}
		}
		throw new IllegalArgumentException ();
	}



	public static int write(Sequence sequence,
				int nFileType,
				File file)
		throws	IOException
	{
		Iterator	writers = TMidiConfig.getMidiFileWriters();
		while (writers.hasNext())
		{
			MidiFileWriter	writer = (MidiFileWriter) writers.next();
			try
			{
				return writer.write(sequence, nFileType, file);
			}
			catch (IllegalArgumentException e)
			{
				if (TDebug.TraceAllExceptions) { TDebug.out(e); }
			}
		}
		throw new IllegalArgumentException ();
	}
*/

	//----------------------------------------------------------


	private static interface MidiFileReaderAction
	{
		public boolean handleMidiFileReader(MidiFileReader audioFileReader)
			throws	IOException;
	}



	/**	Iterates over the available MidiFileReaders.
	 *	For each MidiFileReader, the mathod handleMidiFileReader() of
	 *	the passed MidiFileReaderAction is called with the MidiFileReader
	 *	in question as the only argument.
	 */
	private static void doMidiFileReaderIteration(
		MidiFileReaderAction action)
		throws	IOException
	{
		Iterator	audioFileReaders = TMidiConfig.getMidiFileReaders();
		boolean bCompleted = false;
		while (audioFileReaders.hasNext() && ! bCompleted)
		{
			MidiFileReader	audioFileReader = (MidiFileReader) audioFileReaders.next();
			if (TDebug.TraceMidiSystem) { TDebug.out("MidiSystem.doMidiFileReaderIteration(): handling MidiFileReader: " + audioFileReader); }
			bCompleted = action.handleMidiFileReader(audioFileReader);
		}
	}



	private static interface MidiFileWriterAction
	{
		public boolean handleMidiFileWriter(MidiFileWriter audioFileWriter)
			throws	IOException;
	}



	/**	Iterates over the available MidiFileWriters.
	 *	For each MidiFileWriter, the mathod handleMidiFileWriter() of
	 *	the passed MidiFileWriterAction is called with the MidiFileWriter
	 *	in question as the only argument.
	 */
	private static void doMidiFileWriterIteration(
		MidiFileWriterAction action)
		throws	IOException
	{
		Iterator	audioFileWriters = TMidiConfig.getMidiFileWriters();
		boolean bCompleted = false;
		while (audioFileWriters.hasNext() && ! bCompleted)
		{
			MidiFileWriter	audioFileWriter = (MidiFileWriter) audioFileWriters.next();
			if (TDebug.TraceMidiSystem) { TDebug.out("MidiSystem.doMidiFileWriterIteration(): handling MidiFileWriter: " + audioFileWriter); }
			bCompleted = action.handleMidiFileWriter(audioFileWriter);
		}
	}




}



/*** MidiSystem.java ***/
