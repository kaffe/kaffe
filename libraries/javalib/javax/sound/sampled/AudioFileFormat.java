/*
 *	AudioFileFormat.java
 */

/*
 *  Copyright (c) 1999 by Matthias Pfisterer <Matthias.Pfisterer@gmx.de>
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



public class AudioFileFormat
{


	private Type			m_type;
	private AudioFormat		m_audioFormat;
	private int			m_nLengthInFrames;
	private int			m_nLengthInBytes;



	public AudioFileFormat(Type type,
			       AudioFormat audioFormat,
			       int nLengthInFrames)
	{
		this(type,
		     AudioSystem.NOT_SPECIFIED,
		     audioFormat,
		     nLengthInFrames);
	}



	protected AudioFileFormat(Type type,
				  int nLengthInBytes,
				  AudioFormat audioFormat,
				  int nLengthInFrames)
	{
		m_type = type;
		m_audioFormat = audioFormat;
		m_nLengthInFrames = nLengthInFrames;
		m_nLengthInBytes = nLengthInBytes;
	}



	public Type getType()
	{
		return m_type;
	}



	public int getByteLength()
	{
		return m_nLengthInBytes;
	}




	public AudioFormat getFormat()
	{
		return m_audioFormat;
	}



	public int getFrameLength()
	{
		return m_nLengthInFrames;
	}



	// IDEA: output "not specified" of length == AudioSystem.NOT_SPECIFIED
	public String toString()
	{
		return super.toString() +
			"[type=" + getType() +
			", format=" + getFormat() +
			", lengthInFrames=" + getByteLength() +
			", lengthInBytes=" + getFrameLength() + "]";
	}








	public static class Type
	{
		// $$fb 2000-03-31: extension without dot
		public static final Type		AIFC = new Type("AIFC", "aifc");
		public static final Type		AIFF = new Type("AIFF", "aiff");
		public static final Type		AU = new Type("AU", "au");
		public static final Type		SND = new Type("SND", "snd");
		public static final Type		WAVE = new Type("WAVE", "wav");



		private String	m_strName;
		private String	m_strExtension;



		public Type(String strName, String strExtension)
		{
			m_strName = strName;
			m_strExtension = strExtension;
		}



		public String getExtension()
		{
			return m_strExtension;
		}


		/*
		 *	TODO: I requested from Sun that this should
		 *	compare the strings
		 */
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



/*** AudioFileFormat.java ***/
