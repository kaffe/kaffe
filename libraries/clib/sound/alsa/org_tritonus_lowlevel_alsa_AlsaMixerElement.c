/*
 *	org_tritonus_lowlevel_alsa_AlsaMixerElement.c
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

#include "common.h"
#include "org_tritonus_lowlevel_alsa_AlsaMixerElement.h"


snd_mixer_t*
getMixerNativeHandle(JNIEnv *env, jobject obj);


HandleFieldHandler(snd_mixer_elem_t*)



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaMixerElement
 * Method:    open
 * Signature: (Lorg/tritonus/lowlevel/alsa/AlsaMixer;ILjava/lang/String;)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_open
(JNIEnv *env, jobject obj, jobject mixer, jint nIndex, jstring strName)
{
	snd_mixer_elem_t*	handle;
	snd_mixer_t*		mixerHandle;
	snd_mixer_selem_id_t*	id;
	int			nReturn;
	const char*		name;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_open(): begin\n"); }
	mixerHandle = getMixerNativeHandle(env, mixer);
	snd_mixer_selem_id_alloca(&id);
	snd_mixer_selem_id_set_index(id, nIndex);

	name= (*env)->GetStringUTFChars(env, strName, NULL);
	if (name == NULL)
	{
		throwRuntimeException(env, "GetStringUTFChars() failed");
		return -1;
	}
	snd_mixer_selem_id_set_name(id, name);
	(*env)->ReleaseStringUTFChars(env, strName, name);
	handle = snd_mixer_find_selem(mixerHandle, id);
	if (handle == NULL)
	{
		nReturn = -1;
	}
	else
	{
		setHandle(env, obj, handle);
		nReturn = 0;
	}
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_open(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaMixerElement
 * Method:    getName
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_getName
(JNIEnv* env, jobject obj)
{
	snd_mixer_elem_t*	handle;
	jstring			name;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_getName(): begin\n"); }
	handle = getHandle(env, obj);
	name = (*env)->NewStringUTF(env, snd_mixer_selem_get_name(handle));
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_getName(): end\n"); }
	return name;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaMixerElement
 * Method:    getIndex
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_getIndex
(JNIEnv* env, jobject obj)
{
	snd_mixer_elem_t*	handle;
	int			nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_getIndex(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_mixer_selem_get_index(handle);
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_getIndex(): end\n"); }
	return nReturn;
}


/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaMixerElement
 * Method:    isActive
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_isActive
(JNIEnv* env, jobject obj)
{
	snd_mixer_elem_t*	handle;
	int			nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_isActive(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_mixer_selem_is_active(handle);
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_isActive(): end\n"); }
	return (jboolean) nReturn;
}


/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaMixerElement
 * Method:    isPlaybackMono
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_isPlaybackMono
(JNIEnv* env, jobject obj)
{
	snd_mixer_elem_t*	handle;
	int			nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_isPlaybackMono(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_mixer_selem_is_playback_mono(handle);
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_isPlaybackMono(): end\n"); }
	return (jboolean) nReturn;
}


/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaMixerElement
 * Method:    hasPlaybackChannel
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_hasPlaybackChannel
(JNIEnv* env, jobject obj, jint nChannelType)
{
	snd_mixer_elem_t*	handle;
	int			nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_hasPlaybackChannel(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_mixer_selem_has_playback_channel(handle, (snd_mixer_selem_channel_id_t) nChannelType);
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_hasPlaybackChannel(): end\n"); }
	return (jboolean) nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaMixerElement
 * Method:    isCaptureMono
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_isCaptureMono
(JNIEnv* env, jobject obj)
{
	snd_mixer_elem_t*	handle;
	int			nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_isCaptureMono(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_mixer_selem_is_capture_mono(handle);
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_isCaptureMono(): end\n"); }
	return (jboolean) nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaMixerElement
 * Method:    hasCaptureChannel
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_hasCaptureChannel
(JNIEnv* env, jobject obj, jint nChannelType)
{
	snd_mixer_elem_t*	handle;
	int			nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_hasCaptureChannel(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_mixer_selem_has_capture_channel(handle, (snd_mixer_selem_channel_id_t) nChannelType);
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_hasCaptureChannel(): end\n"); }
	return (jboolean) nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaMixerElement
 * Method:    getCaptureGroup
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_getCaptureGroup
(JNIEnv* env, jobject obj)
{
	snd_mixer_elem_t*	handle;
	int			nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_getCaptureGroup(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_mixer_selem_get_capture_group(handle);
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_getCaptureGroup(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaMixerElement
 * Method:    hasCommonVolume
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_hasCommonVolume
(JNIEnv* env, jobject obj)
{
	snd_mixer_elem_t*	handle;
	int			nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_hasCommonVolume(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_mixer_selem_has_common_volume(handle);
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_hasCommonVolume(): end\n"); }
	return (jboolean) nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaMixerElement
 * Method:    hasPlaybackVolume
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_hasPlaybackVolume
(JNIEnv* env, jobject obj)
{
	snd_mixer_elem_t*	handle;
	int			nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_hasPlaybackVolume(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_mixer_selem_has_playback_volume(handle);
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_hasPlaybackVolume(): end\n"); }
	return (jboolean) nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaMixerElement
 * Method:    hasPlaybackVolumeJoined
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_hasPlaybackVolumeJoined
(JNIEnv* env, jobject obj)
{
	snd_mixer_elem_t*	handle;
	int			nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_hasPlaybackVolumeJoined(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_mixer_selem_has_playback_volume_joined(handle);
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_hasPlaybackVolumeJoined(): end\n"); }
	return (jboolean) nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaMixerElement
 * Method:    hasCaptureVolume
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_hasCaptureVolume
(JNIEnv* env, jobject obj)
{
	snd_mixer_elem_t*	handle;
	int			nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_hasCaptureVolume(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_mixer_selem_has_capture_volume(handle);
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_hasCaptureVolume(): end\n"); }
	return (jboolean) nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaMixerElement
 * Method:    hasCaptureVolumeJoined
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_hasCaptureVolumeJoined
(JNIEnv* env, jobject obj)
{
	snd_mixer_elem_t*	handle;
	int			nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_hasCaptureVolumeJoined(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_mixer_selem_has_capture_volume_joined(handle);
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_hasCaptureVolumeJoined(): end\n"); }
	return (jboolean) nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaMixerElement
 * Method:    hasCommonSwitch
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_hasCommonSwitch
(JNIEnv* env, jobject obj)
{
	snd_mixer_elem_t*	handle;
	int			nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_hasCommonSwitch(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_mixer_selem_has_common_switch(handle);
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_hasCommonSwitch(): end\n"); }
	return (jboolean) nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaMixerElement
 * Method:    hasPlaybackSwitch
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_hasPlaybackSwitch
(JNIEnv* env, jobject obj)
{
	snd_mixer_elem_t*	handle;
	int			nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_hasPlaybackSwitch(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_mixer_selem_has_playback_switch(handle);
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_hasPlaybackSwitch(): end\n"); }
	return (jboolean) nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaMixerElement
 * Method:    hasPlaybackSwitchJoined
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_hasPlaybackSwitchJoined
(JNIEnv* env, jobject obj)
{
	snd_mixer_elem_t*	handle;
	int			nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_hasPlaybackSwitchJoined(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_mixer_selem_has_playback_switch_joined(handle);
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_hasPlaybackSwitchJoined(): end\n"); }
	return (jboolean) nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaMixerElement
 * Method:    hasCaptureSwitch
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_hasCaptureSwitch
(JNIEnv* env, jobject obj)
{
	snd_mixer_elem_t*	handle;
	int			nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_hasCaptureSwitch(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_mixer_selem_has_capture_switch(handle);
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_hasCaptureSwitch(): end\n"); }
	return (jboolean) nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaMixerElement
 * Method:    hasCaptureSwitchJoinded
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_hasCaptureSwitchJoinded
(JNIEnv* env, jobject obj)
{
	snd_mixer_elem_t*	handle;
	int			nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_hasCaptureSwitchJoinded(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_mixer_selem_has_capture_switch_joined(handle);
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_hasCaptureSwitchJoinded(): end\n"); }
	return (jboolean) nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaMixerElement
 * Method:    hasCaptureSwitchExclusive
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_hasCaptureSwitchExclusive
(JNIEnv* env, jobject obj)
{
	snd_mixer_elem_t*	handle;
	int			nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_hasCaptureSwitchExclusive(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_mixer_selem_has_capture_switch_exclusive(handle);
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_hasCaptureSwitchExclusive(): end\n"); }
	return (jboolean) nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaMixerElement
 * Method:    getPlaybackVolume
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_getPlaybackVolume
(JNIEnv* env, jobject obj, jint nChannelType)
{
	snd_mixer_elem_t*	handle;
	int			nReturn;
	long			lValue;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_getPlaybackVolume(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_mixer_selem_get_playback_volume(handle, (snd_mixer_selem_channel_id_t) nChannelType, &lValue);
	if (nReturn < 0)
	{
		throwRuntimeException(env, snd_strerror(nReturn));
	}
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_getPlaybackVolume(): end\n"); }
	return lValue;
}

/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaMixerElement
 * Method:    getCaptureVolume
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_getCaptureVolume
(JNIEnv* env, jobject obj, jint nChannelType)
{
	snd_mixer_elem_t*	handle;
	int			nReturn;
	long			lValue;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_getCaptureVolume(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_mixer_selem_get_capture_volume(handle, (snd_mixer_selem_channel_id_t) nChannelType, &lValue);
	if (nReturn < 0)
	{
		throwRuntimeException(env, snd_strerror(nReturn));
	}
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_getCaptureVolume(): end\n"); }
	return lValue;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaMixerElement
 * Method:    getPlaybackSwitch
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_getPlaybackSwitch
(JNIEnv* env, jobject obj, jint nChannelType)
{
	snd_mixer_elem_t*	handle;
	int			nReturn;
	int			nValue;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_getPlaybackSwitch(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_mixer_selem_get_playback_switch(handle, (snd_mixer_selem_channel_id_t) nChannelType, &nValue);
	if (nReturn < 0)
	{
		throwRuntimeException(env, snd_strerror(nReturn));
	}
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_getPlaybackSwitch(): end\n"); }
	return (jboolean) nValue;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaMixerElement
 * Method:    getCaptureSwitch
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_getCaptureSwitch
(JNIEnv* env, jobject obj, jint nChannelType)
{
	snd_mixer_elem_t*	handle;
	int			nReturn;
	int			nValue;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_getCaptureSwitch(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_mixer_selem_get_capture_switch(handle, (snd_mixer_selem_channel_id_t) nChannelType, &nValue);
	if (nReturn < 0)
	{
		throwRuntimeException(env, snd_strerror(nReturn));
	}
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_getCaptureSwitch(): end\n"); }
	return (jboolean) nValue;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaMixerElement
 * Method:    setPlaybackVolume
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_setPlaybackVolume
(JNIEnv* env, jobject obj, jint nChannelType, jint nValue)
{
	snd_mixer_elem_t*	handle;
	int			nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_setPlaybackVolume(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_mixer_selem_set_playback_volume(handle, (snd_mixer_selem_channel_id_t) nChannelType, nValue);
	if (nReturn < 0)
	{
		throwRuntimeException(env, snd_strerror(nReturn));
	}
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_setPlaybackVolume(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaMixerElement
 * Method:    setCaptureVolume
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_setCaptureVolume
(JNIEnv* env, jobject obj, jint nChannelType, jint nValue)
{
	snd_mixer_elem_t*	handle;
	int			nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_setCaptureVolume(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_mixer_selem_set_capture_volume(handle, (snd_mixer_selem_channel_id_t) nChannelType, nValue);
	if (nReturn < 0)
	{
		throwRuntimeException(env, snd_strerror(nReturn));
	}
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_setCaptureVolume(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaMixerElement
 * Method:    setPlaybackVolumeAll
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_setPlaybackVolumeAll
(JNIEnv* env, jobject obj, jint nValue)
{
	snd_mixer_elem_t*	handle;
	int			nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_setPlaybackVolumeAll(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_mixer_selem_set_playback_volume_all(handle, nValue);
	if (nReturn < 0)
	{
		throwRuntimeException(env, snd_strerror(nReturn));
	}
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_setPlaybackVolumeAll(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaMixerElement
 * Method:    setCaptureVolumeAll
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_setCaptureVolumeAll
(JNIEnv* env, jobject obj, jint nValue)
{
	snd_mixer_elem_t*	handle;
	int			nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_setCaptureVolumeAll(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_mixer_selem_set_capture_volume_all(handle, nValue);
	if (nReturn < 0)
	{
		throwRuntimeException(env, snd_strerror(nReturn));
	}
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_setCaptureVolumeAll(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaMixerElement
 * Method:    setPlaybackSwitch
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_setPlaybackSwitch
(JNIEnv* env, jobject obj, jint nChannelType, jboolean bValue)
{
	snd_mixer_elem_t*	handle;
	int			nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_setPlaybackSwitch(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_mixer_selem_set_playback_switch(handle, (snd_mixer_selem_channel_id_t) nChannelType, bValue);
	if (nReturn < 0)
	{
		throwRuntimeException(env, snd_strerror(nReturn));
	}
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_setPlaybackSwitch(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaMixerElement
 * Method:    setCaptureSwitch
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_setCaptureSwitch
(JNIEnv* env, jobject obj, jint nChannelType, jboolean bValue)
{
	snd_mixer_elem_t*	handle;
	int			nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_setCaptureSwitch(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_mixer_selem_set_capture_switch(handle, (snd_mixer_selem_channel_id_t) nChannelType, bValue);
	if (nReturn < 0)
	{
		throwRuntimeException(env, snd_strerror(nReturn));
	}
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_setCaptureSwitch(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaMixerElement
 * Method:    setPlaybackSwitchAll
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_setPlaybackSwitchAll
(JNIEnv* env, jobject obj, jboolean bValue)
{
	snd_mixer_elem_t*	handle;
	int			nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_setPlaybackSwitchAll(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_mixer_selem_set_playback_switch_all(handle, bValue);
	if (nReturn < 0)
	{
		throwRuntimeException(env, snd_strerror(nReturn));
	}
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_setPlaybackSwitchAll(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaMixerElement
 * Method:    setCaptureSwitchAll
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_setCaptureSwitchAll
(JNIEnv* env, jobject obj, jboolean bValue)
{
	snd_mixer_elem_t*	handle;
	int			nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_setCaptureSwitchAll(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_mixer_selem_set_capture_switch_all(handle, bValue);
	if (nReturn < 0)
	{
		throwRuntimeException(env, snd_strerror(nReturn));
	}
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_setCaptureSwitchAll(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaMixerElement
 * Method:    getPlaybackVolumeRange
 * Signature: ([I)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_getPlaybackVolumeRange
(JNIEnv* env, jobject obj, jintArray anValues)
{
	snd_mixer_elem_t*	handle;
	jint			values[2];

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_getPlaybackVolumeRange(): begin\n"); }
	handle = getHandle(env, obj);
	snd_mixer_selem_get_playback_volume_range(handle, (long*) values, (long*) values + 1);
	checkArrayLength(env, anValues, 2);
	(*env)->SetIntArrayRegion(env, anValues, 0, 2, values);
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_getPlaybackVolumeRange(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaMixerElement
 * Method:    getCaptureVolumeRange
 * Signature: ([I)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_getCaptureVolumeRange
(JNIEnv* env, jobject obj, jintArray anValues)
{
	snd_mixer_elem_t*	handle;
	jint			values[2];

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_getCaptureVolumeRange(): begin\n"); }
	handle = getHandle(env, obj);
	snd_mixer_selem_get_capture_volume_range(handle, (long*) values, (long*) values + 1);
	checkArrayLength(env, anValues, 2);
	(*env)->SetIntArrayRegion(env, anValues, 0, 2, values);
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_getCaptureVolumeRange(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaMixerElement
 * Method:    setPlaybackVolumeRange
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_setPlaybackVolumeRange
(JNIEnv* env, jobject obj, jint nMin, jint nMax)
{
	snd_mixer_elem_t*	handle;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_setPlaybackVolumeRange(): begin\n"); }
	handle = getHandle(env, obj);
	snd_mixer_selem_set_playback_volume_range(handle, nMin, nMax);
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_setPlaybackVolumeRange(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaMixerElement
 * Method:    setCaptureVolumeRange
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_setCaptureVolumeRange
(JNIEnv* env, jobject obj, jint nMin, jint nMax)
{
	snd_mixer_elem_t*	handle;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_setCaptureVolumeRange(): begin\n"); }
	handle = getHandle(env, obj);
	snd_mixer_selem_set_capture_volume_range(handle, nMin, nMax);
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_setCaptureVolumeRange(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaMixerElement
 * Method:    getChannelName
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_getChannelName
(JNIEnv* env, jclass cls UNUSED, jint nChannelType)
{
	const char*		channelName;
	jstring			channelNameObj;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_getChannelName(): begin\n"); }
	channelName = snd_mixer_selem_channel_name((snd_mixer_selem_channel_id_t) nChannelType);
	channelNameObj = (*env)->NewStringUTF(env, channelName);
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_getChannelName(): end\n"); }
	return channelNameObj;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaMixerElement
 * Method:    setTrace
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaMixerElement_setTrace
(JNIEnv* env UNUSED, jclass cls UNUSED, jboolean bTrace)
{
	debug_flag = bTrace;
	debug_file = stderr;
}



/*** org_tritonus_lowlevel_alsa_AlsaMixerElement.c ***/
