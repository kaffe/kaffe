/*
 *	org_tritonus_lowlevel_alsa_AlsaCtlCardInfo.c
 */

/*
 *  Copyright (c) 2002 by Matthias Pfisterer <Matthias.Pfisterer@web.de>
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
#include "org_tritonus_lowlevel_alsa_AlsaCtlCardInfo.h"


HandleFieldHandler(snd_ctl_card_info_t*)



snd_ctl_card_info_t*
getAlsaCtlCardInfoNativeHandle(JNIEnv *env, jobject obj)
{
	return getHandle(env, obj);
}


/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaCtlCardInfo
 * Method:    malloc
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaCtlCardInfo_malloc
(JNIEnv* env, jobject obj)
{
	snd_ctl_card_info_t*	handle;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaCtlCardInfo_malloc(): begin\n"); }
	nReturn = snd_ctl_card_info_malloc(&handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaCtlCardInfo_malloc(): handle: %p\n", handle); }
	setHandle(env, obj, handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaCtlCardInfo_malloc(): end\n"); }
	return nReturn;
}


/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaCtlCardInfo
 * Method:    free
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaCtlCardInfo_free
(JNIEnv* env, jobject obj)
{
	snd_ctl_card_info_t*	handle;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaCtlCardInfo_free(): begin\n"); }
	handle = getHandle(env, obj);
	snd_ctl_card_info_free(handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaCtlCardInfo_free(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaCtlCardInfo
 * Method:    getCard
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaCtlCardInfo_getCard
(JNIEnv* env, jobject obj)
{
	snd_ctl_card_info_t*	handle;
	int			nReturn;

	handle = getHandle(env, obj);
	nReturn = snd_ctl_card_info_get_card(handle);
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaCtlCardInfo
 * Method:    getId
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaCtlCardInfo_getId
(JNIEnv* env, jobject obj)
{
	snd_ctl_card_info_t*	handle;
	jstring			strResult;
	const char*		result;

	handle = getHandle(env, obj);
	result = snd_ctl_card_info_get_id(handle);
	if (result == NULL)
	{
		throwRuntimeException(env, "snd_card_get_id() failed");
	}
	strResult = (*env)->NewStringUTF(env, result);
	if (strResult == NULL)
	{
		throwRuntimeException(env, "NewStringUTF() failed");
	}
	return strResult;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaCtlCardInfo
 * Method:    getDriver
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaCtlCardInfo_getDriver
(JNIEnv* env, jobject obj)
{
	snd_ctl_card_info_t*	handle;
	jstring			strResult;
	const char*		result;

	handle = getHandle(env, obj);
	result = snd_ctl_card_info_get_driver(handle);
	if (result == NULL)
	{
		throwRuntimeException(env, "snd_card_get_driver() failed");
	}
	strResult = (*env)->NewStringUTF(env, result);
	if (strResult == NULL)
	{
		throwRuntimeException(env, "NewStringUTF() failed");
	}
	return strResult;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaCtlCardInfo
 * Method:    getName
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaCtlCardInfo_getName
(JNIEnv* env, jobject obj)
{
	snd_ctl_card_info_t*	handle;
	jstring			strResult;
	const char*		result;

	handle = getHandle(env, obj);
	result = snd_ctl_card_info_get_name(handle);
	if (result == NULL)
	{
		throwRuntimeException(env, "snd_card_get_name() failed");
	}
	strResult = (*env)->NewStringUTF(env, result);
	if (strResult == NULL)
	{
		throwRuntimeException(env, "NewStringUTF() failed");
	}
	return strResult;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaCtlCardInfo
 * Method:    getLongname
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaCtlCardInfo_getLongname
(JNIEnv* env, jobject obj)
{
	snd_ctl_card_info_t*	handle;
	jstring			strResult;
	const char*		result;

	handle = getHandle(env, obj);
	result = snd_ctl_card_info_get_longname(handle);
	if (result == NULL)
	{
		throwRuntimeException(env, "snd_card_get_longname() failed");
	}
	strResult = (*env)->NewStringUTF(env, result);
	if (strResult == NULL)
	{
		throwRuntimeException(env, "NewStringUTF() failed");
	}
	return strResult;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaCtlCardInfo
 * Method:    getMixername
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaCtlCardInfo_getMixername
(JNIEnv* env, jobject obj)
{
	snd_ctl_card_info_t*	handle;
	jstring			strResult;
	const char*		result;

	handle = getHandle(env, obj);
	result = snd_ctl_card_info_get_mixername(handle);
	if (result == NULL)
	{
		throwRuntimeException(env, "snd_card_get_mixername() failed");
	}
	strResult = (*env)->NewStringUTF(env, result);
	if (strResult == NULL)
	{
		throwRuntimeException(env, "NewStringUTF() failed");
	}
	return strResult;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaCtlCardInfo
 * Method:    getComponents
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaCtlCardInfo_getComponents
(JNIEnv* env, jobject obj)
{
	snd_ctl_card_info_t*	handle;
	jstring			strResult;
	const char*		result;

	handle = getHandle(env, obj);
	result = snd_ctl_card_info_get_components(handle);
	if (result == NULL)
	{
		throwRuntimeException(env, "snd_card_get_components() failed");
	}
	strResult = (*env)->NewStringUTF(env, result);
	if (strResult == NULL)
	{
		throwRuntimeException(env, "NewStringUTF() failed");
	}
	return strResult;
}


/*** org_tritonus_lowlevel_alsa_AlsaCtlCardInfo.c ***/
