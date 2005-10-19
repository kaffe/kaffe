/*
 *	AlsaBaseDataLine.java
 *
 *	This file is part of Tritonus.
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


import java.util.Collection;

import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.DataLine;
import javax.sound.sampled.FloatControl;
import javax.sound.sampled.LineUnavailableException;

import org.tritonus.lowlevel.alsa.Alsa;
import org.tritonus.lowlevel.alsa.AlsaPcm;
import org.tritonus.lowlevel.alsa.AlsaPcmHWParams;
import org.tritonus.lowlevel.alsa.AlsaPcmSWParams;
import org.tritonus.share.TDebug;
import org.tritonus.share.sampled.mixer.TBaseDataLine;




public abstract class AlsaBaseDataLine
extends TBaseDataLine
{
	// private static final Class[]	CONTROL_CLASSES = {GainControl.class};


	private AlsaPcm			m_alsaPcm;
	private boolean			m_bSwapBytes;

	/*
	 *	Only used if m_bSwapBytes is true.
	 */
	private int			m_nBytesPerSample;



	public AlsaBaseDataLine(AlsaDataLineMixer mixer, DataLine.Info info)
		throws	LineUnavailableException
	{
		super(mixer,
		      info);
		if (TDebug.TraceSourceDataLine) { TDebug.out("AlsaBaseDataLine.<init>(): begin"); }
		if (TDebug.TraceSourceDataLine) { TDebug.out("AlsaBaseDataLine.<init>(): end"); }
	}



	public AlsaBaseDataLine(AlsaDataLineMixer mixer, DataLine.Info info,
				Collection controls)
		throws	LineUnavailableException
	{
		super(mixer,
		      info);
		if (TDebug.TraceSourceDataLine) { TDebug.out("AlsaBaseDataLine.<init>(): begin"); }
		if (TDebug.TraceSourceDataLine) { TDebug.out("AlsaBaseDataLine.<init>(): end"); }
	}



	protected AlsaDataLineMixer getAlsaDataLineMixer()
	{
		return (AlsaDataLineMixer) getMixer();
	}



	protected AlsaPcm getAlsaPcm()
	{
		return m_alsaPcm;
	}



	/**	Returns the ALSA stream type of this line.
		Subclasses must implement this method to return either
		AlsaPcm.SND_PCM_STREAM_PLAYBACK or
		AlsaPcm.SND_PCM_STREAM_CAPTURE.
		The return value is used by this class to decide if it has
		to deal with a source (PLAYBACK) or target (CAPTURE)
		data line.
	*/
	protected abstract int getAlsaStreamType();



	protected boolean getSwapBytes()
	{
		return m_bSwapBytes;
	}



	protected int getBytesPerSample()
	{
		return m_nBytesPerSample;
	}



	protected void openImpl()
		throws	LineUnavailableException
	{
		if (TDebug.TraceSourceDataLine) { TDebug.out("AlsaBaseDataLine.openImpl(): begin"); }

		/*
		 *	Checks that a format is set.
		 *	Sets the buffer size to a default value if not
		 *	already set.
		 */
		checkOpen();
		AudioFormat	format = getFormat();
		if (TDebug.TraceSourceDataLine) { TDebug.out("AlsaBaseDataLine.openImpl(): input format: " + format); }

		// hack, only true for pmac
		boolean			bHWBigEndian = false;

		AudioFormat.Encoding	encoding  = format.getEncoding();
		boolean			bBigEndian = format.isBigEndian();
		m_bSwapBytes = false;
		if (format.getSampleSizeInBits() == 16 && bBigEndian != bHWBigEndian)
		{
			m_bSwapBytes = true;
			bBigEndian = bHWBigEndian;
		}
		else if (format.getSampleSizeInBits() == 8 &&
			 encoding.equals(AudioFormat.Encoding.PCM_SIGNED))
		{
			m_bSwapBytes = true;
			encoding = AudioFormat.Encoding.PCM_UNSIGNED;
		}
		if (getSwapBytes())
		{
			format = new AudioFormat(encoding,
						 format.getSampleRate(),
						 format.getSampleSizeInBits(),
						 format.getChannels(),
						 format.getFrameSize(),
						 format.getFrameRate(),
						 bBigEndian);
			if (TDebug.TraceSourceDataLine) { TDebug.out("AlsaBaseDataLine.openImpl(): output format: " + format); }
			m_nBytesPerSample = format.getFrameSize() / format.getChannels();
		}
		int	nAlsaOutFormat = AlsaUtils.getAlsaFormat(format);
		if (TDebug.TraceSourceDataLine) { TDebug.out("AlsaBaseDataLine.openImpl(): ALSA output format: " + nAlsaOutFormat); }
		if (nAlsaOutFormat == AlsaPcm.SND_PCM_FORMAT_UNKNOWN)
		{
			throw new IllegalArgumentException("unsupported format");
		}

		try
		{
			m_alsaPcm = new AlsaPcm(
				getAlsaDataLineMixer().getPcmName(),
				getAlsaStreamType(),
				0);	// no special mode
		}
		catch (Exception e)
		{
			if (TDebug.TraceAllExceptions) { TDebug.out(e); }
			throw new LineUnavailableException();
		}
		int	nReturn;
		AlsaPcmHWParams	hwParams = new AlsaPcmHWParams();
		nReturn = m_alsaPcm.getAnyHWParams(hwParams);
		if (nReturn != 0)
		{
			TDebug.out("AlsaBaseDataLine.openImpl(): getAnyHWParams(): " + Alsa.getStringError(nReturn));
			throw new LineUnavailableException(Alsa.getStringError(nReturn));
		}
		nReturn = m_alsaPcm.setHWParamsAccess(hwParams, AlsaPcm.SND_PCM_ACCESS_RW_INTERLEAVED);
		if (nReturn != 0)
		{
			TDebug.out("AlsaBaseDataLine.openImpl(): setHWParamsFormat(): " + Alsa.getStringError(nReturn));
			throw new LineUnavailableException(Alsa.getStringError(nReturn));
		}
		nReturn = m_alsaPcm.setHWParamsFormat(hwParams, nAlsaOutFormat);
		if (nReturn != 0)
		{
			TDebug.out("AlsaBaseDataLine.openImpl(): setHWParamsFormat(): " + Alsa.getStringError(nReturn));
			throw new LineUnavailableException(Alsa.getStringError(nReturn));
		}
		nReturn = m_alsaPcm.setHWParamsChannels(hwParams, format.getChannels());
		if (nReturn != 0)
		{
			TDebug.out("AlsaBaseDataLine.openImpl(): setHWParamsChannels(): " + Alsa.getStringError(nReturn));
			throw new LineUnavailableException(Alsa.getStringError(nReturn));
		}
		nReturn = m_alsaPcm.setHWParamsRateNear(hwParams, (int) format.getSampleRate());
// int nRate = nReturn;
		if (nReturn < 0)
		{
			TDebug.out("AlsaBaseDataLine.openImpl(): setHWParamsRateNear(): " + Alsa.getStringError(nReturn));
			throw new LineUnavailableException(Alsa.getStringError(nReturn));
		}
		nReturn = m_alsaPcm.setHWParamsBufferTimeNear(hwParams, 500000);
		int	nBufferTime = nReturn;
		if (nReturn < 0)
		{
			TDebug.out("AlsaBaseDataLine.openImpl(): setHWParamsBufferTimeNear(): " + Alsa.getStringError(nReturn));
			throw new LineUnavailableException(Alsa.getStringError(nReturn));
		}
		nReturn = m_alsaPcm.setHWParamsPeriodTimeNear(hwParams, nBufferTime / 4);
// int nPeriodTime = nReturn;
		if (nReturn < 0)
		{
			TDebug.out("AlsaBaseDataLine.openImpl(): setHWParamsPeriodTimeNear(): " + Alsa.getStringError(nReturn));
			throw new LineUnavailableException(Alsa.getStringError(nReturn));
		}
		nReturn = m_alsaPcm.setHWParams(hwParams);
		if (nReturn < 0)
		{
			TDebug.out("AlsaBaseDataLine.openImpl(): setHWParams(): " + Alsa.getStringError(nReturn));
			throw new LineUnavailableException(Alsa.getStringError(nReturn));
		}
		int	nChunkSize = hwParams.getPeriodSize(null);
		int	nBufferSize = hwParams.getBufferSize();
		if (nChunkSize == nBufferSize)
		{
			throw new LineUnavailableException("period size is equal to buffer size");
		}

		AlsaPcmSWParams	swParams = new AlsaPcmSWParams();
		nReturn = m_alsaPcm.getSWParams(swParams);
		if (nReturn != 0)
		{
			TDebug.out("AlsaBaseDataLine.openImpl(): getSWParams(): " + Alsa.getStringError(nReturn));
			throw new LineUnavailableException(Alsa.getStringError(nReturn));
		}
		nReturn = m_alsaPcm.setSWParamsSleepMin(swParams, 0);
		if (nReturn != 0)
		{
			TDebug.out("AlsaBaseDataLine.openImpl(): setSWParamsSleepMin(): " + Alsa.getStringError(nReturn));
			throw new LineUnavailableException(Alsa.getStringError(nReturn));
		}
		nReturn = m_alsaPcm.setSWParamsXrunMode(swParams, AlsaPcm.SND_PCM_XRUN_NONE);
		if (nReturn != 0)
		{
			TDebug.out("AlsaBaseDataLine.openImpl(): setSWParamsXrunMode(): " + Alsa.getStringError(nReturn));
			throw new LineUnavailableException(Alsa.getStringError(nReturn));
		}
		nReturn = m_alsaPcm.setSWParamsAvailMin(swParams, nChunkSize);
		if (nReturn != 0)
		{
			TDebug.out("AlsaBaseDataLine.openImpl(): setSWParamsAvailMin(): " + Alsa.getStringError(nReturn));
			throw new LineUnavailableException(Alsa.getStringError(nReturn));
		}
		long	lStartThreshold = (long) ((double) format.getFrameRate() * 1 / 1000000);
		nReturn = m_alsaPcm.setSWParamsStartThreshold(swParams, (int) lStartThreshold);
		if (nReturn != 0)
		{
			TDebug.out("AlsaBaseDataLine.openImpl(): setSWParamsStartThreshold(): " + Alsa.getStringError(nReturn));
			throw new LineUnavailableException(Alsa.getStringError(nReturn));
		}
		long	lStopThreshold = (long) (nBufferSize + (double) format.getFrameRate() * 0 / 1000000);
		nReturn = m_alsaPcm.setSWParamsStopThreshold(swParams, (int) lStopThreshold);
		if (nReturn != 0)
		{
			TDebug.out("AlsaBaseDataLine.openImpl(): setSWParamsStopThreshold(): " + Alsa.getStringError(nReturn));
			throw new LineUnavailableException(Alsa.getStringError(nReturn));
		}
// omitted: xfer_align
		nReturn = m_alsaPcm.setSWParams(swParams);
		if (nReturn != 0)
		{
			TDebug.out("AlsaBaseDataLine.openImpl(): setSWParams(): " + Alsa.getStringError(nReturn));
			throw new LineUnavailableException(Alsa.getStringError(nReturn));
		}
		if (TDebug.TraceSourceDataLine) { TDebug.out("AlsaBaseDataLine.openImpl(): end"); }
	}



	protected void closeImpl()
	{
		if (TDebug.TraceSourceDataLine) { TDebug.out("AlsaBaseDataLine.closeImpl(): begin"); }
		m_alsaPcm.close();
		if (TDebug.TraceSourceDataLine) { TDebug.out("AlsaBaseDataLine.closeImpl(): end"); }
	}



/*
  public void start()
  {
  setStarted(true);
  setActive(true);
  if (TDebug.TraceSourceDataLine) { TDebug.out("AlsaBaseDataLine.start(): channel started."); }
  }
*/


	protected void stopImpl()
	{
		if (TDebug.TraceSourceDataLine) { TDebug.out("AlsaBaseDataLine.stopImpl(): called"); }
		int	nReturn = 0;
		// int	nReturn = m_alsaPcm.flushChannel(AlsaPcm.SND_PCM_CHANNEL_PLAYBACK);
		if (nReturn != 0)
		{
			TDebug.out("flushChannel: " + Alsa.getStringError(nReturn));
		}
		// setStarted(false);
	}



	public int available()
	{
		// TODO:
		return -1;
	}







	public void drain()
	{
		// TODO:
	}



	public void flush()
	{
		// TODO:
	}



	/**
	 *	dGain is logarithmic!!
	 */
	private void setGain(float dGain)
	{
	}



	// IDEA: move inner classes to TBaseDataLine
	public class AlsaBaseDataLineGainControl
		extends		FloatControl
	{
		// private float		m_fGain;
		// private boolean		m_bMuted;



		/*package*/ AlsaBaseDataLineGainControl()
		{
			super(FloatControl.Type.VOLUME,	// or MASTER_GAIN ?
			      -96.0F,	// MIN_GAIN,
			      24.0F,	// MAX_GAIN,
			      0.01F,	// precision
			      0,	// update period?
			      0.0F,	// initial value
			      "dB",
			      "-96.0",
			      "",
			      "+24.0");
			// m_bMuted = false;	// should be included in a compund control?
		}



		public void setValue(float fGain)
		{
			fGain = Math.max(Math.min(fGain, getMaximum()), getMinimum());
			if (Math.abs(fGain - getValue()) > 1.0E9)
			{
				super.setValue(fGain);
				// if (!getMute())
				// {
				AlsaBaseDataLine.this.setGain(getValue());
				// }
			}
		}


/*
  public float getMaximum()
  {
  return MAX_GAIN;
  }



  public float getMinimum()
  {
  return MIN_GAIN;
  }



  public int getIncrements()
  {
  // TODO: check this value
  return GAIN_INCREMENTS;
  }



  public void fade(float fInitialGain, float fFinalGain, int nFrames)
  {
  // TODO:
  }



  public int getFadePrecision()
  {
  //TODO:
  return -1;
  }



  public boolean getMute()
  {
  return m_bMuted;
  }



  public void setMute(boolean bMuted)
  {
  if (bMuted != getMute())
  {
  m_bMuted = bMuted;
  if (getMute())
  {
  AlsaBaseDataLine.this.setGain(getMinimum());
  }
  else
  {
  AlsaBaseDataLine.this.setGain(getGain());
  }
  }
  }
*/


		}
	}



/*** AlsaBaseDataLine.java ***/
