/*
 *	org_tritonus_lowlevel_alsa_AlsaCtl.c
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
#include "org_tritonus_lowlevel_alsa_AlsaCtl.h"


// static HandleFieldHandler<snd_ctl_t*>	handler;
HandleFieldHandlerDeclaration(handler,snd_ctl_t*);

snd_ctl_card_info_t*
getAlsaCtlCardInfoNativeHandle(JNIEnv *env, jobject obj);



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaCtl
 * Method:    loadCard
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaCtl_loadCard
(JNIEnv *env UNUSED, jclass cls UNUSED, jint nCard)
{
	return snd_card_load(nCard);
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaCtl
 * Method:    getCards
 * Signature: ()[I
 */
JNIEXPORT jintArray JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaCtl_getCards
(JNIEnv *env, jclass cls UNUSED)
{
	int		anCards[32];
	int		nCard = -1;
	int		nCardCount = 0;
	int		nError;
	jintArray	cardsArray;

	nError = snd_card_next(&nCard);
	while (nCard >= 0 && nError >= 0)
	{
		anCards[nCardCount] = nCard;
		nCardCount++;
		nError = snd_card_next(&nCard);
	}
	cardsArray = (*env)->NewIntArray(env, nCardCount);
	if (cardsArray == NULL)
	{
		throwRuntimeException(env, "cannot allocate int array");
	}
	(*env)->SetIntArrayRegion(env, cardsArray, 0, nCardCount, (jint*) anCards);
	return cardsArray;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaCtl
 * Method:    getCardIndex
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaCtl_getCardIndex
(JNIEnv *env, jclass cls UNUSED, jstring strCardName)
{
	int	nCard;
	const char*	name = NULL;
	name = (*env)->GetStringUTFChars(env, strCardName, NULL);
	if (name == NULL)
	{
		throwRuntimeException(env, "cannot get characters from string argument");
	}
	nCard = snd_card_get_index(name);
	(*env)->ReleaseStringUTFChars(env, strCardName, name);
	return nCard;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaCtl
 * Method:    getCardName
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaCtl_getCardName
(JNIEnv *env, jclass cls UNUSED, jint nCard)
{
	int	nReturn;
	jstring	strName;
	char*	name;
	nReturn = snd_card_get_name(nCard, &name);
	if (nReturn < 0)
	{
		throwRuntimeException(env, "snd_card_get_name() failed");
	}
	strName = (*env)->NewStringUTF(env, name);
	if (strName == NULL)
	{
		throwRuntimeException(env, "NewStringUTF() failed");
	}
	return strName;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaCtl
 * Method:    getCardLongName
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaCtl_getCardLongName
(JNIEnv *env, jclass cls UNUSED, jint nCard)
{
	int	nReturn;
	jstring	strName;
	char*	name;
	nReturn = snd_card_get_longname(nCard, &name);
	if (nReturn < 0)
	{
		throwRuntimeException(env, "snd_card_get_longname() failed");
	}
	strName = (*env)->NewStringUTF(env, name);
	if (strName == NULL)
	{
		throwRuntimeException(env, "NewStringUTF() failed");
	}
	return strName;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaCtl
 * Method:    open
 * Signature: (Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaCtl_open
(JNIEnv *env, jobject obj, jstring strName, jint nMode)
{
	snd_ctl_t*	handle;
	int		nResult;
	const char*	name;

	// printf("1");
	name = (*env)->GetStringUTFChars(env, strName, NULL);
	// printf("2");
	if (name == NULL)
	{
		// printf("3");
		throwRuntimeException(env, "cannot get characters from string argument");
	}
	// printf("4");
	nResult = snd_ctl_open(&handle, name, nMode);
	// printf("5");
	(*env)->ReleaseStringUTFChars(env, strName, name);
	// printf("6");
	if (nResult >= 0)
	{
		// printf("7");
		setHandle(env, obj, handle);
	}
	return nResult;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaCtl
 * Method:    close
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaCtl_close
(JNIEnv *env, jobject obj)
{
	snd_ctl_t*	handle;
	int		nResult;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaCtl_close(): begin\n"); }
	handle = getHandle(env, obj);
	nResult = snd_ctl_close(handle);
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaCtl_close(): end\n"); }
	return nResult;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaCtl
 * Method:    getCardInfo
 * Signature: (Lorg/tritonus/lowlevel/alsa/AlsaCtlCardInfo;)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaCtl_getCardInfo
(JNIEnv* env, jobject obj, jobject cardInfoObj)
{
	snd_ctl_t*		handle;
	snd_ctl_card_info_t*	cardInfo;
	int			nReturn;

	handle = getHandle(env, obj);
	cardInfo = getAlsaCtlCardInfoNativeHandle(env, cardInfoObj);
	nReturn = snd_ctl_card_info(handle, cardInfo);
	return (jint) nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaCtl
 * Method:    getPcmDevices
 * Signature: ()[I
 */
JNIEXPORT jintArray JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaCtl_getPcmDevices
(JNIEnv *env, jobject obj)
{
	snd_ctl_t*	handle;
	int		anDevices[128];
	int		nDevice = -1;
	int		nDeviceCount = 0;
	int		nError;
	jintArray	devicesArray;

	handle = getHandle(env, obj);
	nError = snd_ctl_pcm_next_device(handle, &nDevice);
	while (nDevice >= 0 && nError >= 0)
	{
		anDevices[nDeviceCount] = nDevice;
		nDeviceCount++;
		nError = snd_ctl_pcm_next_device(handle, &nDevice);
	}
	devicesArray = (*env)->NewIntArray(env, nDeviceCount);
	if (devicesArray == NULL)
	{
		throwRuntimeException(env, "cannot allocate int array");
	}
	(*env)->SetIntArrayRegion(env, devicesArray, 0, nDeviceCount, (jint*) anDevices);
	return devicesArray;
}


/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaCtl
 * Method:    getPcmInfo
 * Signature: ([I[Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaCtl_getPcmInfo
(JNIEnv *env UNUSED, jobject obj UNUSED, jintArray anValues UNUSED, jobjectArray astrValues UNUSED)
{
	// TODO:
	return -1;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaCtl
 * Method:    setTrace
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaCtl_setTrace
(JNIEnv* env UNUSED, jclass cls UNUSED, jboolean bTrace)
{
	debug_flag = bTrace;
	debug_file = stderr;
}

/*** org_tritonus_lowlevel_alsa_AlsaCtl.c ***/
