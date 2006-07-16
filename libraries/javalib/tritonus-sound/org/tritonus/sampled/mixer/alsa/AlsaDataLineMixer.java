/*
 *	AlsaDataLineMixer.java
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


package	org.tritonus.sampled.mixer.alsa;


import java.util.ArrayList;
import java.util.Collection;
import java.util.List;

import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.Clip;
import javax.sound.sampled.DataLine;
import javax.sound.sampled.Line;
import javax.sound.sampled.LineUnavailableException;
import javax.sound.sampled.Mixer;
import javax.sound.sampled.SourceDataLine;
import javax.sound.sampled.TargetDataLine;

import org.tritonus.lowlevel.alsa.Alsa;
import org.tritonus.lowlevel.alsa.AlsaPcm;
import org.tritonus.lowlevel.alsa.AlsaPcmHWParams;
import org.tritonus.lowlevel.alsa.AlsaPcmHWParamsFormatMask;
import org.tritonus.share.GlobalInfo;
import org.tritonus.share.TDebug;
import org.tritonus.share.TSettings;
import org.tritonus.share.sampled.mixer.TMixer;
import org.tritonus.share.sampled.mixer.TMixerInfo;
import org.tritonus.share.sampled.mixer.TSoftClip;



public class AlsaDataLineMixer
	extends		TMixer
{
	private static final AudioFormat[]	EMPTY_AUDIOFORMAT_ARRAY = new AudioFormat[0];
	private static final int	CHANNELS_LIMIT = 32;

	// default buffer size in bytes.
	private static final int	DEFAULT_BUFFER_SIZE = 32768;

	/*	The name of the sound card this mixer is representing.
	 */
	private String	m_strPcmName;



	public static String getDeviceNamePrefix()
	{
		if (TSettings.AlsaUsePlughw)
		{
			return "plughw";
		}
		else
		{
			return "hw";
		}
	}


	public static String getPcmName(int nCard)
	{
		String	strPcmName = getDeviceNamePrefix()
			+ ":" + nCard;
		if (TSettings.AlsaUsePlughw)
		{
			// strPcmName += ",0";
		}
		return strPcmName;
	}



	public AlsaDataLineMixer()
	{
		this(0);
	}



	public AlsaDataLineMixer(int nCard)
	{
		this(getPcmName(nCard));
	}



	public AlsaDataLineMixer(String strPcmName)
	{
		super(new TMixerInfo(
			"Alsa DataLine Mixer (" + strPcmName + ")",
			GlobalInfo.getVendor(),
			"Mixer for the Advanced Linux Sound Architecture (card " + strPcmName + ")",
			GlobalInfo.getVersion()),
		      new Line.Info(Mixer.class));
		if (TDebug.TraceMixer) { TDebug.out("AlsaDataLineMixer.<init>(String): begin."); }
		m_strPcmName = strPcmName;
		List	sourceFormats = getSupportedFormats(AlsaPcm.SND_PCM_STREAM_PLAYBACK);
		List	targetFormats = getSupportedFormats(AlsaPcm.SND_PCM_STREAM_CAPTURE);
		List	sourceLineInfos = new ArrayList();
		Line.Info	sourceLineInfo = new DataLine.Info(
			SourceDataLine.class,
			(AudioFormat[]) sourceFormats.toArray(EMPTY_AUDIOFORMAT_ARRAY),
			AudioSystem.NOT_SPECIFIED,
			AudioSystem.NOT_SPECIFIED);
		sourceLineInfos.add(sourceLineInfo);
		List	targetLineInfos = new ArrayList();
		Line.Info	targetLineInfo = new DataLine.Info(
			TargetDataLine.class,
			(AudioFormat[]) targetFormats.toArray(EMPTY_AUDIOFORMAT_ARRAY),
			AudioSystem.NOT_SPECIFIED,
			AudioSystem.NOT_SPECIFIED);
		targetLineInfos.add(targetLineInfo);
		setSupportInformation(sourceFormats,
				      targetFormats,
				      sourceLineInfos,
				      targetLineInfos);
		if (TDebug.TraceMixer) { TDebug.out("AlsaDataLineMixer.<init>(String): end."); }
	}



	public String getPcmName()
	{
		return m_strPcmName;
	}



	//////////////// Line //////////////////////////////////////


	// TODO: allow real close and reopen of mixer
	public void open()
	{
		if (TDebug.TraceMixer) { TDebug.out("AlsaDataLineMixer.open(): begin"); }

		// currently does nothing

		if (TDebug.TraceMixer) { TDebug.out("AlsaDataLineMixer.open(): end"); }
	}



	public void close()
	{
		if (TDebug.TraceMixer) { TDebug.out("AlsaDataLineMixer.close(): begin"); }

		// currently does nothing

		if (TDebug.TraceMixer) { TDebug.out("AlsaDataLineMixer.close(): end"); }
	}





	//////////////// Mixer //////////////////////////////////////


	public int getMaxLines(Line.Info info)
	{
		if (TDebug.TraceMixer) { TDebug.out("AlsaDataLineMixer.getMaxLines(): begin"); }
		// TODO:
		if (TDebug.TraceMixer) { TDebug.out("AlsaDataLineMixer.getMaxLines(): end"); }
		return 0;
	}




	//////////////// private //////////////////////////////////////


	// nBufferSize is in bytes!
	protected SourceDataLine getSourceDataLine(AudioFormat format, int nBufferSize)
		throws	LineUnavailableException
	{
		if (TDebug.TraceMixer) { TDebug.out("AlsaDataLineMixer.getSourceDataLine(): begin"); }
		if (TDebug.TraceMixer)
		{
			TDebug.out("AlsaDataLineMixer.getSourceDataLine(): format: " + format);
			TDebug.out("AlsaDataLineMixer.getSourceDataLine(): buffer size: " + nBufferSize);
		}
		if (nBufferSize < 1)
		{
			nBufferSize = DEFAULT_BUFFER_SIZE;
		}
		AlsaSourceDataLine	sourceDataLine = new AlsaSourceDataLine(this, format, nBufferSize);
		// sourceDataLine.start();
		if (TDebug.TraceMixer) { TDebug.out("AlsaDataLineMixer.getSourceDataLine(): returning: " + sourceDataLine); }
		if (TDebug.TraceMixer) { TDebug.out("AlsaDataLineMixer.getSourceDataLine(): end"); }
		return sourceDataLine;
	}


	// nBufferSize is in bytes!
	protected TargetDataLine getTargetDataLine(AudioFormat format, int nBufferSize)
		throws	LineUnavailableException
	{
		if (TDebug.TraceMixer) { TDebug.out("AlsaDataLineMixer.getTargetDataLine(): begin"); }
		int			nBufferSizeInBytes = nBufferSize * format.getFrameSize();
		AlsaTargetDataLine	targetDataLine = new AlsaTargetDataLine(this, format, nBufferSizeInBytes);
		// targetDataLine.start();
		if (TDebug.TraceMixer) { TDebug.out("AlsaDataLineMixer.getTargetDataLine(): returning: " + targetDataLine); }
		if (TDebug.TraceMixer) { TDebug.out("AlsaDataLineMixer.getTargetDataLine(): end"); }
		return targetDataLine;
	}



	protected Clip getClip(AudioFormat format)
		throws	LineUnavailableException
	{
		if (TDebug.TraceMixer) { TDebug.out("AlsaDataLineMixer.getClip(): begin"); }
		Clip	clip = new TSoftClip(this, format);
		if (TDebug.TraceMixer) { TDebug.out("AlsaDataLineMixer.getClip(): end"); }
		return clip;
	}



	/*
	  nDirection: should be AlsaPcm.SND_PCM_STREAM_PLAYBACK or
	  AlsaPcm.SND_PCM_STREAM_CAPTURE.
	 */
	private List getSupportedFormats(int nDirection)
	{
		if (TDebug.TraceMixer) { TDebug.out("AlsaDataLineMixer.getSupportedFormats(): begin"); }
		if (TDebug.TraceMixer) { TDebug.out("AlsaDataLineMixer.getSupportedFormats(): direction: " + nDirection); }
		List	supportedFormats = new ArrayList();
		AlsaPcm	alsaPcm = null;
		try
		{
			alsaPcm = new AlsaPcm(
				getPcmName(),
				nDirection,
				0);	// no special mode
		}
		catch (Exception e)
		{
			if (TDebug.TraceAllExceptions) { TDebug.out(e); }
			throw new RuntimeException("cannot open pcm");
		}
		int	nReturn;
		AlsaPcmHWParams	hwParams = new AlsaPcmHWParams();
		nReturn = alsaPcm.getAnyHWParams(hwParams);
		if (nReturn != 0)
		{
			TDebug.out("AlsaDataLineMixer.getSupportedFormats(): getAnyHWParams(): " + Alsa.getStringError(nReturn));
			throw new RuntimeException(Alsa.getStringError(nReturn));
		}
		AlsaPcmHWParamsFormatMask	formatMask = new AlsaPcmHWParamsFormatMask();
		int	nMinChannels = hwParams.getChannelsMin();
		if (TDebug.TraceMixer) { TDebug.out("AlsaDataLineMixer.getSupportedFormats(): min channels: " + nMinChannels); }
		int	nMaxChannels = hwParams.getChannelsMax();
		nMaxChannels = Math.min(nMaxChannels, CHANNELS_LIMIT);
		if (TDebug.TraceMixer) { TDebug.out("AlsaDataLineMixer.getSupportedFormats(): max channels: " + nMaxChannels); }
		hwParams.getFormatMask(formatMask);
		for (int i = 0; i < 32; i++)
		{
			if (TDebug.TraceMixer) { TDebug.out("AlsaDataLineMixer.getSupportedFormats(): checking ALSA format index: " + i); }
			if (formatMask.test(i))
			{
				if (TDebug.TraceMixer) { TDebug.out("AlsaDataLineMixer.getSupportedFormats(): ...supported"); }
				AudioFormat	audioFormat = AlsaUtils.getAlsaFormat(i);
				if (TDebug.TraceMixer) { TDebug.out("AlsaDataLineMixer.getSupportedFormats(): adding AudioFormat: " + audioFormat); }
				addChanneledAudioFormats(supportedFormats, audioFormat, nMinChannels, nMaxChannels);
				// supportedFormats.add(audioFormat);
			}
			else
			{
			if (TDebug.TraceMixer) { TDebug.out("AlsaDataLineMixer.getSupportedFormats(): ...not supported"); }
			}
		}
		// TODO: close/free mask & hwParams?
		alsaPcm.close();
		if (TDebug.TraceMixer) { TDebug.out("AlsaDataLineMixer.getSupportedFormats(): end"); }
		return supportedFormats;
	}



	private static void addChanneledAudioFormats(
		Collection collection,
		AudioFormat protoAudioFormat,
		int nMinChannels,
		int nMaxChannels)
	{
		if (TDebug.TraceMixer) { TDebug.out("AlsaDataLineMixer.addChanneledAudioFormats(): begin"); }
		for (int nChannels = nMinChannels; nChannels <= nMaxChannels; nChannels++)
		{
			AudioFormat	channeledAudioFormat = getChanneledAudioFormat(protoAudioFormat, nChannels);
			if (TDebug.TraceMixer) { TDebug.out("AlsaDataLineMixer.addChanneledAudioFormats(): adding AudioFormat: " + channeledAudioFormat); }
			collection.add(channeledAudioFormat);
		}
		if (TDebug.TraceMixer) { TDebug.out("AlsaDataLineMixer.addChanneledAudioFormats(): end"); }
	}



	// TODO: better name
	// TODO: calculation of frame size is not perfect
	private static AudioFormat getChanneledAudioFormat(AudioFormat audioFormat, int nChannels)
	{
		AudioFormat	channeledAudioFormat = new AudioFormat(
			audioFormat.getEncoding(),
			audioFormat.getSampleRate(),
			audioFormat.getSampleSizeInBits(),
			nChannels,
			(audioFormat.getSampleSizeInBits() / 8) * nChannels,
			audioFormat.getFrameRate(),
			audioFormat.isBigEndian());
		return channeledAudioFormat;
	}
}



/*** AlsaDataLineMixer.java ***/
