/*
 *	FormatConversionProvider.java
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


package	javax.sound.sampled.spi;


import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioInputStream;

import org.tritonus.share.TDebug;



public abstract class FormatConversionProvider
{
	public abstract AudioFormat.Encoding[] getSourceEncodings();



	public abstract AudioFormat.Encoding[] getTargetEncodings();



	public boolean isSourceEncodingSupported(
		AudioFormat.Encoding sourceEncoding)
	{
		AudioFormat.Encoding[]	aSourceEncodings = getSourceEncodings();
		return arrayContains(aSourceEncodings, sourceEncoding);
	}



	public boolean isTargetEncodingSupported(
		AudioFormat.Encoding targetEncoding)
	{
		AudioFormat.Encoding[]	aTargetEncodings = getTargetEncodings();
		return arrayContains(aTargetEncodings, targetEncoding);
	}



	public abstract AudioFormat.Encoding[] getTargetEncodings(
		AudioFormat sourceFormat);


	/**
	 * WARNING: this method uses <code>getTargetEncodings(AudioFormat);</code>
	 * which may create infinite loops if the latter is overwritten.
	 */
	public boolean isConversionSupported(
		AudioFormat.Encoding targetEncoding,
		AudioFormat sourceFormat)
	{
		if (TDebug.TraceAudioConverter)
		{
			TDebug.out(">FormatConversionProvider.isConversionSupported(AudioFormat.Encoding, AudioFormat):");
			TDebug.out("class: "+getClass().getName());
			TDebug.out("checking if conversion possible");
			TDebug.out("from: " + sourceFormat);
			TDebug.out("to: " + targetEncoding);
		}
		AudioFormat.Encoding[]	aTargetEncodings = getTargetEncodings(sourceFormat);
		boolean res = arrayContains(aTargetEncodings, targetEncoding);
		if (TDebug.TraceAudioConverter) {
			TDebug.out("< result="+res);
		}
		return res;
	}



	public abstract AudioFormat[] getTargetFormats(
		AudioFormat.Encoding targetEncoding,
		AudioFormat sourceFormat);



	/**
	 * WARNING: this method uses <code>getTargetFormats(AudioFormat.Encoding, AudioFormat)</code>
	 * which may create infinite loops if the latter is overwritten.
	 */
	public boolean isConversionSupported(
		AudioFormat targetFormat,
		AudioFormat sourceFormat)
	{
		if (TDebug.TraceAudioConverter)
		{
			TDebug.out(">FormatConversionProvider.isConversionSupported(AudioFormat, AudioFormat):");
			TDebug.out("class: "+getClass().getName());
			TDebug.out("checking if conversion possible");
			TDebug.out("from: " + sourceFormat);
			TDebug.out("to: " + targetFormat);
		}
		AudioFormat[]	aTargetFormats = getTargetFormats(targetFormat.getEncoding(), sourceFormat);
		for (int i = 0; i <  aTargetFormats.length; i++)
		{
			if (TDebug.TraceAudioConverter)
			{
				TDebug.out("checking against possible target format: " + aTargetFormats[i]);
			}
			if (aTargetFormats[i] != null && aTargetFormats[i].matches(targetFormat))
			{
				if (TDebug.TraceAudioConverter) 
				{
					TDebug.out("<result=true");
				}
				return true;
			}
		}
		if (TDebug.TraceAudioConverter) {
			TDebug.out("<result=false");
		}
		return false;
	}



	public abstract AudioInputStream getAudioInputStream(
		AudioFormat.Encoding targetEncoding,
		AudioInputStream audioInputStream);



	public abstract AudioInputStream getAudioInputStream(
		AudioFormat targetFormat,
		AudioInputStream audioInputStream);



	/*
	 *	Returns true, if obj is contained in aArray.
	 */
	private static boolean arrayContains(Object[] aArray, Object obj)
	{
		for (int i = 0; i <  aArray.length; i++)
		{
			if (aArray[i] != null && aArray[i].equals(obj))
			{
				return true;
			}
		}
		return false;
	}
}



/*** FormatConversionProvider.java ***/
