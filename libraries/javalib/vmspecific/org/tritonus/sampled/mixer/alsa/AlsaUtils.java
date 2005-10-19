/*
 *	AlsaUtils.java
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


import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioSystem;

import org.tritonus.lowlevel.alsa.AlsaPcm;
import org.tritonus.share.sampled.AudioFormats;



public class AlsaUtils
{
	private static AudioFormat[]	sm_aFormatTable = new AudioFormat[32];
	static
	{
		sm_aFormatTable[AlsaPcm.SND_PCM_FORMAT_S8] = new AudioFormat(
			AudioFormat.Encoding.PCM_SIGNED,
			AudioSystem.NOT_SPECIFIED,
			8,
			AudioSystem.NOT_SPECIFIED,
			AudioSystem.NOT_SPECIFIED,
			AudioSystem.NOT_SPECIFIED,
			true);
		sm_aFormatTable[AlsaPcm.SND_PCM_FORMAT_U8] = new AudioFormat(
			AudioFormat.Encoding.PCM_UNSIGNED,
			AudioSystem.NOT_SPECIFIED,
			8,
			AudioSystem.NOT_SPECIFIED,
			AudioSystem.NOT_SPECIFIED,
			AudioSystem.NOT_SPECIFIED,
			true);
		sm_aFormatTable[AlsaPcm.SND_PCM_FORMAT_S16_LE] = new AudioFormat(
			AudioFormat.Encoding.PCM_SIGNED,
			AudioSystem.NOT_SPECIFIED,
			16,
			AudioSystem.NOT_SPECIFIED,
			AudioSystem.NOT_SPECIFIED,
			AudioSystem.NOT_SPECIFIED,
			false);
		sm_aFormatTable[AlsaPcm.SND_PCM_FORMAT_S16_BE] = new AudioFormat(
			AudioFormat.Encoding.PCM_SIGNED,
			AudioSystem.NOT_SPECIFIED,
			16,
			AudioSystem.NOT_SPECIFIED,
			AudioSystem.NOT_SPECIFIED,
			AudioSystem.NOT_SPECIFIED,
			true);
		sm_aFormatTable[AlsaPcm.SND_PCM_FORMAT_U16_LE] = new AudioFormat(
			AudioFormat.Encoding.PCM_UNSIGNED,
			AudioSystem.NOT_SPECIFIED,
			16,
			AudioSystem.NOT_SPECIFIED,
			AudioSystem.NOT_SPECIFIED,
			AudioSystem.NOT_SPECIFIED,
			false);
		sm_aFormatTable[AlsaPcm.SND_PCM_FORMAT_U16_BE] = new AudioFormat(
			AudioFormat.Encoding.PCM_UNSIGNED,
			AudioSystem.NOT_SPECIFIED,
			16,
			AudioSystem.NOT_SPECIFIED,
			AudioSystem.NOT_SPECIFIED,
			AudioSystem.NOT_SPECIFIED,
			true);
		sm_aFormatTable[AlsaPcm.SND_PCM_FORMAT_S24_LE] = new AudioFormat(
			AudioFormat.Encoding.PCM_SIGNED,
			AudioSystem.NOT_SPECIFIED,
			24,
			AudioSystem.NOT_SPECIFIED,
			AudioSystem.NOT_SPECIFIED,
			AudioSystem.NOT_SPECIFIED,
			false);
		sm_aFormatTable[AlsaPcm.SND_PCM_FORMAT_S24_BE] = new AudioFormat(
			AudioFormat.Encoding.PCM_SIGNED,
			AudioSystem.NOT_SPECIFIED,
			24,
			AudioSystem.NOT_SPECIFIED,
			AudioSystem.NOT_SPECIFIED,
			AudioSystem.NOT_SPECIFIED,
			true);
		sm_aFormatTable[AlsaPcm.SND_PCM_FORMAT_U24_LE] = new AudioFormat(
			AudioFormat.Encoding.PCM_UNSIGNED,
			AudioSystem.NOT_SPECIFIED,
			24,
			AudioSystem.NOT_SPECIFIED,
			AudioSystem.NOT_SPECIFIED,
			AudioSystem.NOT_SPECIFIED,
			false);
		sm_aFormatTable[AlsaPcm.SND_PCM_FORMAT_U24_BE] = new AudioFormat(
			AudioFormat.Encoding.PCM_UNSIGNED,
			AudioSystem.NOT_SPECIFIED,
			24,
			AudioSystem.NOT_SPECIFIED,
			AudioSystem.NOT_SPECIFIED,
			AudioSystem.NOT_SPECIFIED,
			true);
		sm_aFormatTable[AlsaPcm.SND_PCM_FORMAT_S32_LE] = new AudioFormat(
			AudioFormat.Encoding.PCM_SIGNED,
			AudioSystem.NOT_SPECIFIED,
			32,
			AudioSystem.NOT_SPECIFIED,
			AudioSystem.NOT_SPECIFIED,
			AudioSystem.NOT_SPECIFIED,
			false);
		sm_aFormatTable[AlsaPcm.SND_PCM_FORMAT_S32_BE] = new AudioFormat(
			AudioFormat.Encoding.PCM_SIGNED,
			AudioSystem.NOT_SPECIFIED,
			32,
			AudioSystem.NOT_SPECIFIED,
			AudioSystem.NOT_SPECIFIED,
			AudioSystem.NOT_SPECIFIED,
			true);
		sm_aFormatTable[AlsaPcm.SND_PCM_FORMAT_U32_LE] = new AudioFormat(
			AudioFormat.Encoding.PCM_UNSIGNED,
			AudioSystem.NOT_SPECIFIED,
			32,
			AudioSystem.NOT_SPECIFIED,
			AudioSystem.NOT_SPECIFIED,
			AudioSystem.NOT_SPECIFIED,
			false);
		sm_aFormatTable[AlsaPcm.SND_PCM_FORMAT_U32_BE] = new AudioFormat(
			AudioFormat.Encoding.PCM_UNSIGNED,
			AudioSystem.NOT_SPECIFIED,
			32,
			AudioSystem.NOT_SPECIFIED,
			AudioSystem.NOT_SPECIFIED,
			AudioSystem.NOT_SPECIFIED,
			true);
		sm_aFormatTable[AlsaPcm.SND_PCM_FORMAT_MU_LAW] = new AudioFormat(
			AudioFormat.Encoding.ULAW,
			AudioSystem.NOT_SPECIFIED,
			8,
			AudioSystem.NOT_SPECIFIED,
			AudioSystem.NOT_SPECIFIED,
			AudioSystem.NOT_SPECIFIED,
			true);
		sm_aFormatTable[AlsaPcm.SND_PCM_FORMAT_A_LAW] = new AudioFormat(
			AudioFormat.Encoding.ALAW,
			AudioSystem.NOT_SPECIFIED,
			8,
			AudioSystem.NOT_SPECIFIED,
			AudioSystem.NOT_SPECIFIED,
			AudioSystem.NOT_SPECIFIED,
			true);
	}



	public static AudioFormat getAlsaFormat(int nAudioFormat)
	{
		return sm_aFormatTable[nAudioFormat];
	}



	public static int getAlsaFormat(AudioFormat audioFormat)
	{
		int	nChannels = audioFormat.getChannels();
		AudioFormat.Encoding	encoding = audioFormat.getEncoding();
		int	nSampleSize = audioFormat.getSampleSizeInBits();

		for (int nFormat = 0; nFormat < sm_aFormatTable.length; nFormat++)
		{
			if (sm_aFormatTable[nFormat] != null && AudioFormats.matches(sm_aFormatTable[nFormat], audioFormat))
			{
				return nFormat;
			}
		}
		return AlsaPcm.SND_PCM_FORMAT_UNKNOWN;
	}
}



/*** AlsaUtils.java ***/
