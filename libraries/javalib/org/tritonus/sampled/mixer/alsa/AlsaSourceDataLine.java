/*
 *	AlsaSourceDataLine.java
 *
 *	This file is part of Tritonus.
 */

/*
 *  Copyright (c) 1999 - 2001 by Matthias Pfisterer <Matthias.Pfisterer@gmx.de>
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

package	org.tritonus.sampled.mixer.alsa;

import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.DataLine;
import javax.sound.sampled.FloatControl;
import javax.sound.sampled.LineUnavailableException;
import javax.sound.sampled.SourceDataLine;

import org.tritonus.lowlevel.alsa.Alsa;
import org.tritonus.lowlevel.alsa.AlsaPcm;
import org.tritonus.share.TDebug;
import org.tritonus.share.sampled.TConversionTool;




public class AlsaSourceDataLine
extends AlsaBaseDataLine
implements SourceDataLine
{
	// private static final Class[]	CONTROL_CLASSES = {GainControl.class};

	private byte[]			m_abSwapBuffer;



	// TODO: has info object to change if format or buffer size are changed later?
	// no, but it has to represent the mixer's capabilities. So a fixed info per mixer.
	public AlsaSourceDataLine(AlsaDataLineMixer mixer, AudioFormat format, int nBufferSize)
		throws	LineUnavailableException
	{
		super(mixer,
		      new DataLine.Info(SourceDataLine.class,
					format,
					nBufferSize));
		if (TDebug.TraceSourceDataLine) { TDebug.out("AlsaSourceDataLine.<init>(): begin"); }
		if (TDebug.TraceSourceDataLine) { TDebug.out("AlsaSourceDataLine.<init>(): buffer size: " + nBufferSize); }
		if (TDebug.TraceSourceDataLine) { TDebug.out("AlsaSourceDataLine.<init>(): end"); }
	}



	protected int getAlsaStreamType()
	{
		return AlsaPcm.SND_PCM_STREAM_PLAYBACK;
	}



/*
  public void start()
  {
  setStarted(true);
  setActive(true);
  if (TDebug.TraceSourceDataLine) { TDebug.out("AlsaSourceDataLine.start(): channel started."); }
  }
*/


	protected void stopImpl()
	{
		if (TDebug.TraceSourceDataLine) { TDebug.out("AlsaSourceDataLine.stopImpl(): called"); }
		int	nReturn = 0;
		// int	nReturn = getAlsaPcm().flushChannel(AlsaPcm.SND_PCM_CHANNEL_PLAYBACK);
		if (nReturn != 0)
		{
			TDebug.out("flushChannel: " + Alsa.getStringError(nReturn));
		}
		// setStarted(false);
	}



	public int available()
	{
		// TODO:
		throwNYIException();
		return -1;
	}




	// TODO: check if should block
	public int write(byte[] abData, int nOffset, int nLength)
	{
		if (TDebug.TraceSourceDataLine) { TDebug.out("AlsaSourceDataLine.write(): begin"); }
		if (getSwapBytes())
		{
			if (m_abSwapBuffer == null || m_abSwapBuffer.length < nOffset + nLength)
			{
				m_abSwapBuffer = new byte[nOffset + nLength];
			}
			TConversionTool.changeOrderOrSign(
				abData, nOffset,
				m_abSwapBuffer, nOffset,
				nLength, getBytesPerSample());
			abData = m_abSwapBuffer;
		}
		int nReturn = writeImpl(abData, nOffset, nLength);
		if (TDebug.TraceSourceDataLine) { TDebug.out("AlsaSourceDataLine.write(): end"); }
		return nReturn;
	}






	/** Write data to the line.

	@param abData The buffer to use.

	@param nOffset

	@param nLength The length of the data that should be written,
	in bytes. Can be less that the length of abData.

	@return The number of bytes written. May be less than nLength.

	*/

	// TODO: check if should block
	private int writeImpl(byte[] abData, int nOffset, int nLength)
	{
		if (TDebug.TraceSourceDataLine) { TDebug.out("AlsaSourceDataLine.writeImpl(): begin"); }
		if (nLength > 0 && !isActive())
		{
			start();
		}
		int		nFrameSize = getFormat().getFrameSize();
		int		nRemaining = nLength;
		while (nRemaining > 0 && isOpen())
		{
			synchronized (this)
			{
				if (!isOpen())
				{
					return nLength - nRemaining;
				}
				if (TDebug.TraceSourceDataLine) { TDebug.out("AlsaSourceDataLine.writeImpl(): trying to write (bytes): " + nRemaining); }
				int	nRemainingFrames = nRemaining / nFrameSize;
				if (TDebug.TraceSourceDataLine) { TDebug.out("AlsaSourceDataLine.writeImpl(): trying to write (frames): " + nRemainingFrames); }
				int	nWrittenFrames = (int) getAlsaPcm().writei(abData, nOffset, nRemainingFrames);
				if (nWrittenFrames < 0)
				{
					TDebug.out("AlsaSourceDataLine.writeImpl(): " + Alsa.getStringError(nWrittenFrames));
					return nLength - nRemaining;
				}
				if (TDebug.TraceSourceDataLine) { TDebug.out("AlsaSourceDataLine.writeImpl(): written (frames): " + nWrittenFrames); }
				int	nWrittenBytes = nWrittenFrames * nFrameSize;
				if (TDebug.TraceSourceDataLine) { TDebug.out("AlsaSourceDataLine.writeImpl(): written (bytes): " + nWrittenBytes); }
				nOffset += nWrittenBytes;
				nRemaining -= nWrittenBytes;
			}
		}
		return nLength;
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



	/** Throw a RuntimeException saying "not yet implemented".
	 */
	private void throwNYIException()
	{
		throw new RuntimeException("sorry, this feature is not yet implemented");
	}


	// IDEA: move inner classes to TBaseDataLine
	public class AlsaSourceDataLineGainControl
		extends		FloatControl
	{
		// private float		m_fGain;
		// private boolean		m_bMuted;



		/*package*/ AlsaSourceDataLineGainControl()
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
				AlsaSourceDataLine.this.setGain(getValue());
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
  AlsaSourceDataLine.this.setGain(getMinimum());
  }
  else
  {
  AlsaSourceDataLine.this.setGain(getGain());
  }
  }
  }
*/


		}
	}



/*** AlsaSourceDataLine.java ***/
