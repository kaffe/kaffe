/*
 *	EsdSourceDataLine.java
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
import javax.sound.sampled.BooleanControl;
import javax.sound.sampled.DataLine;
import javax.sound.sampled.FloatControl;
import javax.sound.sampled.LineUnavailableException;
import javax.sound.sampled.SourceDataLine;

import org.tritonus.lowlevel.esd.Esd;
import org.tritonus.lowlevel.esd.EsdStream;
import org.tritonus.share.TDebug;
import org.tritonus.share.sampled.TConversionTool;
import org.tritonus.share.sampled.TVolumeUtils;
import org.tritonus.share.sampled.mixer.TBaseDataLine;
import org.tritonus.share.sampled.mixer.TMixer;



public class EsdSourceDataLine
	extends		TBaseDataLine
	implements	SourceDataLine
{
	private EsdStream		m_esdStream;
	private boolean			m_bSwapBytes;
	private byte[]			m_abSwapBuffer;

	/*
	 *	Only used if m_bSwapBytes is true.
	 */
	private int			m_nBytesPerSample;


	/*
	 *	Used to store the muted state.
	 */
	private boolean			m_bMuted;


	/*
	 *	Used to store the gain while the channel is muted.
	 */
	private float			m_fGain;


	/*
	 *	Used to store the pan while the channel is muted.
	 */
	private float			m_fPan;





	// TODO: has info object to change if format or buffer size are changed later?
	// no, but it has to represent the mixer's capabilities. So a fixed info per mixer.
	public EsdSourceDataLine(TMixer mixer, AudioFormat format, int nBufferSize)
		throws	LineUnavailableException
	{
		super(mixer,
		      new DataLine.Info(SourceDataLine.class,
					format,
					nBufferSize));
		addControl(new EsdSourceDataLineGainControl());
		addControl(new EsdSourceDataLinePanControl());
		addControl(new EsdSourceDataLineMuteControl());
/*
  if (TDebug.TraceSourceDataLine)
  {
  TDebug.out("EsdSourceDataLine.<init>(): buffer size: " + nBufferSize);
  }
*/
	}



	protected void openImpl()
	{
		if (TDebug.TraceSourceDataLine)
		{
			TDebug.out("EsdSourceDataLine.openImpl(): called.");
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
		/*
		 *	Ugly hack, should fade as soon as possible.
		 *	IDEA: if swapping IS necessary here, isolate the "detection" of
		 *	big-endian architectures into a seperate class. Perhaps have a
		 *	property with a list of big-endian architecture names, so that
		 *	support can be extended to other architectures without changes
		 *	in the source code.
		 */
		// TODO: does 8 bit work? (perhaps problem inside esd?)
		if (System.getProperty("os.arch").equals("ppc")
		    && format.getSampleSizeInBits() == 16)
		{
			m_bSwapBytes ^= true;
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

		m_esdStream = new EsdStream();
		m_esdStream.open(nOutFormat,
				 (int) format.getSampleRate());
	}




	public int available()
	{
		// TODO:
		return -1;
	}




	// TODO: check if should block
	public int write(byte[] abData, int nOffset, int nLength)
	{
		if (TDebug.TraceSourceDataLine)
		{
			TDebug.out("EsdSourceDataLine.write(): called.");
		}
		if (m_bSwapBytes)
		{
			if (m_abSwapBuffer == null || m_abSwapBuffer.length < nOffset + nLength)
			{
				m_abSwapBuffer = new byte[nOffset + nLength];
			}
			TConversionTool.changeOrderOrSign(
				abData, nOffset,
				m_abSwapBuffer, nOffset,
				nLength, m_nBytesPerSample);
			abData = m_abSwapBuffer;
		}
		if (nLength > 0 && !isActive())
		{
			start();
		}
		int		nRemaining = nLength;
		while (nRemaining > 0 && isOpen())
		{
			synchronized (this)
			{
				/*
				  while ((availableWrite() == 0 || isPaused()) && isOpen())
				  {
				  try
				  {
				  wait();
				  }
				  catch (InterruptedException e)
				  {
				if (TDebug.TraceAllExceptions)
				{
					TDebug.out(e);
				}
				  }
				  }
				*/
				if (!isOpen())
				{
					return nLength - nRemaining;
				}
				// TODO: check return 
				int nWritten = m_esdStream.write(abData, nOffset, nRemaining);
				nOffset += nWritten;
				nRemaining -= nWritten;
			}
		}
		return nLength;
	}



	protected void closeImpl()
	{
		if (TDebug.TraceSourceDataLine)
		{
			TDebug.out("EsdSourceDataLine.closeImpl(): called.");
		}
		m_esdStream.close();
	}





	public void drain()
	{
		if (TDebug.TraceSourceDataLine)
		{
			TDebug.out("EsdSourceDataLine.drain(): called.");
		}
		// TODO:
	}



	public void flush()
	{
		if (TDebug.TraceSourceDataLine)
		{
			TDebug.out("EsdSourceDataLine.flush(): called.");
		}
		// TODO:
	}



	/**
	 *	fGain is logarithmic!!
	 */
	private void setGain(float fGain)
	{
		if (TDebug.TraceSourceDataLine)
		{
			TDebug.out("EsdSourceDataLine.setGain(): gain: " + fGain);
		}
		m_fGain = fGain;
		if (! m_bMuted)
		{
			setGainImpl();
		}
	}



	/**
	 */
	private void setPan(float fPan)
	{
		if (TDebug.TraceSourceDataLine)
		{
			TDebug.out("EsdSourceDataLine.setPan(): pan: " + fPan);
		}
		m_fPan = fPan;
		if (! m_bMuted)
		{
			setGainImpl();
		}
	}



	/**
	 */
	private void setMuted(boolean bMuted)
	{
		if (TDebug.TraceSourceDataLine)
		{
			TDebug.out("EsdSourceDataLine.setMuted(): muted: " + bMuted);
		}
		m_bMuted = bMuted;
		if (m_bMuted)
		{
			// m_esdStream.setVolume(0, 0);
		}
		else
		{
			setGainImpl();
		}
	}



	/**
	 */
	private void setGainImpl()
	{
		if (TDebug.TraceSourceDataLine)
		{
			TDebug.out("EsdSourceDataLine.setGainImpl(): called: ");
		}
		float	fLeftDb = m_fGain + m_fPan * 15.0F;
		float	fRightDb = m_fGain - m_fPan * 15.0F;
		float	fLeftLinear = (float) TVolumeUtils.log2lin(fLeftDb);
		float	fRightLinear = (float) TVolumeUtils.log2lin(fRightDb);
// 		m_esdStream.setVolume((int) (fLeftLinear * 256),
// 				      (int) (fRightLinear * 256));
	}




	// IDEA: move inner classes to TBaseDataLine
	public class EsdSourceDataLineGainControl
		extends		FloatControl
	{
		/*package*/ EsdSourceDataLineGainControl()
		{
			super(FloatControl.Type.MASTER_GAIN,	// or VOLUME  ?
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
			if (TDebug.TraceSourceDataLine)
			{
				TDebug.out("EsdSourceDataLineGainControl.setValue(): gain: " + fGain);
			}
			float	fOldGain = getValue();
			super.setValue(fGain);
			if (Math.abs(fOldGain - getValue()) > 1.0E-9)
			{
				if (TDebug.TraceSourceDataLine)
				{
					TDebug.out("EsdSourceDataLineGainControl.setValue(): really changing gain");
				}
				EsdSourceDataLine.this.setGain(getValue());
			}
		}
	}



	// IDEA: move inner classes to TBaseDataLine
	public class EsdSourceDataLinePanControl
		extends		FloatControl
	{
		/*package*/ EsdSourceDataLinePanControl()
		{
			super(FloatControl.Type.PAN,
			      -1.0F,	// MIN_GAIN,
			      1.0F,	// MAX_GAIN,
			      0.01F,	// precision
			      0,	// update period?
			      0.0F,	// initial value
			      "??",
			      "left",
			      "center",
			      "right");
		}



		public void setValue(float fPan)
		{
			if (TDebug.TraceSourceDataLine)
			{
				TDebug.out("EsdSourceDataLinePanControl.setValue(): pan: " + fPan);
			}
			float	fOldPan = getValue();
			super.setValue(fPan);
			if (Math.abs(fOldPan - getValue()) > 1.0E-9)
			{
				if (TDebug.TraceSourceDataLine)
				{
					TDebug.out("EsdSourceDataLinePanControl.setValue(): really changing pan");
				}
				EsdSourceDataLine.this.setPan(getValue());
			}
		}
	}



	public class EsdSourceDataLineMuteControl
		extends		BooleanControl
	{
		/*package*/ EsdSourceDataLineMuteControl()
		{
			super(BooleanControl.Type.MUTE,
			      false,
			      "muted",
			      "unmuted");
		}



		public void setValue(boolean bMuted)
		{
			if (TDebug.TraceSourceDataLine)
			{
				TDebug.out("EsdSourceDataLineMuteControl.setValue(): muted: " + bMuted);
			}
			if (bMuted != getValue())
			{
				if (TDebug.TraceSourceDataLine)
				{
					TDebug.out("EsdSourceDataLineMuteControl.setValue(): really changing mute status");
				}
				super.setValue(bMuted);
				EsdSourceDataLine.this.setMuted(getValue());
			}
		}


/*
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
  EsdSourceDataLine.this.setGain(getMinimum());
  }
  else
  {
  EsdSourceDataLine.this.setGain(getGain());
  }
  }
  }
*/


	}
}



/*** EsdSourceDataLine.java ***/
