/*
 *	org_tritonus_lowlevel_alsa_AlsaPcmHWParams.c
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

#include "org_tritonus_lowlevel_alsa_AlsaPcmHWParams.h"
#include "common.h"


snd_pcm_format_mask_t*
getFormatMaskNativeHandle(JNIEnv* env, jobject obj);


HandleFieldHandler(snd_pcm_hw_params_t*)



/*
 */
static void
setDirection(JNIEnv* env, jint nDirection, jintArray anDirection)
{
	if (anDirection != NULL)
	{
		checkArrayLength(env, anDirection, 1);
		(*env)->SetIntArrayRegion(env, anDirection, 0, 1, &nDirection);
	}
}




snd_pcm_hw_params_t*
getHWParamsNativeHandle(JNIEnv* env, jobject obj)
{
	return getHandle(env, obj);
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcmHWParams
 * Method:    malloc
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_malloc
(JNIEnv* env, jobject obj)
{
	snd_pcm_hw_params_t*	handle;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_malloc(): begin\n"); }
	nReturn = snd_pcm_hw_params_malloc(&handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_malloc(): handle: %p\n", handle); }
	setHandle(env, obj, handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_malloc(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcmHWParams
 * Method:    free
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_free
(JNIEnv* env, jobject obj)
{
	snd_pcm_hw_params_t*	handle;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_free(): begin\n"); }
	handle = getHandle(env, obj);
	snd_pcm_hw_params_free(handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_free(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcmHWParams
 * Method:    getRate
 * Signature: ([J)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getRate___3J
(JNIEnv* env, jobject obj, jlongArray alValues)
{
	snd_pcm_hw_params_t*	handle;
	int			nReturn;
	unsigned int		nNumerator;
	unsigned int		nDenominator;
	jlong			values[2];

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getRate(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_pcm_hw_params_get_rate_numden(handle, &nNumerator, &nDenominator);
	checkArrayLength(env, alValues, 2);
	values[0] = nNumerator;
	values[1] = nDenominator;
	(*env)->SetLongArrayRegion(env, alValues, 0, 2, values);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getRate(): end\n"); }
	return nReturn;
}

/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcmHWParams
 * Method:    getSBits
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getSBits
(JNIEnv* env, jobject obj)
{
	snd_pcm_hw_params_t*	handle;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getSBits(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_pcm_hw_params_get_sbits(handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getSBits(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcmHWParams
 * Method:    getFifoSize
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getFifoSize
(JNIEnv* env, jobject obj)
{
	snd_pcm_hw_params_t*	handle;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getFifoSize(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_pcm_hw_params_get_fifo_size(handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getFifoSize(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcmHWParams
 * Method:    getAccess
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getAccess
(JNIEnv* env, jobject obj)
{
	snd_pcm_hw_params_t*	handle;
	snd_pcm_access_t		access;
	int						nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getAccess(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_pcm_hw_params_get_access(handle, &access);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getAccess(): end\n"); }
	return access;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcmHWParams
 * Method:    getFormat
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getFormat
(JNIEnv* env, jobject obj)
{
	snd_pcm_hw_params_t*	handle;
	snd_pcm_format_t	nValue;
	int					nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getFormat(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_pcm_hw_params_get_format(handle, &nValue);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getFormat(): end\n"); }
	return nValue;
}


/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcmHWParams
 * Method:    getFormatMask
 * Signature: (Lorg/tritonus/lowlevel/alsa/AlsaPcm$HWParams$FormatMask;)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getFormatMask
(JNIEnv* env, jobject obj, jobject objFormatMask)
{
	snd_pcm_hw_params_t*	handle;
	snd_pcm_format_mask_t*	formatMask;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getFormatMask(): begin\n"); }
	handle = getHandle(env, obj);
	formatMask = getFormatMaskNativeHandle(env, objFormatMask);
	snd_pcm_hw_params_get_format_mask(handle, formatMask);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getFormatMask(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcmHWParams
 * Method:    getSubformat
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getSubformat
(JNIEnv* env, jobject obj)
{
	snd_pcm_hw_params_t*	handle;
	snd_pcm_subformat_t	nValue;
	int					nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getSubformat(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_pcm_hw_params_get_subformat(handle, &nValue);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getSubformat(): end\n"); }
	return nValue;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcmHWParams
 * Method:    getChannels
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getChannels
(JNIEnv* env, jobject obj)
{
	snd_pcm_hw_params_t*	handle;
	unsigned int	nValue;
	int				nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getChannels(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_pcm_hw_params_get_channels(handle, &nValue);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getChannels(): end\n"); }
	return nValue;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcmHWParams
 * Method:    getChannelsMin
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getChannelsMin
(JNIEnv* env, jobject obj)
{
	snd_pcm_hw_params_t*	handle;
	unsigned int	nValue;
	int				nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getChannelsMin(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_pcm_hw_params_get_channels_min(handle, &nValue);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getChannelsMin(): end\n"); }
	return nValue;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcmHWParams
 * Method:    getChannelsMax
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getChannelsMax
(JNIEnv* env, jobject obj)
{
	snd_pcm_hw_params_t*	handle;
	unsigned int	nValue;
	int				nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getChannelsMax(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_pcm_hw_params_get_channels_max(handle, &nValue);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getChannelsMax(): end\n"); }
	return nValue;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcmHWParams
 * Method:    getRate
 * Signature: ([I)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getRate___3I
(JNIEnv* env, jobject obj, jintArray anDirection)
{
	snd_pcm_hw_params_t*	handle;
	int				nReturn;
	unsigned int	nValue;
	int				nDirection;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getRate(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_pcm_hw_params_get_rate(handle, &nValue, &nDirection);
	setDirection(env, nDirection, anDirection);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getRate(): end\n"); }
	return nValue;
}

/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcmHWParams
 * Method:    getRateMin
 * Signature: ([I)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getRateMin
(JNIEnv* env, jobject obj, jintArray anDirection)
{
	snd_pcm_hw_params_t*	handle;
	int				nReturn;
	unsigned int	nValue;
	int				nDirection;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getRateMin(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_pcm_hw_params_get_rate_min(handle, &nValue, &nDirection);
	setDirection(env, nDirection, anDirection);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getRateMin(): end\n"); }
	return nValue;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcmHWParams
 * Method:    getRateMax
 * Signature: ([I)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getRateMax
(JNIEnv* env, jobject obj, jintArray anDirection)
{
	snd_pcm_hw_params_t*	handle;
	int				nReturn;
	unsigned int	nValue;
	int				nDirection;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getRateMax(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_pcm_hw_params_get_rate_max(handle, &nValue, &nDirection);
	setDirection(env, nDirection, anDirection);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getRateMax(): end\n"); }
	return nValue;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcmHWParams
 * Method:    getPeriodTime
 * Signature: ([I)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getPeriodTime
(JNIEnv* env, jobject obj, jintArray anDirection)
{
	snd_pcm_hw_params_t*	handle;
	int				nReturn;
	unsigned int	nValue;
	int				nDirection;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getPeriodTime(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_pcm_hw_params_get_period_time(handle, &nValue, &nDirection);
	setDirection(env, nDirection, anDirection);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getPeriodTime(): end\n"); }
	return nValue;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcmHWParams
 * Method:    getPeriodTimeMin
 * Signature: ([I)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getPeriodTimeMin
(JNIEnv* env, jobject obj, jintArray anDirection)
{
	snd_pcm_hw_params_t*	handle;
	int			nReturn;
	unsigned int	nValue;
	int			nDirection;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getPeriodTimeMin(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_pcm_hw_params_get_period_time_min(handle, &nValue, &nDirection);
	setDirection(env, nDirection, anDirection);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getPeriodTimeMin(): end\n"); }
	return nValue;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcmHWParams
 * Method:    getPeriodTimeMax
 * Signature: ([I)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getPeriodTimeMax
(JNIEnv* env, jobject obj, jintArray anDirection)
{
	snd_pcm_hw_params_t*	handle;
	int				nReturn;
	unsigned int	nValue;
	int				nDirection;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getPeriodTimeMax(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_pcm_hw_params_get_period_time_max(handle, &nValue, &nDirection);
	setDirection(env, nDirection, anDirection);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getPeriodTimeMax(): end\n"); }
	return nValue;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcmHWParams
 * Method:    getPeriodSize
 * Signature: ([I)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getPeriodSize
(JNIEnv* env, jobject obj, jintArray anDirection)
{
	snd_pcm_hw_params_t*	handle;
	int					nReturn;
	snd_pcm_uframes_t	nValue;
	int					nDirection;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getPeriodSize(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_pcm_hw_params_get_period_size(handle, &nValue, &nDirection);
	setDirection(env, nDirection, anDirection);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getPeriodSize(): end\n"); }
	return nValue;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcmHWParams
 * Method:    getPeriodSizeMin
 * Signature: ([I)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getPeriodSizeMin
(JNIEnv* env, jobject obj, jintArray anDirection)
{
	snd_pcm_hw_params_t*	handle;
	int					nReturn;
	snd_pcm_uframes_t	nValue;
	int					nDirection;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getPeriodSizeMin(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_pcm_hw_params_get_period_size_min(handle, &nValue, &nDirection);
	setDirection(env, nDirection, anDirection);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getPeriodSizeMin(): end\n"); }
	return nValue;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcmHWParams
 * Method:    getPeriodSizeMax
 * Signature: ([I)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getPeriodSizeMax
(JNIEnv* env, jobject obj, jintArray anDirection)
{
	snd_pcm_hw_params_t*	handle;
	int					nReturn;
	snd_pcm_uframes_t	nValue;
	int					nDirection;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getPeriodSizeMax(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_pcm_hw_params_get_period_size_max(handle, &nValue, &nDirection);
	setDirection(env, nDirection, anDirection);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getPeriodSizeMax(): end\n"); }
	return nValue;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcmHWParams
 * Method:    getPeriods
 * Signature: ([I)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getPeriods
(JNIEnv* env, jobject obj, jintArray anDirection)
{
	snd_pcm_hw_params_t*	handle;
	int				nReturn;
	unsigned int	nValue;
	int				nDirection;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getPeriods(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_pcm_hw_params_get_periods(handle, &nValue, &nDirection);
	setDirection(env, nDirection, anDirection);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getPeriods(): end\n"); }
	return nValue;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcmHWParams
 * Method:    getPeriodsMin
 * Signature: ([I)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getPeriodsMin
(JNIEnv* env, jobject obj, jintArray anDirection)
{
	snd_pcm_hw_params_t*	handle;
	int				nReturn;
	unsigned int	nValue;
	int				nDirection;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getPeriodsMin(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_pcm_hw_params_get_periods_min(handle, &nValue, &nDirection);
	setDirection(env, nDirection, anDirection);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getPeriodsMin(): end\n"); }
	return nValue;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcmHWParams
 * Method:    getPeriodsMax
 * Signature: ([I)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getPeriodsMax
(JNIEnv* env, jobject obj, jintArray anDirection)
{
	snd_pcm_hw_params_t*	handle;
	int				nReturn;
	unsigned int	nValue;
	int				nDirection;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getPeriodsMax(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_pcm_hw_params_get_periods_max(handle, &nValue, &nDirection);
	setDirection(env, nDirection, anDirection);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getPeriodsMax(): end\n"); }
	return nValue;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcmHWParams
 * Method:    getBufferTime
 * Signature: ([I)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getBufferTime
(JNIEnv* env, jobject obj, jintArray anDirection)
{
	snd_pcm_hw_params_t*	handle;
	int				nReturn;
	unsigned int	nValue;
	int				nDirection;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getBufferTime(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_pcm_hw_params_get_buffer_time(handle, &nValue, &nDirection);
	setDirection(env, nDirection, anDirection);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getBufferTime(): end\n"); }
	return nValue;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcmHWParams
 * Method:    getBufferTimeMin
 * Signature: ([I)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getBufferTimeMin
(JNIEnv* env, jobject obj, jintArray anDirection)
{
	snd_pcm_hw_params_t*	handle;
	int				nReturn;
	unsigned int	nValue;
	int				nDirection;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getBufferTimeMin(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_pcm_hw_params_get_buffer_time_min(handle, &nValue, &nDirection);
	setDirection(env, nDirection, anDirection);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getBufferTimeMin(): end\n"); }
	return nValue;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcmHWParams
 * Method:    getBufferTimeMax
 * Signature: ([I)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getBufferTimeMax
(JNIEnv* env, jobject obj, jintArray anDirection)
{
	snd_pcm_hw_params_t*	handle;
	int				nReturn;
	unsigned int	nValue;
	int				nDirection;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getBufferTimeMax(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_pcm_hw_params_get_buffer_time_max(handle, &nValue, &nDirection);
	setDirection(env, nDirection, anDirection);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getBufferTimeMax(): end\n"); }
	return nValue;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcmHWParams
 * Method:    getBufferSize
 * Signature: ([I)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getBufferSize
(JNIEnv* env, jobject obj)
{
	snd_pcm_hw_params_t*	handle;
	snd_pcm_uframes_t		nValue;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getBufferSize(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_pcm_hw_params_get_buffer_size(handle, &nValue);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getBufferSize(): end\n"); }
	return nValue;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcmHWParams
 * Method:    getBufferSizeMin
 * Signature: ([I)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getBufferSizeMin
(JNIEnv* env, jobject obj)
{
	snd_pcm_hw_params_t*	handle;
	snd_pcm_uframes_t		nValue;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getBufferSizeMin(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_pcm_hw_params_get_buffer_size_min(handle, &nValue);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getBufferSizeMin(): end\n"); }
	return nValue;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcmHWParams
 * Method:    getBufferSizeMax
 * Signature: ([I)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getBufferSizeMax
(JNIEnv* env, jobject obj)
{
	snd_pcm_hw_params_t*	handle;
	snd_pcm_uframes_t		nValue;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getBufferSizeMax(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_pcm_hw_params_get_buffer_size_max(handle, &nValue);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getBufferSizeMax(): end\n"); }
	return nValue;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcmHWParams
 * Method:    getTickTime
 * Signature: ([I)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getTickTime
(JNIEnv* env, jobject obj, jintArray anDirection)
{
	snd_pcm_hw_params_t*	handle;
	int				nReturn;
	unsigned int	nValue;
	int				nDirection;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getTickTime(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_pcm_hw_params_get_tick_time(handle, &nValue, &nDirection);
	setDirection(env, nDirection, anDirection);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getTickTime(): end\n"); }
	return nValue;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcmHWParams
 * Method:    getTickTimeMin
 * Signature: ([I)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getTickTimeMin
(JNIEnv* env, jobject obj, jintArray anDirection)
{
	snd_pcm_hw_params_t*	handle;
	int				nReturn;
	unsigned int	nValue;
	int				nDirection;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getTickTimeMin(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_pcm_hw_params_get_tick_time_min(handle, &nValue, &nDirection);
	setDirection(env, nDirection, anDirection);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getTickTimeMin(): end\n"); }
	return nValue;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaPcmHWParams
 * Method:    getTickTimeMax
 * Signature: ([I)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getTickTimeMax
(JNIEnv* env, jobject obj, jintArray anDirection)
{
	snd_pcm_hw_params_t*	handle;
	int				nReturn;
	unsigned int	nValue;
	int				nDirection;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getTickTimeMax(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_pcm_hw_params_get_tick_time_max(handle, &nValue, &nDirection);
	setDirection(env, nDirection, anDirection);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaPcmHWParams_getTickTimeMax(): end\n"); }
	return nValue;
}



/*** org_tritonus_lowlevel_alsa_AlsaPcmHWParams.c ***/
