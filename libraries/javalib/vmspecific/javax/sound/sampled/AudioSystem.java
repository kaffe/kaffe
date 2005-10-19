/*
 *	AudioSystem.java
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

package	javax.sound.sampled;


import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.URL;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Set;

import javax.sound.sampled.spi.AudioFileReader;
import javax.sound.sampled.spi.AudioFileWriter;
import javax.sound.sampled.spi.FormatConversionProvider;
import javax.sound.sampled.spi.MixerProvider;

import org.tritonus.core.TAudioConfig;
import org.tritonus.share.ArraySet;
import org.tritonus.share.TDebug;


public class AudioSystem
{
	public static final int		NOT_SPECIFIED = -1;

	private static final AudioFormat[]	EMPTY_FORMAT_ARRAY = new AudioFormat[0];
	private static final AudioFormat.Encoding[]	EMPTY_ENCODING_ARRAY = new AudioFormat.Encoding[0];
	private static final AudioFileFormat.Type[]	EMPTY_TYPE_ARRAY = new AudioFileFormat.Type[0];
	private static final Mixer.Info[]	EMPTY_MIXER_INFO_ARRAY = new Mixer.Info[0];
	private static final Line.Info[]	EMPTY_LINE_INFO_ARRAY = new Line.Info[0];



	/**	MixerProviderAction for getMixerInfo().
	 */
	private static class GetMixerInfoMixerProviderAction
	implements	MixerProviderAction
	{
		private Collection	m_mixerInfos = new ArrayList();



		public GetMixerInfoMixerProviderAction()
		{
		}



		public boolean handleMixerProvider(
			MixerProvider mixerProvider)
		{
			Mixer.Info[]	aMixerInfos = mixerProvider.getMixerInfo();
			if (aMixerInfos != null)
			{
				// TODO: is this if needed?
				if (aMixerInfos.length > 0)
				{
					if (TDebug.TraceAudioSystem)
					{
						TDebug.out("AudioSystem.getMixerInfo(): MixerProvider returns array:");
						for (int i = 0; i < aMixerInfos.length; i++)
						{
							TDebug.out("" + aMixerInfos[i]);
						}
					}
				}
				else if (TDebug.TraceAudioSystem)
				{
					TDebug.out("AudioSystem.getMixerInfo(): MixerProvider returns empty array.");
				}
				m_mixerInfos.addAll(Arrays.asList(aMixerInfos));
			}
			else if (TDebug.TraceAudioSystem)
			{
				TDebug.out("AudioSystem.getMixerInfo(): MixerProvider returns null.");
			}
			// always continue
			return false;
		}



		public Mixer.Info[] getMixerInfos()
		{
			return (Mixer.Info[]) m_mixerInfos.toArray(EMPTY_MIXER_INFO_ARRAY);
		}
	}



	public static Mixer.Info[] getMixerInfo()
	{
		GetMixerInfoMixerProviderAction	action = new GetMixerInfoMixerProviderAction();
		doMixerProviderIteration(action);
		Mixer.Info[] infos = action.getMixerInfos();
		// TDebug.out("MI length: " + infos.length);
		return action.getMixerInfos();
	}



	/**	MixerProviderAction for getMixer().
	 */
	private static class GetMixerMixerProviderAction
	implements	MixerProviderAction
	{
		private Mixer.Info	m_info = null;
		private Mixer		m_mixer = null;



		public GetMixerMixerProviderAction(Mixer.Info info)
		{
			m_info = info;
		}



		public boolean handleMixerProvider(
			MixerProvider mixerProvider)
		{
			try
			{
				Mixer	mixer = mixerProvider.getMixer(m_info);
				if (m_mixer == null)
				{
					m_mixer = mixer;
					// now interrupt the iteration
					return true;
				}
			}
			catch (IllegalArgumentException e)
			{
				if (TDebug.TraceAudioSystem || TDebug.TraceAllExceptions) { TDebug.out(e); }
			}
			// continue the iteration
			return false;
		}



		public Mixer getMixer()
		{
			return m_mixer;
		}
	}



	public static Mixer getMixer(Mixer.Info info)
	{
		if (info == null)
		{
			// TODO: this currently always yields null!!!
			// but default mixers are handled by some magic in
			// TMixerProvider
			info = TAudioConfig.getDefaultMixerInfo();
		}
		GetMixerMixerProviderAction	action = new GetMixerMixerProviderAction(info);
		doMixerProviderIteration(action);
		Mixer	mixer = action.getMixer();
		if (mixer != null)
		{
			return mixer;
		}
		else
		{
			throw new IllegalArgumentException("no mixer found for " + info);
		}
	}



	/**	MixerAction for getSourceLineInfo().
	 */
	private static class GetSourceLineInfoMixerAction
	implements	MixerAction
	{
		private Line.Info	m_info = null;
		private Set		m_infos;



		public GetSourceLineInfoMixerAction(Line.Info info)
		{
			m_info = info;
			m_infos = new ArraySet();
		}



		public boolean handleMixer(
			Mixer mixer)
		{
			Line.Info[]	infos = mixer.getSourceLineInfo(m_info);
			m_infos.addAll(Arrays.asList(infos));
			// always continue the iteration
			return false;
		}



		public Line.Info[] getInfos()
		{
			return (Line.Info[]) m_infos.toArray(EMPTY_LINE_INFO_ARRAY);
		}
	}



	public static Line.Info[] getSourceLineInfo(Line.Info info)
	{
		GetSourceLineInfoMixerAction	action = new GetSourceLineInfoMixerAction(info);
		doMixerIteration(action);
		return action.getInfos();
	}



	/**	MixerAction for getTargetLineInfo().
	 */
	private static class GetTargetLineInfoMixerAction
	implements	MixerAction
	{
		private Line.Info	m_info = null;
		private Set		m_infos;



		public GetTargetLineInfoMixerAction(Line.Info info)
		{
			m_info = info;
			m_infos = new ArraySet();
		}



		public boolean handleMixer(
			Mixer mixer)
		{
			Line.Info[]	infos = mixer.getTargetLineInfo(m_info);
			m_infos.addAll(Arrays.asList(infos));
			// always continue the iteration
			return false;
		}



		public Line.Info[] getInfos()
		{
			return (Line.Info[]) m_infos.toArray(EMPTY_LINE_INFO_ARRAY);
		}
	}



	public static Line.Info[] getTargetLineInfo(Line.Info info)
	{
		GetTargetLineInfoMixerAction	action = new GetTargetLineInfoMixerAction(info);
		doMixerIteration(action);
		return action.getInfos();
	}



	/**	MixerAction for isLineSupported().
	 */
	private static class IsLineSupportedMixerAction
	implements	MixerAction
	{
		private Line.Info	m_info = null;
		private boolean		m_bSupported = false;



		public IsLineSupportedMixerAction(Line.Info info)
		{
			m_info = info;
		}



		public boolean handleMixer(
			Mixer mixer)
		{
			boolean bSupported = mixer.isLineSupported(m_info);
			m_bSupported |= bSupported;
			// interrupt the iteration depending on the result
			return bSupported;
		}



		public boolean isSupported()
		{
			return m_bSupported;
		}
	}



	public static boolean isLineSupported(Line.Info info)
	{
		IsLineSupportedMixerAction	action = new IsLineSupportedMixerAction(info);
		doMixerIteration(action);
		return action.isSupported();
	}



	/**	MixerAction for getLine().
	 */
	private static class GetLineMixerAction
	implements	MixerAction
	{
		private Line.Info	m_info = null;
		private Line		m_line = null;
		private boolean		m_bLineTypeSupported = false;



		public GetLineMixerAction(Line.Info info)
		{
			m_info = info;
		}



		public boolean handleMixer(
			Mixer mixer)
		{
			try
			{
				Line	line = mixer.getLine(m_info);
				if (m_line == null)
				{
					m_line = line;
					// interrupt the iteration
					return true;
				}
			}
			catch (LineUnavailableException e)
			{
				m_bLineTypeSupported = true;
				if (TDebug.TraceAudioSystem || TDebug.TraceAllExceptions) { TDebug.out(e); }
			}
			catch (IllegalArgumentException e)
			{
				if (TDebug.TraceAudioSystem || TDebug.TraceAllExceptions) { TDebug.out(e); }
			}
			// continue the iteration
			return false;
		}



		public Line getLine()
		{
			return m_line;
		}



		public boolean isLineTypeSupported()
		{
			return m_bLineTypeSupported;
		}
	}



	public static Line getLine(Line.Info info)
		throws	LineUnavailableException
	{
		GetLineMixerAction	action = new GetLineMixerAction(info);
		doMixerIteration(action);
		Line	line = action.getLine();
		if (line != null)
		{
			return line;
		}
		else if (action.isLineTypeSupported())
		{
			throw new LineUnavailableException("currently no line available due to resource restrictions");
		}
		else
		{
			throw new IllegalArgumentException("no mixer supporting this type of line: " + info);
		}
	}



	/**	FormatConversionProviderAction for getTargetEncodings().
	 */
	private static class GetTargetEncodingsFormatConversionProviderAction
	implements	FormatConversionProviderAction
	{
		private Object		m_sourceDescription;
		private Collection	m_targetEncodings;


		//$$fb 2000-08-15: added for workaround below
		public GetTargetEncodingsFormatConversionProviderAction() {
			this(null);
		}

		public GetTargetEncodingsFormatConversionProviderAction(
			Object sourceDescription)
		{
			m_sourceDescription = sourceDescription;
			m_targetEncodings = new ArraySet();
		}


		public boolean handleFormatConversionProvider(
			FormatConversionProvider formatConversionProvider)
		{
			// $$fb 2000-03-30 default to empty array
			AudioFormat.Encoding[]	encodings = EMPTY_ENCODING_ARRAY;
			if (m_sourceDescription instanceof AudioFormat.Encoding)
			{
				// TODO: not directely implementable. Contact Sun.
				//$$fb 2000-08-15: see workaround below
				/*
				  encodings = formatConversionProvider.getTargetEncodings(
				  (AudioFormat.Encoding) m_sourceDescription);
				*/
			}
			else if (m_sourceDescription instanceof AudioFormat)
			{
				encodings = formatConversionProvider.getTargetEncodings(
					(AudioFormat) m_sourceDescription);
			}
			else
			{
				// TODO: debug message
			}
			m_targetEncodings.addAll(Arrays.asList(encodings));
			// continue the iteration
			return false;
		}


		public AudioFormat.Encoding[] getEncodings()
		{
			return (AudioFormat.Encoding[]) m_targetEncodings.toArray(EMPTY_ENCODING_ARRAY);
		}

		//$$fb 2000-08-15: added for workaround below
		public void setSourceDescription(Object sourceDescription) {
			m_sourceDescription = sourceDescription;
		}
			
	}



	//$$fb 2000-08-15: added for workaround below
	private static void doEncodingActionWorkaround(boolean bigEndian, AudioFormat.Encoding encoding, 
						       GetTargetEncodingsFormatConversionProviderAction action) {
		AudioFormat format=new AudioFormat(
			encoding, 
			NOT_SPECIFIED, // sample rate
			NOT_SPECIFIED, // sample size in bits
			NOT_SPECIFIED, // channels
			NOT_SPECIFIED, // frame size
			NOT_SPECIFIED, // frame rate,
			bigEndian);
		action.setSourceDescription(format);
		doFormatConversionProviderIteration(action);
	}

	public static AudioFormat.Encoding[] getTargetEncodings(
		AudioFormat.Encoding sourceEncoding)
	{
		//$$fb 2000-08-15: workaround
		//return getTargetEncodings((Object) sourceEncoding);
		GetTargetEncodingsFormatConversionProviderAction action = 
			new GetTargetEncodingsFormatConversionProviderAction();
		doEncodingActionWorkaround(false, sourceEncoding, action);
		doEncodingActionWorkaround(true, sourceEncoding, action);
		return action.getEncodings();
	}



	public static AudioFormat.Encoding[] getTargetEncodings(
		AudioFormat sourceFormat)
	{
		return getTargetEncodings((Object) sourceFormat);
	}



	private static AudioFormat.Encoding[] getTargetEncodings(
		Object sourceDescription)
	{
		GetTargetEncodingsFormatConversionProviderAction	action = new GetTargetEncodingsFormatConversionProviderAction(sourceDescription);
		doFormatConversionProviderIteration(action);
		return action.getEncodings();
	}



	/**	FormatConversionProviderAction for isConversionSupported().
	 */
	private static class IsConversionSupportedFormatConversionProviderAction
	implements	FormatConversionProviderAction
	{
		private AudioFormat		m_sourceFormat;
		/*
		 *	May be AudioFormat or AudioFormat.Encoding.
		 */
		private Object			m_targetDescription;
		private boolean			m_bSupported;



		public IsConversionSupportedFormatConversionProviderAction(
			AudioFormat sourceFormat,
			Object targetDescription)
		{
			m_sourceFormat = sourceFormat;
			m_targetDescription = targetDescription;
			m_bSupported = false;
		}



		public boolean handleFormatConversionProvider(
			FormatConversionProvider formatConversionProvider)
		{
			boolean bSupported = false;
			if (m_targetDescription instanceof AudioFormat.Encoding)
			{
				bSupported = formatConversionProvider.isConversionSupported(
					(AudioFormat.Encoding) m_targetDescription,
					m_sourceFormat);
			}
			else if (m_targetDescription instanceof AudioFormat)
			{
				bSupported = formatConversionProvider.isConversionSupported(
					(AudioFormat) m_targetDescription,
					m_sourceFormat);
			}
			else
			{
				// TODO: debug message
			}
			m_bSupported |= bSupported;
			// interrupt the iteration depending on the result
			return bSupported;
		}



		public boolean isSupported()
		{
			return m_bSupported;
		}
	}



	public static boolean isConversionSupported(
		AudioFormat.Encoding targetEncoding,
		AudioFormat sourceFormat)
	{
		IsConversionSupportedFormatConversionProviderAction	action = new IsConversionSupportedFormatConversionProviderAction(sourceFormat, targetEncoding);
		doFormatConversionProviderIteration(action);
		return action.isSupported();
	}



	/**	FormatConversionProviderAction for getAudioInputStream().
	 */
	private static class GetAudioInputStreamFormatConversionProviderAction
	implements	FormatConversionProviderAction
	{
		private AudioInputStream	m_sourceAudioInputStream;
		private Object			m_targetDescription;
		private AudioInputStream	m_targetAudioInputStream;



		public GetAudioInputStreamFormatConversionProviderAction(
			AudioInputStream audioInputStream,
			Object targetDescription)
		{
			m_sourceAudioInputStream = audioInputStream;
			m_targetDescription = targetDescription;
			m_targetAudioInputStream = null;
		}



		public boolean handleFormatConversionProvider(
			FormatConversionProvider formatConversionProvider)
		{
			AudioInputStream	audioInputStream = null;
			try
			{
				if (m_targetDescription instanceof AudioFormat.Encoding)
				{
					audioInputStream = formatConversionProvider.getAudioInputStream(
						(AudioFormat.Encoding) m_targetDescription,
						m_sourceAudioInputStream);
				}
				else if (m_targetDescription instanceof AudioFormat)
				{
					audioInputStream = formatConversionProvider.getAudioInputStream(
						(AudioFormat) m_targetDescription,
						m_sourceAudioInputStream);
				}
				else
				{
					// TODO: debug message
				}
				m_targetAudioInputStream = audioInputStream;
				// interrupt the iteration
				return true;
			}
			catch (IllegalArgumentException e)
			{
				if (TDebug.TraceAudioSystem || TDebug.TraceAllExceptions) { TDebug.out(e); }
			}
			// continue the iteration
			return false;
		}



		public AudioInputStream getAudioInputStream()
		{
			return m_targetAudioInputStream;
		}
	}



	public static AudioInputStream getAudioInputStream(
		AudioFormat.Encoding targetEncoding,
		AudioInputStream sourceStream)
	{
		return getAudioInputStreamImpl((Object) targetEncoding,
					       sourceStream);
	}



	private static AudioInputStream getAudioInputStreamImpl(
		Object targetDescription,
		AudioInputStream sourceStream)
	{
		GetAudioInputStreamFormatConversionProviderAction	action = new GetAudioInputStreamFormatConversionProviderAction(sourceStream, targetDescription);
		doFormatConversionProviderIteration(action);
		AudioInputStream	audioInputStream = action.getAudioInputStream();
		if (audioInputStream != null)
		{
			return audioInputStream;
		}
		else
		{
			throw new IllegalArgumentException("conversion not supported");
		}
	}



	public static AudioFormat[] getTargetFormats(
		AudioFormat.Encoding targetEncoding,
		AudioFormat sourceFormat)
	{
		// TODO:
		return null;
	}


	public static boolean isConversionSupported(
		AudioFormat targetFormat,
		AudioFormat sourceFormat)
	{
		IsConversionSupportedFormatConversionProviderAction	action = new IsConversionSupportedFormatConversionProviderAction(sourceFormat, targetFormat);
		doFormatConversionProviderIteration(action);
		return action.isSupported();
	}



	public static AudioInputStream getAudioInputStream(
		AudioFormat targetFormat,
		AudioInputStream sourceStream)
	{
		return getAudioInputStreamImpl((Object) targetFormat,
					       sourceStream);
	}




	// $$fb 19 Dec 99: what about cascaded conversions ?
	// we'll never have an alround-converter doing all possible conversions.
	// we could implement an algorithm that, if no conversion is available,
	// in a second step a conversion to PCM is tried, followed by a conversion from
	// PCM to the requested target format.
	// we might even introduce a third level for inter-PCM-conversion.
	// e.g. we want to convert from aLaw 8000Hz to uLaw 44100Hz.
	// 1. the first round we won't find a direct converter
	// 2. try to find a converter from aLaw 8000 to PCM 44100
	//    we won't find it.
	// 3. third level: try to find a converter from alaw 8000 to PCM 8000
	//    we'll find one.
	//    then find a converter from PCM 8000 to PCM 44100
	//    let's say we have that already.
	//    then find a converter from PCM 44100 to ulaw 44100.
	// I think that sounds more impossible than it is, when
	// we have a powerful PCM-PCM converter.
/*
  public static AudioInputStream getAudioInputStream(AudioFormat audioFormat, AudioInputStream audioInputStream)
  {
  Iterator	formatConversionProviders = TAudioConfig.getFormatConversionProviders();
  while (formatConversionProviders.hasNext())
  {
  FormatConversionProvider	formatConversionProvider = (FormatConversionProvider) formatConversionProviders.next();
  if (TDebug.TraceAudioSystem)
  {
  System.out.print("AudioSystem.getAudioInputStream(AudioFormat, AudioInputStream): asking FormatConversionProvider: " + formatConversionProvider + "...");
  }
  try
  {
  AudioInputStream	outputStream = formatConversionProvider.getConvertedStream(audioFormat, audioInputStream);
  if (TDebug.TraceAudioSystem)
  {
  TDebug.out("ok");
  }
  return outputStream;
  }
  catch (IllegalArgumentException e)
  {
  if (TDebug.TraceAudioSystem || TDebug.TraceAllExceptions)
  {
  TDebug.out("failed");
  TDebug.out(e);
  }
  }
  }
  if (TDebug.TraceAudioSystem)
  {
  TDebug.out("AudioSystem.getAudioInputStream(AudioFormat, AudioInputStream): no suitable FormatConversionProvider found. Returning null.");
  }
  return null;
  }
*/
/*
  public static AudioInputStream getAudioInputStream(Type encoding, AudioInputStream audioInputStream)
  {
  AudioFormat	inputFormat = audioInputStream.getFormat();
  // $$fb 19 Dec 99: Here I think, sample size in bits and framesize should
  // be set to NOT_SPECIFIED, as we cannot assume that it stays
  // the same with the new encoding.
  */
	/* $$mp19991220: Yes, possible optimisation:
	   find all formats with the given encoding that can be
	   generated by conversion from the stream. Then pick the
	   one nearest to the original format.
	*/
/*
  AudioFormat	outputFormat = new AudioFormat(encoding,
  inputFormat.getSampleRate(),
  inputFormat.getSampleSizeInBits(),
  inputFormat.getChannels(),
  inputFormat.getFrameSize(),
  inputFormat.getFrameRate(),
  inputFormat.isBigEndian());
  return getAudioInputStream(outputFormat, audioInputStream);
  }
*/


	/**	AudioFileReaderAction for getAudioFileFormat().
	 */
	private static class GetAudioFileFormatAudioFileReaderAction
	implements	AudioFileReaderAction
	{
		private Object			m_source = null;
		private AudioFileFormat		m_audioFileFormat = null;



		public GetAudioFileFormatAudioFileReaderAction(Object source)
		{
			m_source = source;
		}



		public boolean handleAudioFileReader(
			AudioFileReader audioFileReader)
			throws	IOException
		{
			AudioFileFormat	audioFileFormat = null;
			try
			{
				if (m_source instanceof InputStream)
				{
					audioFileFormat = audioFileReader.getAudioFileFormat((InputStream) m_source);
				}
				else if (m_source instanceof File)
				{
					audioFileFormat = audioFileReader.getAudioFileFormat((File) m_source);
				}
				else if (m_source instanceof URL)
				{
					audioFileFormat = audioFileReader.getAudioFileFormat((URL) m_source);
				}
				else
				{
					// TODO: debug message
				}
				if (m_audioFileFormat == null)
				{
					m_audioFileFormat = audioFileFormat;
					// interrupt the iteration
					return true;
				}
			}
			catch (UnsupportedAudioFileException e)
			{
				if (TDebug.TraceAudioSystem || TDebug.TraceAllExceptions) { TDebug.out(e); }
			}
			// continue the iteration
			return false;
		}



		public AudioFileFormat getAudioFileFormat()
		{
			return m_audioFileFormat;
		}
	}



	public static AudioFileFormat getAudioFileFormat(
		InputStream inputStream)
		throws	UnsupportedAudioFileException, IOException
	{
		return getAudioFileFormatImpl(inputStream);
	}



	public static AudioFileFormat getAudioFileFormat(URL url)
		throws	UnsupportedAudioFileException, IOException
	{
		return getAudioFileFormatImpl(url);
	}



	public static AudioFileFormat getAudioFileFormat(File file)
		throws	UnsupportedAudioFileException, IOException
	{
		return getAudioFileFormatImpl(file);
	}



	private static AudioFileFormat getAudioFileFormatImpl(
		Object source)
		throws	UnsupportedAudioFileException, IOException
	{
		GetAudioFileFormatAudioFileReaderAction	action = new GetAudioFileFormatAudioFileReaderAction(source);
		doAudioFileReaderIteration(action);
		AudioFileFormat	audioFileFormat = action.getAudioFileFormat();
		if (audioFileFormat != null)
		{
			return audioFileFormat;
		}
		else
		{
			throw new UnsupportedAudioFileException("format not supported");
		}
	}



	/**	AudioFileReaderAction for getAudioInputStream().
	 */
	private static class GetAudioInputStreamAudioFileReaderAction
	implements	AudioFileReaderAction
	{
		private Object			m_source = null;
		private AudioInputStream	m_audioInputStream = null;



		public GetAudioInputStreamAudioFileReaderAction(Object source)
		{
			m_source = source;
		}



		public boolean handleAudioFileReader(
			AudioFileReader audioFileReader)
			throws	IOException
		{
			AudioInputStream	audioInputStream = null;
			try
			{
				if (m_source instanceof InputStream)
				{
					audioInputStream = audioFileReader.getAudioInputStream((InputStream) m_source);
				}
				else if (m_source instanceof File)
				{
					audioInputStream = audioFileReader.getAudioInputStream((File) m_source);
				}
				else if (m_source instanceof URL)
				{
					audioInputStream = audioFileReader.getAudioInputStream((URL) m_source);
				}
				else
				{
					// TODO: debug message
				}
				if (m_audioInputStream == null)
				{
					m_audioInputStream = audioInputStream;
					// interrupt the iteration
					return true;
				}
			}
			catch (UnsupportedAudioFileException e)
			{
				if (TDebug.TraceAudioSystem || TDebug.TraceAllExceptions) { TDebug.out(e); }
			}
			// continue the iteration
			return false;
		}



		public AudioInputStream getAudioInputStream()
		{
			return m_audioInputStream;
		}
	}



	public static AudioInputStream getAudioInputStream(InputStream inputStream)
		throws	UnsupportedAudioFileException, IOException
	{
		return getAudioInputStreamImpl(inputStream);
	}



	public static AudioInputStream getAudioInputStream(URL url)
		throws	UnsupportedAudioFileException, IOException
	{
		return getAudioInputStreamImpl(url);
	}



	public static AudioInputStream getAudioInputStream(File file)
		throws	UnsupportedAudioFileException, IOException
	{
		return getAudioInputStreamImpl(file);
	}



	private static AudioInputStream getAudioInputStreamImpl(
		Object source)
		throws	UnsupportedAudioFileException, IOException
	{
		GetAudioInputStreamAudioFileReaderAction	action = new GetAudioInputStreamAudioFileReaderAction(source);
		doAudioFileReaderIteration(action);
		AudioInputStream	audioInputStream = action.getAudioInputStream();
		if (audioInputStream != null)
		{
			return audioInputStream;
		}
		else
		{
			throw new UnsupportedAudioFileException("format not supported");
		}
	}



	public static AudioFileFormat.Type[] getAudioFileTypes()
	{
		Set		supportedTypes = new HashSet();
		Iterator	audioFileWriters = TAudioConfig.getAudioFileWriters();
		if (TDebug.TraceAudioSystem) { TDebug.out(">AudioSystem.getAudioFileTypes()"); }
		while (audioFileWriters.hasNext())
		{
			AudioFileWriter	audioFileProvider = (AudioFileWriter) audioFileWriters.next();
			if (TDebug.TraceAudioSystem) { TDebug.out("trying AudioFileWriter: " + audioFileProvider); }
			AudioFileFormat.Type[]	aSupportedTypes = audioFileProvider.getAudioFileTypes();
			if (TDebug.TraceAudioSystem) { TDebug.out("this AudioFileWriter supports the following Types:"); }
			for (int i = 0; i < aSupportedTypes.length; i++)
			{
				if (TDebug.TraceAudioSystem) { TDebug.out(aSupportedTypes[i].toString()); }
				supportedTypes.add(aSupportedTypes[i]);
			}
		}
		if (TDebug.TraceAudioSystem) { TDebug.out("< returning "+supportedTypes.size()+" types."); }
		return (AudioFileFormat.Type[]) supportedTypes.toArray(EMPTY_TYPE_ARRAY);
	}



	/**	AudioFileWriterAction for isFileTypeSupported().
	 */
	private static class IsFileTypeSupportedAudioFileWriterAction
	implements	AudioFileWriterAction
	{
		private AudioFileFormat.Type	m_fileType;
		private AudioInputStream	m_audioInputStream;
		private boolean			m_bSupported;



		public IsFileTypeSupportedAudioFileWriterAction(
			AudioFileFormat.Type fileType,
			AudioInputStream audioInputStream)
		{
			m_fileType = fileType;
			m_audioInputStream = audioInputStream;
			m_bSupported = false;
		}



		public boolean handleAudioFileWriter(
			AudioFileWriter mixer)
		{
			boolean bSupported = false;
			if (m_audioInputStream == null)
			{
				bSupported = mixer.isFileTypeSupported(
					m_fileType);
			}
			else
			{
				bSupported = mixer.isFileTypeSupported(
					m_fileType,
					m_audioInputStream);
			}
			m_bSupported |= bSupported;
			// interrupt the iteration depending on the result
			return bSupported;
		}



		public boolean isSupported()
		{
			return m_bSupported;
		}
	}



	public static boolean isFileTypeSupported(
		AudioFileFormat.Type fileType)
	{
		return isFileTypeSupported(fileType, null);
	}



	public static AudioFileFormat.Type[] getAudioFileTypes(
		AudioInputStream audioInputStream)
	{
		Set		supportedTypes = new HashSet();
		Iterator	audioFileWriters = TAudioConfig.getAudioFileWriters();
		if (TDebug.TraceAudioSystem) { TDebug.out(">AudioSystem.getAudioFileTypes()"); }
		while (audioFileWriters.hasNext())
		{
			AudioFileWriter	audioFileProvider = (AudioFileWriter) audioFileWriters.next();
			if (TDebug.TraceAudioSystem) { TDebug.out("trying AudioFileWriter: " + audioFileProvider); }
			AudioFileFormat.Type[]	aSupportedTypes = audioFileProvider.getAudioFileTypes(audioInputStream);
			if (TDebug.TraceAudioSystem) { TDebug.out("this AudioFileWriter supports the following Types:"); }
			for (int i = 0; i < aSupportedTypes.length; i++)
			{
				if (TDebug.TraceAudioSystem)
				{
					TDebug.out(aSupportedTypes[i].toString());
				}
				supportedTypes.add(aSupportedTypes[i]);
			}
		}
		if (TDebug.TraceAudioSystem) { TDebug.out("< returning "+supportedTypes.size()+" types."); }
		return (AudioFileFormat.Type[]) supportedTypes.toArray(EMPTY_TYPE_ARRAY);
	}



	public static boolean isFileTypeSupported(
		AudioFileFormat.Type fileType,
		AudioInputStream audioInputStream)
	{
		IsFileTypeSupportedAudioFileWriterAction	action = new IsFileTypeSupportedAudioFileWriterAction(
			fileType,
			audioInputStream);
		try
		{
			doAudioFileWriterIteration(action);
		}
		catch (IOException e)
		{
			if (TDebug.TraceAllExceptions) { TDebug.out(e); }
		}
		return action.isSupported();
	}



	/**	AudioFileWriterAction for write().
	 */
	private static class WriteAudioFileWriterAction
	implements	AudioFileWriterAction
	{
		private AudioInputStream	m_audioInputStream;
		private AudioFileFormat.Type	m_fileType;
		private Object			m_destination;
		private int			m_nWritten;



		public WriteAudioFileWriterAction(
			AudioInputStream audioInputStream,
			AudioFileFormat.Type fileType,
			Object destination)
		{
			m_audioInputStream = audioInputStream;
			m_fileType = fileType;
			m_destination = destination;
			m_nWritten = -1;
		}



		// $$fb 2000-04-02: variable should be called "audioFileWriter" (too much copy&paste :)
		public boolean handleAudioFileWriter(
			AudioFileWriter audioFileWriter)
			throws	IOException
		{
			int	nWritten = -1;
			// $$fb 2000-04-02: need to check whether this audioFileWriter is actually
			//                  capable of handling this file type !
			if (TDebug.TraceAudioSystem) { TDebug.out(">AudioSystem.handleAudioFileWriter("+audioFileWriter.getClass().getName()+") checking for type "+m_fileType); }
			if (!audioFileWriter.isFileTypeSupported(m_fileType)) 
			{
				if (TDebug.TraceAudioSystem) { TDebug.out("< is not capable of handling this file type"); }
				return false;
			}
			try
			{
				if (m_destination instanceof OutputStream)
				{
					// $$fb 2000-04-02: s.a.
					nWritten = audioFileWriter.write(
						m_audioInputStream,
						m_fileType,
						(OutputStream) m_destination);
				}
				else if (m_destination instanceof File)
				{
					// $$fb 2000-04-02: s.a.
					nWritten = audioFileWriter.write(
						m_audioInputStream,
						m_fileType,
						(File) m_destination);
				}
				else
				{
					// TODO: debug message
				}
				m_nWritten = nWritten;
				if (TDebug.TraceAudioSystem) { TDebug.out("< wrote "+nWritten+" bytes"); }
				// interrupt the iteration
				return true;
			}
			catch (IllegalArgumentException e)
			{
				if (TDebug.TraceAudioSystem || TDebug.TraceAllExceptions) { TDebug.out(e); }
			}
			if (TDebug.TraceAudioSystem) { TDebug.out("< does not support this file type."); }
			// continue the iteration
			return false;
		}



		public int getWritten()
		{
			return m_nWritten;
		}
	}



	public static int write(AudioInputStream inputStream,
				AudioFileFormat.Type fileType,
				OutputStream outputStream)
		throws	IOException
	{
		return writeImpl(inputStream,
				 fileType,
				 outputStream);
	}



	public static int write(AudioInputStream inputStream,
				AudioFileFormat.Type fileType,
				File file)
		throws	IOException
	{
		return writeImpl(inputStream,
				 fileType,
				 file);
	}



	private static int writeImpl(AudioInputStream audioInputStream,
				     AudioFileFormat.Type fileType,
				     Object destination)
		throws	IOException
	{
		WriteAudioFileWriterAction	action = new WriteAudioFileWriterAction(
			audioInputStream,
			fileType,
			destination);
		doAudioFileWriterIteration(action);
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




	//////////////////////////////////////////////////////////////
	//
	//	auxiliary methods and interfaces
	//
	//////////////////////////////////////////////////////////////



	private static interface MixerProviderAction
	{
		public boolean handleMixerProvider(MixerProvider mixerProvider);
	}



	/**	Iterates over the available MixerProviders.
	 *	For each MixerProvider, the mathod handleMixerProvider() of
	 *	the passed MixerProviderAction is called with the MixerProvider
	 *	in question as the only argument.
	 */
	private static void doMixerProviderIteration(
		MixerProviderAction action)
	{
		Iterator	mixerProviders = TAudioConfig.getMixerProviders();
		boolean	bCompleted = false;
		if (TDebug.TraceAudioSystem) { TDebug.out(">AudioSystem.doMixerProviderIteration()"); }
		while (mixerProviders.hasNext() && ! bCompleted)
		{
			MixerProvider	mixerProvider = (MixerProvider) mixerProviders.next();
			if (TDebug.TraceAudioSystem) { TDebug.out("handling MixerProvider: " + mixerProvider); }
			bCompleted = action.handleMixerProvider(mixerProvider);
		}
		if (TDebug.TraceAudioSystem) { TDebug.out("< completed="+bCompleted); }
	}



	private static interface MixerAction
	{
		public boolean handleMixer(Mixer mixer);
	}



	/**	Iterates over the available MixerProviders.
	 *	For each MixerProvider, the mathod handleMixerProvider() of
	 *	the passed MixerProviderAction is called with the MixerProvider
	 *	in question as the only argument.
	 */
	private static void doMixerIteration(
		MixerAction action)
	{
		Mixer.Info[]	mixerInfos = getMixerInfo();
		boolean	bCompleted = false;
		if (TDebug.TraceAudioSystem) { TDebug.out(">AudioSystem.doMixerIteration()"); }
		for (int nMixer = 0; nMixer < mixerInfos.length && ! bCompleted; nMixer++)
		{
			Mixer	mixer = getMixer(mixerInfos[nMixer]);
			if (TDebug.TraceAudioSystem) { TDebug.out("handling Mixer: " + mixer); }
			bCompleted = action.handleMixer(mixer);
		}
		if (TDebug.TraceAudioSystem) { TDebug.out("< completed="+bCompleted); }
	}



	private static interface FormatConversionProviderAction
	{
		public boolean handleFormatConversionProvider(FormatConversionProvider formatConversionProvider);
		// throws	IOException;
	}



	/**	Iterates over the available FormatConversionProviders.
	 *	For each FormatConversionProvider, the mathod handleFormatConversionProvider() of
	 *	the passed FormatConversionProviderAction is called with the FormatConversionProvider
	 *	in question as the only argument.
	 */
	private static void doFormatConversionProviderIteration(
		FormatConversionProviderAction action)
		// throws	IOException
	{
		Iterator	formatConversionProviders = TAudioConfig.getFormatConversionProviders();
		boolean bCompleted = false;
		if (TDebug.TraceAudioSystem) { TDebug.out(">AudioSystem.doFormatConversionProviderIteration()"); }
		while (formatConversionProviders.hasNext() && ! bCompleted)
		{
			FormatConversionProvider	formatConversionProvider = (FormatConversionProvider) formatConversionProviders.next();
			if (TDebug.TraceAudioSystem) { TDebug.out("handling FormatConversionProvider: " + formatConversionProvider); }
			bCompleted = action.handleFormatConversionProvider(formatConversionProvider);
		}
		if (TDebug.TraceAudioSystem) { TDebug.out("< completed="+bCompleted); }
	}



	private static interface AudioFileReaderAction
	{
		public boolean handleAudioFileReader(AudioFileReader audioFileReader)
			throws	IOException;
	}



	/**	Iterates over the available AudioFileReaders.
	 *	For each AudioFileReader, the mathod handleAudioFileReader() of
	 *	the passed AudioFileReaderAction is called with the AudioFileReader
	 *	in question as the only argument.
	 */
	private static void doAudioFileReaderIteration(
		AudioFileReaderAction action)
		throws	IOException
	{
		Iterator	audioFileReaders = TAudioConfig.getAudioFileReaders();
		boolean bCompleted = false;
		if (TDebug.TraceAudioSystem) { TDebug.out(">AudioSystem.doAudioFileReaderIteration()"); }
		while (audioFileReaders.hasNext() && ! bCompleted)
		{
			AudioFileReader	audioFileReader = (AudioFileReader) audioFileReaders.next();
			if (TDebug.TraceAudioSystem) { TDebug.out("handling AudioFileReader: " + audioFileReader); }
			bCompleted = action.handleAudioFileReader(audioFileReader);
		}
		if (TDebug.TraceAudioSystem) { TDebug.out("< completed="+bCompleted); }
	}



	private static interface AudioFileWriterAction
	{
		public boolean handleAudioFileWriter(AudioFileWriter audioFileWriter)
			throws	IOException;
	}



	/**	Iterates over the available AudioFileWriters.
	 *	For each AudioFileWriter, the mathod handleAudioFileWriter() of
	 *	the passed AudioFileWriterAction is called with the AudioFileWriter
	 *	in question as the only argument.
	 */
	private static void doAudioFileWriterIteration(
		AudioFileWriterAction action)
		throws	IOException
	{
		Iterator	audioFileWriters = TAudioConfig.getAudioFileWriters();
		boolean bCompleted = false;
		if (TDebug.TraceAudioSystem) { TDebug.out(">AudioSystem.doAudioFileWriterIteration()"); }
		while (audioFileWriters.hasNext() && ! bCompleted)
		{
			AudioFileWriter	audioFileWriter = (AudioFileWriter) audioFileWriters.next();
			if (TDebug.TraceAudioSystem) { TDebug.out("handling AudioFileWriter: " + audioFileWriter); }
			bCompleted = action.handleAudioFileWriter(audioFileWriter);
		}
		if (TDebug.TraceAudioSystem) { TDebug.out("< completed="+bCompleted); }
	}
}



/*** AudioSystem.java ***/
