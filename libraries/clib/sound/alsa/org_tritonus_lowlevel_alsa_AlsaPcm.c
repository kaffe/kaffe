/*
 *	org_tritonus_lowlevel_alsa_AlsaPcm.c
 */

/*
 *  Copyright (c) 2000 - 2001 by Matthias Pfisterer <Matthias.Pfisterer@gmx.de>
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

#include "common.h"
#include "org_tritonus_lowlevel_alsa_AlsaPcm.h"


// referring to methods in ..AlsaPcm_*.c
snd_pcm_hw_params_t*
getHWParamsNativeHandle(JNIEnv *env, jobject obj);
snd_pcm_format_mask_t*
getFormatMaskNativeHandle(JNIEnv *env, jobject obj);
snd_pcm_sw_params_t*
getSWParamsNativeHandle(JNIEnv *env, jobject obj);


HandleFieldHandler(snd_pcm_t*)



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcm
 * Method:    open
 * Signature: (Ljava/lang/String;II)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcm_open
(JNIEnv *env, jobject obj, jstring strPcmName, jint nDirection, jint nMode)
{
	const char*	name = NULL;
	snd_pcm_t*	handle;
	int		nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_open(): begin\n"); }
	name = (*env)->GetStringUTFChars(env, strPcmName, NULL);
	if (name == NULL)
	{
		throwRuntimeException(env, "cannot get characters from string argument");
	}
	if (debug_flag)
	{
	fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_open(): name: %s\n", name);
	fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_open(): direction: %d\n", (int) nDirection);
	fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_open(): mode: %d\n", (int) nMode);
	}
	nReturn = snd_pcm_open(&handle, name, (snd_pcm_stream_t) nDirection, nMode);
	(*env)->ReleaseStringUTFChars(env, strPcmName, name);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_open(): handle: %p\n", handle); }
	if (nReturn >= 0)
	{
		setHandle(env, obj, handle);
	}
	else
	{
		if (debug_flag) { fprintf(debug_file, "org_tritonus_lowlevel_alsa_AlsaPcm.open(): returns %d\n", nReturn); }
	}
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_open(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcm
 * Method:    close
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcm_close
(JNIEnv *env, jobject obj)
{
	snd_pcm_t*	handle;
	int		nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_close(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_pcm_close(handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_close(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcm
 * Method:    getAnyHWParams
 * Signature: (Lorg/tritonus/lowlevel/alsa/AlsaPcm$HWParams;)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcm_getAnyHWParams
(JNIEnv *env, jobject obj, jobject objHwParams)
{
	snd_pcm_t*		handle;
	snd_pcm_hw_params_t*	hwParams;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_getAnyHWParams(): begin\n"); }
	handle = getHandle(env, obj);
	hwParams = getHWParamsNativeHandle(env, objHwParams);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_getAnyHWParams(): hwparams handle: %p\n", hwParams); }
	nReturn = snd_pcm_hw_params_any(handle, hwParams);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_getAnyHWParams(): snd_pcm_hw_params_any() returns: %d\n", nReturn); }
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_getAnyHWParams(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcm
 * Method:    setHWParamsAccess
 * Signature: (Lorg/tritonus/lowlevel/alsa/AlsaPcm$HWParams;I)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcm_setHWParamsAccess
(JNIEnv *env, jobject obj, jobject objHwParams, jint nAccess)
{
	snd_pcm_t*		handle;
	snd_pcm_hw_params_t*	hwParams;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_setHWParamsAccess(): begin\n"); }
	handle = getHandle(env, obj);
	hwParams = getHWParamsNativeHandle(env, objHwParams);
	nReturn = snd_pcm_hw_params_set_access(handle, hwParams, (snd_pcm_access_t) nAccess);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_setHWParamsAccess(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcm
 * Method:    setHWParamsFormat
 * Signature: (Lorg/tritonus/lowlevel/alsa/AlsaPcm$HWParams;I)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcm_setHWParamsFormat
(JNIEnv *env, jobject obj, jobject objHwParams, jint nFormat)
{
	snd_pcm_t*		handle;
	snd_pcm_hw_params_t*	hwParams;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_setHWParamsFormat(): begin\n"); }
	handle = getHandle(env, obj);
	hwParams = getHWParamsNativeHandle(env, objHwParams);
	nReturn = snd_pcm_hw_params_set_format(handle, hwParams, (snd_pcm_format_t) nFormat);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_setHWParamsFormat(): end\n"); }
	return nReturn;
}

/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcm
 * Method:    setHWParamsFormatMask
 * Signature: (Lorg/tritonus/lowlevel/alsa/AlsaPcm$HWParams;Lorg/tritonus/lowlevel/alsa/AlsaPcm$HWParams$FormatMask;)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcm_setHWParamsFormatMask
(JNIEnv *env, jobject obj, jobject objHwParams, jobject objFormatMask)
{
	snd_pcm_t*		handle;
	snd_pcm_hw_params_t*	hwParams;
	snd_pcm_format_mask_t*	formatMask;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_setHWParamsFormatMask(): begin\n"); }
	handle = getHandle(env, obj);
	hwParams = getHWParamsNativeHandle(env, objHwParams);
	formatMask = getFormatMaskNativeHandle(env, objFormatMask);
	nReturn = snd_pcm_hw_params_set_format_mask(handle, hwParams, formatMask);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_setHWParamsFormatMask(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcm
 * Method:    setHWParamsChannels
 * Signature: (Lorg/tritonus/lowlevel/alsa/AlsaPcm$HWParams;I)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcm_setHWParamsChannels
(JNIEnv *env, jobject obj, jobject objHwParams, jint nChannels)
{
	snd_pcm_t*		handle;
	snd_pcm_hw_params_t*	hwParams;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_setHWParamsChannels(): begin\n"); }
	handle = getHandle(env, obj);
	hwParams = getHWParamsNativeHandle(env, objHwParams);
	nReturn = snd_pcm_hw_params_set_channels(handle, hwParams, nChannels);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_setHWParamsChannels(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcm
 * Method:    setHWParamsRateNear
 * Signature: (Lorg/tritonus/lowlevel/alsa/AlsaPcm$HWParams;I)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcm_setHWParamsRateNear
(JNIEnv *env, jobject obj, jobject objHwParams, jint nRate)
{
	snd_pcm_t*		handle;
	snd_pcm_hw_params_t*	hwParams;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_setHWParamsRateNear(): begin\n"); }
	handle = getHandle(env, obj);
	hwParams = getHWParamsNativeHandle(env, objHwParams);
	nReturn = snd_pcm_hw_params_set_rate_near(handle, hwParams,
											  (unsigned int*)&nRate, 0);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_setHWParamsRateNear(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcm
 * Method:    setHWParamsBufferTimeNear
 * Signature: (Lorg/tritonus/lowlevel/alsa/AlsaPcm$HWParams;I)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcm_setHWParamsBufferTimeNear
(JNIEnv *env, jobject obj, jobject objHwParams, jint nBufferTime)
{
	snd_pcm_t*		handle;
	snd_pcm_hw_params_t*	hwParams;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_setHWParamsBufferTimeNear(): begin\n"); }
	handle = getHandle(env, obj);
	hwParams = getHWParamsNativeHandle(env, objHwParams);
	nReturn = snd_pcm_hw_params_set_buffer_time_near(handle, hwParams,
													 (unsigned int*) &nBufferTime, 0);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_setHWParamsBufferTimeNear(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcm
 * Method:    setHWParamsPeriodTimeNear
 * Signature: (Lorg/tritonus/lowlevel/alsa/AlsaPcm$HWParams;I)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcm_setHWParamsPeriodTimeNear
(JNIEnv *env, jobject obj, jobject objHwParams, jint nPeriodTime)
{
	snd_pcm_t*		handle;
	snd_pcm_hw_params_t*	hwParams;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_setHWParamsPeriodTimeNear(): begin\n"); }
	handle = getHandle(env, obj);
	hwParams = getHWParamsNativeHandle(env, objHwParams);
	nReturn = snd_pcm_hw_params_set_period_time_near(handle, hwParams,
													 (unsigned int*) &nPeriodTime, 0);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_setHWParamsPeriodTimeNear(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcm
 * Method:    setHWParams
 * Signature: (Lorg/tritonus/lowlevel/alsa/AlsaPcm$HWParams;)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcm_setHWParams
(JNIEnv *env, jobject obj, jobject objHwParams)
{
	snd_pcm_t*		handle;
	snd_pcm_hw_params_t*	hwParams;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_setHWParams(): begin\n"); }
	handle = getHandle(env, obj);
	hwParams = getHWParamsNativeHandle(env, objHwParams);
	nReturn = snd_pcm_hw_params(handle, hwParams);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_setHWParams(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcm
 * Method:    getHWParamsPeriodSize
 * Signature: (Lorg/tritonus/lowlevel/alsa/AlsaPcm$HWParams;)I
 */
/* JNIEXPORT jint JNICALL */
/* Java_org_tritonus_lowlevel_alsa_AlsaPcm_getHWParamsPeriodSize */
/* (JNIEnv *env, jobject obj, jobject objHwParams) */
/* { */
/* 	snd_pcm_t*		handle; */
/* 	snd_pcm_hw_params_t*	hwParams; */
/* 	int			nReturn; */

/* 	handle = getHandle(env, obj); */
/* 	hwParams = getHWParamsNativeHandle(env, objHwParams); */
/* 	nReturn = snd_pcm_hw_params_get_period_size(handle, hwParams, NULL); */
/* 	return nReturn; */
/* } */



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcm
 * Method:    getHWParamsBufferSize
 * Signature: (Lorg/tritonus/lowlevel/alsa/AlsaPcm$HWParams;)I
 */
/* JNIEXPORT jint JNICALL */
/* Java_org_tritonus_lowlevel_alsa_AlsaPcm_getHWParamsBufferSize */
/* (JNIEnv *env, jobject obj, jobject objHwParams) */
/* { */
/* 	snd_pcm_t*		handle; */
/* 	snd_pcm_hw_params_t*	hwParams; */
/* 	int			nReturn; */

/* 	handle = getHandle(env, obj); */
/* 	hwParams = getHWParamsNativeHandle(env, objHwParams); */
/* 	nReturn = snd_pcm_hw_params_get_buffer_size(handle, hwParams); */
/* 	return nReturn; */
/* } */


/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcm
 * Method:    getSWParams
 * Signature: (Lorg/tritonus/lowlevel/alsa/AlsaPcm$SWParams;)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcm_getSWParams
(JNIEnv *env, jobject obj, jobject objSwParams)
{
	snd_pcm_t*		handle;
	snd_pcm_sw_params_t*	swParams;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_getSWParams(): begin\n"); }
	handle = getHandle(env, obj);
	swParams = getSWParamsNativeHandle(env, objSwParams);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_getSWParams(): swparams handle: %p\n", swParams); }
	nReturn = snd_pcm_sw_params_current(handle, swParams);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_getSWParams(): snd_pcm_sw_params_any() returns: %d\n", nReturn); }
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_getSWParams(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcm
 * Method:    setSWParamsStartMode
 * Signature: (Lorg/tritonus/lowlevel/alsa/AlsaPcm$SWParams;I)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcm_setSWParamsStartMode
(JNIEnv *env, jobject obj, jobject objSwParams, jint nStartMode)
{
	snd_pcm_t*		handle;
	snd_pcm_sw_params_t*	swParams;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_setSWParamsStartMode(): begin\n"); }
	handle = getHandle(env, obj);
	swParams = getSWParamsNativeHandle(env, objSwParams);
	nReturn = snd_pcm_sw_params_set_start_mode(handle, swParams, (snd_pcm_start_t) nStartMode);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_setSWParamsStartMode(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcm
 * Method:    setSWParamsXrunMode
 * Signature: (Lorg/tritonus/lowlevel/alsa/AlsaPcm$SWParams;I)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcm_setSWParamsXrunMode
(JNIEnv *env, jobject obj, jobject objSwParams, jint nXrunMode)
{
	snd_pcm_t*		handle;
	snd_pcm_sw_params_t*	swParams;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_setSWParamsXrunMode(): begin\n"); }
	handle = getHandle(env, obj);
	swParams = getSWParamsNativeHandle(env, objSwParams);
	nReturn = snd_pcm_sw_params_set_xrun_mode(handle, swParams, (snd_pcm_xrun_t) nXrunMode);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_setSWParamsXrunMode(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcm
 * Method:    setSWParamsTStampMode
 * Signature: (Lorg/tritonus/lowlevel/alsa/AlsaPcm$SWParams;I)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcm_setSWParamsTStampMode
(JNIEnv *env, jobject obj, jobject objSwParams, jint nTStampMode)
{
	snd_pcm_t*		handle;
	snd_pcm_sw_params_t*	swParams;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_setSWParamsTStampMode(): begin\n"); }
	handle = getHandle(env, obj);
	swParams = getSWParamsNativeHandle(env, objSwParams);
	nReturn = snd_pcm_sw_params_set_tstamp_mode(handle, swParams, (snd_pcm_tstamp_t) nTStampMode);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_setSWParamsTStampMode(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcm
 * Method:    setSWParamsSleepMin
 * Signature: (Lorg/tritonus/lowlevel/alsa/AlsaPcm$SWParams;I)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcm_setSWParamsSleepMin
(JNIEnv *env, jobject obj, jobject objSwParams, jint nSleepMin)
{
	snd_pcm_t*		handle;
	snd_pcm_sw_params_t*	swParams;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_setSWParamsSleepMin(): begin\n"); }
	handle = getHandle(env, obj);
	swParams = getSWParamsNativeHandle(env, objSwParams);
	nReturn = snd_pcm_sw_params_set_sleep_min(handle, swParams, nSleepMin);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_setSWParamsSleepMin(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcm
 * Method:    setSWParamsAvailMin
 * Signature: (Lorg/tritonus/lowlevel/alsa/AlsaPcm$SWParams;I)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcm_setSWParamsAvailMin
(JNIEnv *env, jobject obj, jobject objSwParams, jint nAvailMin)
{
	snd_pcm_t*		handle;
	snd_pcm_sw_params_t*	swParams;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_setSWParamsAvailMin(): begin\n"); }
	handle = getHandle(env, obj);
	swParams = getSWParamsNativeHandle(env, objSwParams);
	nReturn = snd_pcm_sw_params_set_avail_min(handle, swParams, nAvailMin);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_setSWParamsAvailMin(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcm
 * Method:    setSWParamsXferAlign
 * Signature: (Lorg/tritonus/lowlevel/alsa/AlsaPcm$SWParams;I)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcm_setSWParamsXferAlign
(JNIEnv *env, jobject obj, jobject objSwParams, jint nXferAlign)
{
	snd_pcm_t*		handle;
	snd_pcm_sw_params_t*	swParams;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_setSWParamsXferAlign(): begin\n"); }
	handle = getHandle(env, obj);
	swParams = getSWParamsNativeHandle(env, objSwParams);
	nReturn = snd_pcm_sw_params_set_xfer_align(handle, swParams, nXferAlign);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_setSWParamsXferAlign(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcm
 * Method:    setSWParamsStartThreshold
 * Signature: (Lorg/tritonus/lowlevel/alsa/AlsaPcm$SWParams;I)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcm_setSWParamsStartThreshold
(JNIEnv *env, jobject obj, jobject objSwParams, jint nStartThreshold)
{
	snd_pcm_t*		handle;
	snd_pcm_sw_params_t*	swParams;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_setSWParamsStartThreshold(): begin\n"); }
	handle = getHandle(env, obj);
	swParams = getSWParamsNativeHandle(env, objSwParams);
	nReturn = snd_pcm_sw_params_set_start_threshold(handle, swParams, nStartThreshold);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_setSWParamsStartThreshold(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcm
 * Method:    setSWParamsStopThreshold
 * Signature: (Lorg/tritonus/lowlevel/alsa/AlsaPcm$SWParams;I)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcm_setSWParamsStopThreshold
(JNIEnv *env, jobject obj, jobject objSwParams, jint nStopThreshold)
{
	snd_pcm_t*		handle;
	snd_pcm_sw_params_t*	swParams;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_setSWParamsStopThreshold(): begin\n"); }
	handle = getHandle(env, obj);
	swParams = getSWParamsNativeHandle(env, objSwParams);
	nReturn = snd_pcm_sw_params_set_stop_threshold(handle, swParams, nStopThreshold);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_setSWParamsStopThreshold(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcm
 * Method:    setSWParamsSilenceThreshold
 * Signature: (Lorg/tritonus/lowlevel/alsa/AlsaPcm$SWParams;I)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcm_setSWParamsSilenceThreshold
(JNIEnv *env, jobject obj, jobject objSwParams, jint nSilenceThreshold)
{
	snd_pcm_t*		handle;
	snd_pcm_sw_params_t*	swParams;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_setSWParamsSilenceThreshold(): begin\n"); }
	handle = getHandle(env, obj);
	swParams = getSWParamsNativeHandle(env, objSwParams);
	nReturn = snd_pcm_sw_params_set_silence_threshold(handle, swParams, nSilenceThreshold);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_setSWParamsSilenceThreshold(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcm
 * Method:    setSWParamsSilenceSize
 * Signature: (Lorg/tritonus/lowlevel/alsa/AlsaPcm$SWParams;I)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcm_setSWParamsSilenceSize
(JNIEnv *env, jobject obj, jobject objSwParams, jint nSilenceSize)
{
	snd_pcm_t*		handle;
	snd_pcm_sw_params_t*	swParams;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_setSWParamsSilenceSize(): begin\n"); }
	handle = getHandle(env, obj);
	swParams = getSWParamsNativeHandle(env, objSwParams);
	nReturn = snd_pcm_sw_params_set_silence_size(handle, swParams, nSilenceSize);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_setSWParamsSilenceSize(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcm
 * Method:    setSWParams
 * Signature: (Lorg/tritonus/lowlevel/alsa/AlsaPcm$SWParams;)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcm_setSWParams
(JNIEnv *env, jobject obj, jobject objSwParams)
{
	snd_pcm_t*		handle;
	snd_pcm_sw_params_t*	swParams;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_setSWParams(): begin\n"); }
	handle = getHandle(env, obj);
	swParams = getSWParamsNativeHandle(env, objSwParams);
	nReturn = snd_pcm_sw_params(handle, swParams);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_setSWParams(): end\n"); }
	return nReturn;
}







/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcm
 * Method:    writei
 * Signature: ([BJJ)J
 */
JNIEXPORT jlong JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcm_writei
(JNIEnv *env, jobject obj, jbyteArray abData, jlong lOffset, jlong lFrameCount)
{
	snd_pcm_t*	handle;
	signed char*	data;
	snd_pcm_sframes_t	lWritten;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_writei(): begin\n"); }
	handle = getHandle(env, obj);
	data = (*env)->GetByteArrayElements(env, abData, NULL);
	if (data == NULL)
	{
 		throwRuntimeException(env, "GetByteArrayElements() failed");
	}
	if (debug_flag) { printf("Java_org_tritonus_lowlevel_alsa_AlsaPcm_writei(): trying to write (frames): %lld\n", lFrameCount); }
	lWritten = snd_pcm_writei(handle, data + lOffset, lFrameCount);
	if (debug_flag) { printf("Java_org_tritonus_lowlevel_alsa_AlsaPcm_writei(): Written: %ld\n", lWritten); }
	// we can do a JNI_ABORT because we know the data wasn't altered.
	// This may improve performance (no copying back).
	(*env)->ReleaseByteArrayElements(env, abData, data, JNI_ABORT);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_writei(): end\n"); }
	return lWritten;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcm
 * Method:    readi
 * Signature: ([BJJ)J
 */
JNIEXPORT jlong JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcm_readi
(JNIEnv *env, jobject obj, jbyteArray abData, jlong lOffset, jlong lFrameCount)
{
	snd_pcm_t*		handle;
	signed char*		data;
	snd_pcm_sframes_t	lRead;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_readi(): begin\n"); }
	handle = getHandle(env, obj);
	data = (*env)->GetByteArrayElements(env, abData, NULL);
	printf("native array: %p\n", data);
	if (data == NULL)
	{
 		throwRuntimeException(env, "GetByteArrayElements() failed");
	}
	if (debug_flag) { printf("Java_org_tritonus_lowlevel_alsa_AlsaPcm_readi(): trying to read (frames): %lld\n", lFrameCount); }
	// To the ALSA method, frame count means number of complete PCM frames.
	// Frame: sample data vector for all channels. For 16 Bit stereo data, one frame has a length of four bytes.
	lRead = snd_pcm_readi(handle, data + lOffset, lFrameCount);
	if (debug_flag) { printf("Java_org_tritonus_lowlevel_alsa_AlsaPcm_readi(): Read: %ld\n", lRead); }
	// printf("1\n");
	(*env)->ReleaseByteArrayElements(env, abData, data, 0);
	// printf("2\n");
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcm_readi(): end\n"); }
	return lRead;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcm
 * Method:    setTrace
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcm_setTrace
(JNIEnv* env UNUSED, jclass cls UNUSED, jboolean bTrace)
{
	debug_flag = bTrace;
	debug_file = stderr;
}



/*** org_tritonus_lowlevel_alsa_AlsaPcm.c ***/
