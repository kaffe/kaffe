/*
 *	org_tritonus_lowlevel_alsa_AlsaPcmSWParams.c
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
#include "org_tritonus_lowlevel_alsa_AlsaPcmSWParams.h"


HandleFieldHandler(snd_pcm_sw_params_t*)



snd_pcm_sw_params_t*
getSWParamsNativeHandle(JNIEnv *env, jobject obj)
{
	return getHandle(env, obj);
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcmSWParams
 * Method:    malloc
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcmSWParams_malloc
(JNIEnv *env, jobject obj)
{
	snd_pcm_sw_params_t*	handle;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmSWParams_malloc(): begin\n"); }
	nReturn = snd_pcm_sw_params_malloc(&handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmSWParams_malloc(): handle: %p\n", handle); }
	setHandle(env, obj, handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmSWParams_malloc(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcmSWParams
 * Method:    free
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcmSWParams_free
(JNIEnv *env, jobject obj)
{
	snd_pcm_sw_params_t*	handle;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmSWParams_free(): begin\n"); }
	handle = getHandle(env, obj);
	snd_pcm_sw_params_free(handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmSWParams_free(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcmSWParams
 * Method:    getStartMode
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcmSWParams_getStartMode
(JNIEnv *env, jobject obj)
{
	snd_pcm_sw_params_t*	handle;
	int			nReturn;

	handle = getHandle(env, obj);
	nReturn = snd_pcm_sw_params_get_start_mode(handle);
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcmSWParams
 * Method:    getXrunMode
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcmSWParams_getXrunMode
(JNIEnv *env, jobject obj)
{
	snd_pcm_sw_params_t*	handle;
	int			nReturn;

	handle = getHandle(env, obj);
	nReturn = snd_pcm_sw_params_get_xrun_mode(handle);
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcmSWParams
 * Method:    getTStampMode
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcmSWParams_getTStampMode
(JNIEnv *env, jobject obj)
{
	snd_pcm_sw_params_t*	handle;
	snd_pcm_tstamp_t	nValue;
	int					nReturn;

	handle = getHandle(env, obj);
	nReturn = snd_pcm_sw_params_get_tstamp_mode(handle, &nValue);
	return nValue;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcmSWParams
 * Method:    getSleepMin
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcmSWParams_getSleepMin
(JNIEnv *env, jobject obj)
{
	snd_pcm_sw_params_t*	handle;
	unsigned int	nValue;
	int			nReturn;

	handle = getHandle(env, obj);
	nReturn = snd_pcm_sw_params_get_sleep_min(handle, &nValue);
	return nValue;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcmSWParams
 * Method:    getAvailMin
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcmSWParams_getAvailMin
(JNIEnv *env, jobject obj)
{
	snd_pcm_sw_params_t*	handle;
	snd_pcm_uframes_t	nValue;
	int					nReturn;

	handle = getHandle(env, obj);
	nReturn = snd_pcm_sw_params_get_avail_min(handle, &nValue);
	return nValue;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcmSWParams
 * Method:    getXferAlign
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcmSWParams_getXferAlign
(JNIEnv *env, jobject obj)
{
	snd_pcm_sw_params_t*	handle;
	snd_pcm_uframes_t	nValue;
	int					nReturn;

	handle = getHandle(env, obj);
	nReturn = snd_pcm_sw_params_get_xfer_align(handle, &nValue);
	return nValue;
}




/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcmSWParams
 * Method:    getStartThreshold
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcmSWParams_getStartThreshold
(JNIEnv *env, jobject obj)
{
	snd_pcm_sw_params_t*	handle;
	snd_pcm_uframes_t	nValue;
	int					nReturn;

	handle = getHandle(env, obj);
	nReturn = snd_pcm_sw_params_get_start_threshold(handle, &nValue);
	return nValue;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcmSWParams
 * Method:    getStopThreshold
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcmSWParams_getStopThreshold
(JNIEnv *env, jobject obj)
{
	snd_pcm_sw_params_t*	handle;
	snd_pcm_uframes_t	nValue;
	int					nReturn;

	handle = getHandle(env, obj);
	nReturn = snd_pcm_sw_params_get_stop_threshold(handle, &nValue);
	return nValue;
}


/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcmSWParams
 * Method:    getSilenceThreshold
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcmSWParams_getSilenceThreshold
(JNIEnv *env, jobject obj)
{
	snd_pcm_sw_params_t*	handle;
	snd_pcm_uframes_t	nValue;
	int					nReturn;

	handle = getHandle(env, obj);
	nReturn = snd_pcm_sw_params_get_silence_threshold(handle, &nValue);
	return nValue;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcmSWParams
 * Method:    getSilenceSize
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcmSWParams_getSilenceSize
(JNIEnv *env, jobject obj)
{
	snd_pcm_sw_params_t*	handle;
	snd_pcm_uframes_t	nValue;
	int					nReturn;

	handle = getHandle(env, obj);
	nReturn = snd_pcm_sw_params_get_silence_size(handle, &nValue);
	return nValue;
}



/*** org_tritonus_lowlevel_alsa_AlsaPcmSWParams.c ***/
