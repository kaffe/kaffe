/*
 *	org_tritonus_lowlevel_alsa_AlsaSeqClientInfo.c
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
#include "org_tritonus_lowlevel_alsa_AlsaSeqClientInfo.h"


HandleFieldHandler(snd_seq_client_info_t*)


snd_seq_client_info_t*
getClientInfoNativeHandle(JNIEnv *env, jobject obj)
{
	return getHandle(env, obj);
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqClientInfo
 * Method:    malloc
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqClientInfo_malloc
(JNIEnv* env, jobject obj)
{
	snd_seq_client_info_t*	handle;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqClientInfo_malloc(): begin\n"); }
	nReturn = snd_seq_client_info_malloc(&handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqClientInfo_malloc(): handle: %p\n", handle); }
	setHandle(env, obj, handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqClientInfo_malloc(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqClientInfo
 * Method:    free
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqClientInfo_free
(JNIEnv* env, jobject obj)
{
	snd_seq_client_info_t*	handle;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqClientInfo_free(): begin\n"); }
	handle = getHandle(env, obj);
	snd_seq_client_info_free(handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqClientInfo_free(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqClientInfo
 * Method:    getClient
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqClientInfo_getClient
(JNIEnv* env, jobject obj)
{
	snd_seq_client_info_t*	handle;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqClientInfo_getClient(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_seq_client_info_get_client(handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqClientInfo_getClient(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqClientInfo
 * Method:    getType
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqClientInfo_getType
(JNIEnv* env, jobject obj)
{
	snd_seq_client_info_t*	handle;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqClientInfo_getType(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_seq_client_info_get_type(handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqClientInfo_getType(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqClientInfo
 * Method:    getName
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqClientInfo_getName
(JNIEnv* env, jobject obj)
{
	snd_seq_client_info_t*	handle;
	const char*		pName;
	jstring			strName;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqClientInfo_getName(): begin\n"); }
	handle = getHandle(env, obj);
	pName = snd_seq_client_info_get_name(handle);
	strName = (*env)->NewStringUTF(env, pName);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqClientInfo_getName(): end\n"); }
	return strName;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqClientInfo
 * Method:    getBroadcastFilter
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqClientInfo_getBroadcastFilter
(JNIEnv* env, jobject obj)
{
	snd_seq_client_info_t*	handle;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqClientInfo_getBroadcastFilter(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_seq_client_info_get_broadcast_filter(handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqClientInfo_getBroadcastFilter(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqClientInfo
 * Method:    getErrorBounce
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqClientInfo_getErrorBounce
(JNIEnv* env, jobject obj)
{
	snd_seq_client_info_t*	handle;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqClientInfo_getErrorBounce(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_seq_client_info_get_error_bounce(handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqClientInfo_getErrorBounce(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqClientInfo
 * Method:    getNumPorts
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqClientInfo_getNumPorts
(JNIEnv* env, jobject obj)
{
	snd_seq_client_info_t*	handle;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqClientInfo_getNumPorts(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_seq_client_info_get_num_ports(handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqClientInfo_getNumPorts(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqClientInfo
 * Method:    getEventLost
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqClientInfo_getEventLost
(JNIEnv* env, jobject obj)
{
	snd_seq_client_info_t*	handle;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqClientInfo_getEventLost(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_seq_client_info_get_event_lost(handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqClientInfo_getEventLost(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqClientInfo
 * Method:    setClient
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqClientInfo_setClient
(JNIEnv* env, jobject obj, jint nClient)
{
	snd_seq_client_info_t*	handle;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqClientInfo_setClient(): begin\n"); }
	handle = getHandle(env, obj);
	snd_seq_client_info_set_client(handle, nClient);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqClientInfo_setClient(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqClientInfo
 * Method:    setName
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqClientInfo_setName
(JNIEnv* env, jobject obj, jstring strName)
{
	snd_seq_client_info_t*	handle;
	const char*		pName;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqClientInfo_setName(): begin\n"); }
	handle = getHandle(env, obj);
	pName = (*env)->GetStringUTFChars(env, strName, NULL);
	snd_seq_client_info_set_name(handle, pName);
	(*env)->ReleaseStringUTFChars(env, strName, pName);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqClientInfo_setName(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqClientInfo
 * Method:    setBroadcastFilter
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqClientInfo_setBroadcastFilter
(JNIEnv* env, jobject obj, jint nBroadcastFilter)
{
	snd_seq_client_info_t*	handle;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqClientInfo_setBroadcastFilter(): begin\n"); }
	handle = getHandle(env, obj);
	snd_seq_client_info_set_broadcast_filter(handle, nBroadcastFilter);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqClientInfo_setBroadcastFilter(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqClientInfo
 * Method:    setErrorBounce
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqClientInfo_setErrorBounce
(JNIEnv* env, jobject obj, jint nErrorBounce)
{
	snd_seq_client_info_t*	handle;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqClientInfo_setErrorBounce(): begin\n"); }
	handle = getHandle(env, obj);
	snd_seq_client_info_set_error_bounce(handle, nErrorBounce);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqClientInfo_setErrorBounce(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqClientInfo
 * Method:    setTrace
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqClientInfo_setTrace
(JNIEnv* env UNUSED, jclass cls UNUSED, jboolean bTrace)
{
	debug_flag = bTrace;
	debug_file = stderr;
}



/*** org_tritonus_lowlevel_alsa_AlsaSeqClientInfo.c ***/
