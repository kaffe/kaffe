/*
 *	EsdTargetDataLine.java
 */

/*
 *  Copyright (c) 1999, 2000 by Matthias Pfisterer <Matthias.Pfisterer@gmx.de>
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


package	org.tritonus.sampled.mixer.esd;


import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.DataLine;
import javax.sound.sampled.FloatControl;
import javax.sound.sampled.LineUnavailableException;
import javax.sound.sampled.TargetDataLine;

import org.tritonus.lowlevel.esd.Esd;
import org.tritonus.lowlevel.esd.EsdRecordingStream;
import org.tritonus.share.TDebug;
import org.tritonus.share.sampled.TConversionTool;
import org.tritonus.share.sampled.mixer.TBaseDataLine;
import org.tritonus.share.sampled.mixer.TMixer;



public class EsdTargetDataLine
	extends		TBaseDataLine
	implements	TargetDataLine
{
	// private static final Class[]	CONTROL_CLASSES = {GainControl.class};


	private EsdRecordingStream	m_esdStream;
	private boolean			m_bSwapBytes;
	private byte[]			m_abSwapBuffer;

	/*
	 *	Only used if m_bSwapBytes is true.
	 */
	private int			m_nBytesPerSample;


	public EsdTargetDataLine(TMixer mixer, AudioFormat format, int nBufferSize)
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


	protected void openImpl()
	{
		if (TDebug.TraceTargetDataLine)
		{
			TDebug.out("EsdTargetDataLine.openImpl(): called.");
		}
		/*
		 *	Checks that a format is set.
		 *	Sets the buffer size to a default value if not
		 *	already set.
		 */
		checkOpen();
		AudioFormat	format = getFormat();
		AudioFormat.Encoding	encoding  = format.getEncoding();
		boolean			bBigEndian = format.isBigEndian();
		m_bSwapBytes = false;
		if (format.getSampleSizeInBits() == 16 && bBigEndian)
		{
			m_bSwapBytes = true;
			bBigEndian = false;
		}
		else if (format.getSampleSizeInBits() == 8 &&
			 encoding.equals(AudioFormat.Encoding.PCM_SIGNED))
		{
			m_bSwapBytes = true;
			encoding = AudioFormat.Encoding.PCM_UNSIGNED;
		}
		if (m_bSwapBytes)
		{
			format = new AudioFormat(encoding,
						 format.getSampleRate(),
						 format.getSampleSizeInBits(),
						 format.getChannels(),
						 format.getFrameSize(),
						 format.getFrameRate(),
						 bBigEndian);
			m_nBytesPerSample = format.getFrameSize() / format.getChannels();
		}
		int	nOutFormat = Esd.ESD_STREAM | Esd.ESD_PLAY | EsdUtils.getEsdFormat(format);
		m_esdStream = new EsdRecordingStream();
		m_esdStream.open(nOutFormat,
				 (int) format.getSampleRate());
	}

/*
	public void start()
	{
		setStarted(true);
		setActive(true);
		if (TDebug.TraceSourceDataLine)
		{
			TDebug.out("EsdTargetDataLine.start(): channel started.");
		}
	}



	public void stop()
	{
		setStarted(false);
	}
*/


	public int available()
	{
		// TODO:
		return -1;
		// return m_nAvailable;
	}




	// TODO: check if should block
	public int read(byte[] abData, int nOffset, int nLength)
	{
		if (TDebug.TraceTargetDataLine)
		{
			TDebug.out("EsdTargetDataLine.read(): called.");
			TDebug.out("EsdTargetDataLine.read(): wanted length: " + nLength);
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
				TDebug.out("EsdTargetDataLine.read(): stream closed");
			}
		}
		int	nBytesRead = m_esdStream.read(abData, nOffset, nLength);
		if (TDebug.TraceTargetDataLine)
		{
			TDebug.out("EsdTargetDataLine.read(): read (bytes): " + nBytesRead);
		}
		if (m_bSwapBytes && nBytesRead > 0)
		{
			TConversionTool.swapOrder16(abData, nOriginalOffset, nBytesRead / 2);
		}
		return nBytesRead;
	}



	public void closeImpl()
	{
		m_esdStream.close();
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



	public class EsdTargetDataLineGainControl
		extends		FloatControl
	{
		// private float		m_fGain;
		// private boolean		m_bMuted;



		/*package*/ EsdTargetDataLineGainControl()
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
				EsdTargetDataLine.this.setGain(getValue());
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
  EsdTargetDataLine.this.setGain(getMinimum());
  }
  else
  {
  EsdTargetDataLine.this.setGain(getGain());
  }
  }
  }
*/


	}
}



/*** EsdTargetDataLine.java ***/
