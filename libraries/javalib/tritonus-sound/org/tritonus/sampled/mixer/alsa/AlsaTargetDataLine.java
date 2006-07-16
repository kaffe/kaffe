/*
 *	AlsaTargetDataLine.java
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
import javax.sound.sampled.TargetDataLine;

import org.tritonus.lowlevel.alsa.Alsa;
import org.tritonus.lowlevel.alsa.AlsaPcm;
import org.tritonus.share.TDebug;
import org.tritonus.share.sampled.TConversionTool;



public class AlsaTargetDataLine
extends AlsaBaseDataLine
implements TargetDataLine
{
	private byte[]			m_abSwapBuffer;



	public AlsaTargetDataLine(AlsaDataLineMixer mixer, AudioFormat format, int nBufferSize)
		throws	LineUnavailableException
	{
		// TODO: use an info object that represents the mixer's capabilities (all possible formats for the line)
		super(mixer,
		      new DataLine.Info(TargetDataLine.class,
					format,
					nBufferSize)/*,
						    // TODO: has info object to change if format or buffer size are changed later?
						    format, nBufferSize*/);
	}


	protected int getAlsaStreamType()
	{
		return AlsaPcm.SND_PCM_STREAM_CAPTURE;
	}





/*
  public void start()
  {
  // getAlsaPcm().goCapture();
  setStarted(true);
  setActive(true);
  if (TDebug.TraceSourceDataLine)
  {
  TDebug.out("AlsaTargetDataLine.start(): channel started.");
  }
  }
*/



	protected void stopImpl()
	{
		if (TDebug.TraceTargetDataLine)
		{
			TDebug.out("AlsaTargetDataLine.stopImpl(): called.");
		}
		int	nReturn = 0; //getAlsaPcm().flushChannel(AlsaPcm.SND_PCM_CHANNEL_CAPTURE);
		if (nReturn != 0)
		{
			TDebug.out("flushChannel: " + Alsa.getStringError(nReturn));
		}
	}



	public int available()
	{
		// TODO:
		return -1;
	}




	// TODO: check if should block
	public int read(byte[] abData, int nOffset, int nLength)
	{
		if (TDebug.TraceTargetDataLine)
		{
			TDebug.out("AlsaTargetDataLine.read(): called.");
			TDebug.out("AlsaTargetDataLine.read(): wanted length: " + nLength);
		}
		int	nOriginalOffset = nOffset;
		if (nLength > 0 && !isActive())
		{
			start();
		}
		if (!isOpen())
		{
			if (TDebug.TraceTargetDataLine)
			{
				TDebug.out("AlsaTargetDataLine.read(): stream closed");
			}
		}
		int	nBytesRead = readImpl(abData, nOffset, nLength);
		if (TDebug.TraceTargetDataLine)
		{
			TDebug.out("AlsaTargetDataLine.read(): read (bytes): " + nBytesRead);
		}
		if (getSwapBytes() && nBytesRead > 0)
		{
			TConversionTool.swapOrder16(abData, nOriginalOffset, nBytesRead / 2);
		}
		return nBytesRead;
	}





	// TODO: check if should block
	public int readImpl(byte[] abData, int nOffset, int nLength)
	{
		if (TDebug.TraceTargetDataLine)
		{
			TDebug.out("AlsaTargetDataLine.readImpl(): called.");
			TDebug.out("AlsaTargetDataLine.readImpl(): wanted length: " + nLength);
		}
		int	nFrameSize = getFormat().getFrameSize();
		int	nOriginalOffset = nOffset;
		int	nFramesToRead = nLength / nFrameSize;
		if (nLength > 0 && !isActive())
		{
			start();
		}
		if (!isOpen())
		{
			if (TDebug.TraceTargetDataLine) { TDebug.out("AlsaTargetDataLine.readImpl(): stream closed"); }
		}
		int	nFramesRead = (int) getAlsaPcm().readi(abData, nOffset, nFramesToRead);
		if (nFramesRead < 0)
		{
			TDebug.out("AlsaTargetDataLine.readImpl(): " + Alsa.getStringError(nFramesRead));
		}
		int	nBytesRead = nFramesRead * nFrameSize;
		if (TDebug.TraceTargetDataLine)
		{
			TDebug.out("AlsaTargetDataLine.readImpl(): read (bytes): " + nBytesRead);
		}
		return nBytesRead;
	}




	public void drain()
	{
		// TODO:
	}



	public void flush()
	{
		// TODO:
	}



	public long getPosition()
	{
		// TODO:
		return 0;
	}



	/**
	 *	fGain is logarithmic!!
	 */
	private void setGain(float fGain)
	{
	}



	public class AlsaTargetDataLineGainControl
	extends		FloatControl
	{
		// private float		m_fGain;
		// private boolean		m_bMuted;



		/*package*/ AlsaTargetDataLineGainControl()
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
				AlsaTargetDataLine.this.setGain(getValue());
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
  AlsaTargetDataLine.this.setGain(getMinimum());
  }
  else
  {
  AlsaTargetDataLine.this.setGain(getGain());
  }
  }
  }
*/


		}
	}



/*** AlsaTargetDataLine.java ***/
