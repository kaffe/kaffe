/*
 *	EsdUtils.java
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

import org.tritonus.lowlevel.esd.Esd;



public class EsdUtils
{
	public static int getEsdFormat(AudioFormat audioFormat)
	{
		int	nChannels = audioFormat.getChannels();
		AudioFormat.Encoding	encoding = audioFormat.getEncoding();
		int	nSampleSize = audioFormat.getSampleSizeInBits();
		int	nFormat = 0;

		if (nSampleSize == 8)
		{
			if (! encoding.equals(AudioFormat.Encoding.PCM_UNSIGNED))
			{
				throw new IllegalArgumentException("encoding must be PCM_UNSIGNED for 8 bit data");
			}
			nFormat |= Esd.ESD_BITS8;
		}
		else if (nSampleSize == 16)
		{
			if (! encoding.equals(AudioFormat.Encoding.PCM_SIGNED))
			{
				throw new IllegalArgumentException("encoding must be PCM_SIGNED for 16 bit data");
			}
			nFormat |= Esd.ESD_BITS16;
		}
		else
		{
			throw new IllegalArgumentException("only 8 bit and 16 bit samples are supported");
		}

		if (nChannels == 1)
		{
			nFormat |= Esd.ESD_MONO;
		}
		else if (nChannels == 2)
		{
			nFormat |= Esd.ESD_STEREO;
		}
		else
		{
			throw new IllegalArgumentException("only mono and stereo are supported");
		}

		return nFormat;
	}
}



/*** EsdUtils.java ***/
