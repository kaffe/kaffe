/*
 *	AlsaMixerElement.java
 */

/*
 *  Copyright (c) 2001 - 2002 by Matthias Pfisterer <Matthias.Pfisterer@gmx.de>
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

package	org.tritonus.lowlevel.alsa;


import org.tritonus.share.TDebug;


/**	TODO:
 */
public class AlsaMixerElement
{
	/*	Channel type constants.
		They mirror the values of snd_mixer_selem_channel_id_t.
	 */

	/** Unknown */
	public static final int SND_MIXER_SCHN_UNKNOWN = -1;
	/** Front left */
	public static final int SND_MIXER_SCHN_FRONT_LEFT = 0;
	/** Front right */
	public static final int SND_MIXER_SCHN_FRONT_RIGHT = 1;
	/** Front center */
	public static final int SND_MIXER_SCHN_FRONT_CENTER = 2;
	/** Rear left */
	public static final int SND_MIXER_SCHN_REAR_LEFT = 3;
	/** Rear right */
	public static final int SND_MIXER_SCHN_REAR_RIGHT = 4;
	/** Woofer */
	public static final int SND_MIXER_SCHN_WOOFER = 5;
	public static final int SND_MIXER_SCHN_LAST = 31;
	/** Mono (Front left alias) */
	public static final int SND_MIXER_SCHN_MONO = SND_MIXER_SCHN_FRONT_LEFT;



	private AlsaMixer	m_mixer;
	private long		m_lNativeHandle;



	static
	{
		Alsa.loadNativeLibrary();
		if (TDebug.TraceAlsaMixerNative)
		{
			setTrace(true);
		}
	}



	public AlsaMixerElement(AlsaMixer mixer,
				int nIndex,
				String strName)
	{
		m_mixer = mixer;
		int	nReturn;
		nReturn = open(getMixer(), nIndex, strName);
		{
			if (nReturn < 0)
			{
				throw new RuntimeException("cannot open");
			}
		}
	}

	/**	Calls snd_mixer_find_selem().
	 */
	private native int open(AlsaMixer mixer,
			       int nIndex,
			       String strName);


	private AlsaMixer getMixer()
	{
		return m_mixer;
	}


	// TODO: getId()


	/**	Calls snd_mixer_selem_get_name().
	 */
	public native String getName();

	/**	Calls snd_mixer_selem_get_index().
	 */
	public native int getIndex();

	/**	Calls snd_mixer_selem_is_active().
	 */
	public native boolean isActive();

	/**	Calls snd_mixer_selem_is_playback_mono().
	 */
	public native boolean isPlaybackMono();

	/**	Calls snd_mixer_selem_has_playback_channel().
	 */
	public native boolean hasPlaybackChannel(int nChannelType);

	/**	Calls snd_mixer_selem_is_capture_mono().
	 */
	public native boolean isCaptureMono();

	/**	Calls snd_mixer_selem_has_capture_channel().
	 */
	public native boolean hasCaptureChannel(int nChannelType);

	/**	Calls snd_mixer_selem_get_capture_group().
	 */
	public native int getCaptureGroup();

	/**	Calls snd_mixer_selem_has_common_volume().
	 */
	public native boolean hasCommonVolume();

	/**	Calls snd_mixer_selem_has_playback_volume().
	 */
	public native boolean hasPlaybackVolume();

	/**	Calls snd_mixer_selem_has_playback_volume_joined().
	 */
	public native boolean hasPlaybackVolumeJoined();

	/**	Calls snd_mixer_selem_has_capture_volume().
	 */
	public native boolean hasCaptureVolume();

	/**	Calls snd_mixer_selem_has_capture_volume_joined().
	 */
	public native boolean hasCaptureVolumeJoined();


	/**	Calls snd_mixer_selem_has_common_switch().
	 */
	public native boolean hasCommonSwitch();

	/**	Calls snd_mixer_selem_has_playback_switch().
	 */
	public native boolean hasPlaybackSwitch();

	/**	Calls snd_mixer_selem_has_playback_switch_joined().
	 */
	public native boolean hasPlaybackSwitchJoined();

	/**	Calls snd_mixer_selem_has_capture_switch().
	 */
	public native boolean hasCaptureSwitch();

	/**	Calls snd_mixer_selem_has_capture_switch_joined().
	 */
	public native boolean hasCaptureSwitchJoinded();

	/**	Calls snd_mixer_selem_has_capture_switch_exclusive().
	 */
	public native boolean hasCaptureSwitchExclusive();


	/**	Calls snd_mixer_selem_get_playback_volume().
	 */
	public native int getPlaybackVolume(int nChannelType);

	/**	Calls snd_mixer_selem_get_capture_volume().
	 */
	public native int getCaptureVolume(int nChannelType);

	/**	Calls snd_mixer_selem_get_playback_switch().
	 */
	public native boolean getPlaybackSwitch(int nChannelType);

	/**	Calls snd_mixer_selem_get_capture_switch().
	 */
	public native boolean getCaptureSwitch(int nChannelType);


	/**	Calls snd_mixer_selem_set_playback_volume().
	 */
	public native void setPlaybackVolume(int nChannelType, int nValue);

	/**	Calls snd_mixer_selem_set_capture_volume().
	 */
	public native void setCaptureVolume(int nChannelType, int nValue);

	/**	Calls snd_mixer_selem_set_playback_volume_all().
	 */
	public native void setPlaybackVolumeAll(int nValue);

	/**	Calls snd_mixer_selem_set_capture_volume_all().
	 */
	public native void setCaptureVolumeAll(int nValue);


	/**	Calls snd_mixer_selem_set_playback_switch().
	 */
	public native void setPlaybackSwitch(int nChannelType, boolean bValue);

	/**	Calls snd_mixer_selem_set_capture_switch().
	 */
	public native void setCaptureSwitch(int nChannelType, boolean bValue);

	/**	Calls snd_mixer_selem_set_playback_switch_all().
	 */
	public native void setPlaybackSwitchAll(boolean bValue);

	/**	Calls snd_mixer_selem_set_capture_switch_all().
	 */
	public native void setCaptureSwitchAll(boolean bValue);


	/**	Calls snd_mixer_selem_get_playback_volume_range().
		anValues[0]: minimum
		anValues[1]: maximum
	 */
	public native void getPlaybackVolumeRange(int[] anValues);

	/**	Calls snd_mixer_selem_get_capture_volume_range().
		anValues[0]: minimum
		anValues[1]: maximum
	 */
	public native void getCaptureVolumeRange(int[] anValues);

	/**	Calls snd_mixer_selem_set_playback_volume_range().
	 */
	public native void setPlaybackVolumeRange(int nMin, int nMax);

	/**	Calls snd_mixer_selem_set_capture_volume_range().
	 */
	public native void setCaptureVolumeRange(int nMin, int nMax);



	/**	Calls snd_mixer_selem_channel_name().
	 */
	public static native String getChannelName(int nChannelType);

	/**	TODO:
	 */
	private static native void setTrace(boolean bTrace);
}



/*** AlsaMixerElement.java ***/
