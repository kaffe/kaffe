/*
 *	AudioFormat.java
 */

/*
 *  Copyright (c) 1999,2000 by Matthias Pfisterer <Matthias.Pfisterer@gmx.de>
 *  Copyright (c) 1999 by Florian Bomers <florian@bome.com>
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


package	javax.sound.sampled;




public class AudioFormat
{
	private Encoding	m_encoding;
	private float		m_fSampleRate;
	private int		m_nSampleSizeInBits;
	private int		m_nChannels;
	private int		m_nFrameSize;
	private float		m_fFrameRate;
	private boolean		m_bBigEndian;



	public AudioFormat(Encoding encoding,
			   float fSampleRate,
			   int nSampleSizeInBits,
			   int nChannels,
			   int nFrameSize,
			   float fFrameRate,
			   boolean bBigEndian)
	{
		m_encoding = encoding;
		m_fSampleRate = fSampleRate;
		m_nSampleSizeInBits = nSampleSizeInBits;
		m_nChannels = nChannels;
		m_nFrameSize = nFrameSize;
		m_fFrameRate = fFrameRate;
		m_bBigEndian = bBigEndian;
	}



	public AudioFormat(float fSampleRate,
			   int nSampleSizeInBits,
			   int nChannels,
			   boolean bSigned,
			   boolean bBigEndian)
	{
		this(bSigned ? Encoding.PCM_SIGNED : Encoding.PCM_UNSIGNED,
		     fSampleRate,
		     nSampleSizeInBits,
		     nChannels,
		     ((nChannels != AudioSystem.NOT_SPECIFIED) && (nSampleSizeInBits != AudioSystem.NOT_SPECIFIED))?
		     (nChannels * nSampleSizeInBits) / 8:AudioSystem.NOT_SPECIFIED,
		     fSampleRate,
		     bBigEndian);
	}



	public Encoding getEncoding()
	{
		return m_encoding;
	}



	public float getSampleRate()
	{
		return m_fSampleRate;
	}



	public int getSampleSizeInBits()
	{
		return m_nSampleSizeInBits;
	}



	public int getChannels()
	{
		return m_nChannels;
	}



	public int getFrameSize()
	{
		return m_nFrameSize;
	}



	public float getFrameRate()
	{
		return m_fFrameRate;
	}



	public boolean isBigEndian()
	{
		return m_bBigEndian;
	}



	//$$fb 19 Dec 99: added
	//$$fb ohh, I just read the documentation - it must indeed match exactly.
	//I think it would be much better that a pair of values also matches
	//when at least one is NOT_SPECIFIED.
	// support for NOT_SPECIFIED should be consistent in JavaSound...
	// As a "workaround" I implemented it like this in TFormatConversionProvider
	private boolean doMatch(int i1, int i2)
	{
		return i1 == AudioSystem.NOT_SPECIFIED
			|| i2 == AudioSystem.NOT_SPECIFIED
			|| i1 == i2;
	}



	private boolean doMatch(float f1, float f2)
	{
		return f1 == AudioSystem.NOT_SPECIFIED
			|| f2 == AudioSystem.NOT_SPECIFIED
			|| Math.abs(f1 - f2) < 1.0e-9;
	}



	public boolean matches(AudioFormat format)
	{
		//$$fb 19 Dec 99: endian must be checked, too.
		//
		// we do have a problem with redundant elements:
		// e.g. 
		// encoding=ALAW || ULAW -> bigEndian and samplesizeinbits don't matter
		// sample size in bits == 8 -> bigEndian doesn't matter
		// sample size in bits > 8 -> PCM is always signed. 
		// This is an overall issue in JavaSound, I think.
		// At present, it is not consistently implemented to support these 
		// redundancies and implicit definitions
		//
		// As a workaround of this issue I return in the converters
		// all combinations, e.g. for ULAW I return bigEndian and !bigEndian formats.
/* old version
   return getEncoding().equals(format.getEncoding())
   && getChannels() == format.getChannels()
   && getSampleSizeInBits() == format.getSampleSizeInBits()
   && getFrameSize() == format.getFrameSize()
   && (Math.abs(getSampleRate() - format.getSampleRate()) < 1.0e-9 || format.getSampleRate() == AudioSystem.NOT_SPECIFIED)
   && (Math.abs(getFrameRate() - format.getFrameRate()) < 1.0e-9 || format.getFrameRate() == AudioSystem.NOT_SPECIFIED);
*/
		// as proposed by florian
		return getEncoding().equals(format.getEncoding())
			&& (format.getSampleSizeInBits()<=8 || format.getSampleSizeInBits()==AudioSystem.NOT_SPECIFIED || isBigEndian()==format.isBigEndian())
			&& doMatch(getChannels(),format.getChannels())
			&& doMatch(getSampleSizeInBits(), format.getSampleSizeInBits())
			&& doMatch(getFrameSize(), format.getFrameSize())
			&& doMatch(getSampleRate(), format.getSampleRate())
			&& doMatch(getFrameRate(),format.getFrameRate());
	}



	public String toString()
	{
		return getEncoding() + ", " +
			getChannels() + " channel(s), " +
			getSampleSizeInBits() + " bit samples, " +
			getFrameSize() + " byte frames, " +
			getSampleRate() + " Hz, " +
			getFrameRate() + " frames/second, " +
			(isBigEndian() ? "big endian" : "little endian");
	}




	public static class Encoding
	{
		public static final Encoding	PCM_SIGNED = new Encoding("PCM_SIGNED");
		public static final Encoding	PCM_UNSIGNED = new Encoding("PCM_UNSIGNED");
		public static final Encoding	ULAW = new Encoding("ULAW");
		public static final Encoding	ALAW = new Encoding("ALAW");




		private String	m_strName;



		protected Encoding(String strName)
		{
			m_strName = strName;
		}



		public final boolean equals(Object obj)
		{
			return super.equals(obj);
		}



		public final int hashCode()
		{
			return super.hashCode();
		}



		public final String toString()
		{
			return m_strName;
		}
	}



}



/*** AudioFormat.java ***/
