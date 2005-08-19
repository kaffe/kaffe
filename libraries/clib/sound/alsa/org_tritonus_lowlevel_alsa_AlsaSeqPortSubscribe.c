/*
 *	org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe.c
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
#include "org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe.h"


HandleFieldHandler(snd_seq_port_subscribe_t*)


snd_seq_port_subscribe_t*
getPortSubscribeNativeHandle(JNIEnv *env, jobject obj)
{
	return getHandle(env, obj);
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe
 * Method:    malloc
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe_malloc
(JNIEnv* env, jobject obj)
{
	snd_seq_port_subscribe_t*	handle;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe_malloc(): begin\n"); }
	nReturn = snd_seq_port_subscribe_malloc(&handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe_malloc(): handle: %p\n", handle); }
	setHandle(env, obj, handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe_malloc(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe
 * Method:    free
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe_free
(JNIEnv* env, jobject obj)
{
	snd_seq_port_subscribe_t*	handle;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe_free(): begin\n"); }
	handle = getHandle(env, obj);
	snd_seq_port_subscribe_free(handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe_free(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe
 * Method:    getSenderClient
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe_getSenderClient
(JNIEnv* env, jobject obj)
{
	snd_seq_port_subscribe_t*	handle;
	const snd_seq_addr_t*		address;
	int				nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe_getSenderClient(): begin\n"); }
	handle = getHandle(env, obj);
	address = snd_seq_port_subscribe_get_sender(handle);
	nReturn = address->client;
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe_getSenderClient(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe
 * Method:    getSenderPort
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe_getSenderPort
(JNIEnv* env, jobject obj)
{
	snd_seq_port_subscribe_t*	handle;
	const snd_seq_addr_t*		address;
	int				nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe_getSenderPort(): begin\n"); }
	handle = getHandle(env, obj);
	address = snd_seq_port_subscribe_get_sender(handle);
	nReturn = address->port;
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe_getSenderPort(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe
 * Method:    getDestClient
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe_getDestClient
(JNIEnv* env, jobject obj)
{
	snd_seq_port_subscribe_t*	handle;
	const snd_seq_addr_t*		address;
	int				nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe_getDestClient(): begin\n"); }
	handle = getHandle(env, obj);
	address = snd_seq_port_subscribe_get_dest(handle);
	nReturn = address->client;
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe_getDestClient(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe
 * Method:    getDestPort
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe_getDestPort
(JNIEnv* env, jobject obj)
{
	snd_seq_port_subscribe_t*	handle;
	const snd_seq_addr_t*		address;
	int				nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe_getDestPort(): begin\n"); }
	handle = getHandle(env, obj);
	address = snd_seq_port_subscribe_get_dest(handle);
	nReturn = address->port;
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe_getDestPort(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe
 * Method:    getQueue
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe_getQueue
(JNIEnv* env, jobject obj)
{
	snd_seq_port_subscribe_t*	handle;
	int				nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe_getQueue(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_seq_port_subscribe_get_queue(handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe_getQueue(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe
 * Method:    getExclusive
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe_getExclusive
(JNIEnv* env, jobject obj)
{
	snd_seq_port_subscribe_t*	handle;
	int				nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe_getExclusive(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_seq_port_subscribe_get_exclusive(handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe_getExclusive(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe
 * Method:    getTimeUpdate
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe_getTimeUpdate
(JNIEnv* env, jobject obj)
{
	snd_seq_port_subscribe_t*	handle;
	int				nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe_getTimeUpdate(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_seq_port_subscribe_get_time_update(handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe_getTimeUpdate(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe
 * Method:    getTimeReal
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe_getTimeReal
(JNIEnv* env, jobject obj)
{
	snd_seq_port_subscribe_t*	handle;
	int				nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe_getTimeReal(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_seq_port_subscribe_get_time_real(handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe_getTimeReal(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe
 * Method:    setSender
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe_setSender
(JNIEnv* env, jobject obj, jint nClient, jint nPort)
{
	snd_seq_port_subscribe_t*	handle;
	snd_seq_addr_t			newAddress;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe_setSender(): begin\n"); }
	handle = getHandle(env, obj);
	newAddress.client = nClient;
	newAddress.port = nPort;
	snd_seq_port_subscribe_set_sender(handle, &newAddress);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe_setSender(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe
 * Method:    setDest
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe_setDest
(JNIEnv* env, jobject obj, jint nClient, jint nPort)
{
	snd_seq_port_subscribe_t*	handle;
	snd_seq_addr_t			newAddress;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe_setDest(): begin\n"); }
	handle = getHandle(env, obj);
	newAddress.client = nClient;
	newAddress.port = nPort;
	snd_seq_port_subscribe_set_dest(handle, &newAddress);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe_setDest(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe
 * Method:    setQueue
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe_setQueue
(JNIEnv* env, jobject obj, jint nQueue)
{
	snd_seq_port_subscribe_t*	handle;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe_setQueue(): begin\n"); }
	handle = getHandle(env, obj);
	snd_seq_port_subscribe_set_queue(handle, nQueue);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe_setQueue(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe
 * Method:    setExclusive
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe_setExclusive
(JNIEnv* env, jobject obj, jboolean bExclusive)
{
	snd_seq_port_subscribe_t*	handle;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe_setExclusive(): begin\n"); }
	handle = getHandle(env, obj);
	snd_seq_port_subscribe_set_exclusive(handle, bExclusive);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe_setExclusive(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe
 * Method:    setTimeUpdate
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe_setTimeUpdate
(JNIEnv* env, jobject obj, jboolean bUpdate)
{
	snd_seq_port_subscribe_t*	handle;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe_setTimeUpdate(): begin\n"); }
	handle = getHandle(env, obj);
	snd_seq_port_subscribe_set_time_update(handle, bUpdate);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe_setTimeUpdate(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe
 * Method:    setTimeReal
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe_setTimeReal
(JNIEnv* env, jobject obj, jboolean bReal)
{
	snd_seq_port_subscribe_t*	handle;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe_setTimeReal(): begin\n"); }
	handle = getHandle(env, obj);
	snd_seq_port_subscribe_set_time_real(handle, bReal);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe_setTimeReal(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe
 * Method:    setTrace
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe_setTrace
(JNIEnv* env UNUSED, jclass cls UNUSED, jboolean bTrace)
{
	debug_flag = bTrace;
	debug_file = stderr;
}



/*** org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe.c ***/
