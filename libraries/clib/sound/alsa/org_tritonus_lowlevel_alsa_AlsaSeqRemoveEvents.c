/*
 *	org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents.c
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
#include "org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents.h"


HandleFieldHandler(snd_seq_remove_events_t*)


snd_seq_remove_events_t*
getRemoveEventsNativeHandle(JNIEnv *env, jobject obj)
{
	return getHandle(env, obj);
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents
 * Method:    malloc
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_malloc
(JNIEnv* env, jobject obj)
{
	snd_seq_remove_events_t*	handle;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_malloc(): begin\n"); }
	nReturn = snd_seq_remove_events_malloc(&handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_malloc(): handle: %p\n", handle); }
	setHandle(env, obj, handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_malloc(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents
 * Method:    free
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_free
(JNIEnv* env, jobject obj)
{
	snd_seq_remove_events_t*	handle;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_free(): begin\n"); }
	handle = getHandle(env, obj);
	snd_seq_remove_events_free(handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_free(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents
 * Method:    getCondition
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_getCondition
(JNIEnv* env, jobject obj)
{
	snd_seq_remove_events_t*	handle;
	int				nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_getCondition(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_seq_remove_events_get_condition(handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_getCondition(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents
 * Method:    getQueue
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_getQueue
(JNIEnv* env, jobject obj)
{
	snd_seq_remove_events_t*	handle;
	int				nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_getQueue(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_seq_remove_events_get_queue(handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_getQueue(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents
 * Method:    getTime
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_getTime
(JNIEnv* env, jobject obj)
{
	snd_seq_remove_events_t*	handle;
	int				nCondition;
	const snd_seq_timestamp_t*	time;
	jlong				lTimestamp;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_getTime(): begin\n"); }
	handle = getHandle(env, obj);
	nCondition = snd_seq_remove_events_get_condition(handle);
	time = snd_seq_remove_events_get_time(handle);
	if ((nCondition & SND_SEQ_REMOVE_TIME_TICK) == SND_SEQ_REMOVE_TIME_TICK)
	{
		lTimestamp = time->tick;
	}
	else	// time
	{
		lTimestamp = (jlong) time->time.tv_sec * (jlong) 1000000000 + (jlong) time->time.tv_nsec;
	}
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_getTime(): end\n"); }
	return lTimestamp;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents
 * Method:    getDestClient
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_getDestClient
(JNIEnv* env, jobject obj)
{
	snd_seq_remove_events_t*	handle;
	const snd_seq_addr_t*		address;
	int				nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_getDestClient(): begin\n"); }
	handle = getHandle(env, obj);
	address = snd_seq_remove_events_get_dest(handle);
	nReturn = address->client;
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_getDestClient(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents
 * Method:    getDestPort
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_getDestPort
(JNIEnv* env, jobject obj)
{
	snd_seq_remove_events_t*	handle;
	const snd_seq_addr_t*		address;
	int				nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_getDestPort(): begin\n"); }
	handle = getHandle(env, obj);
	address = snd_seq_remove_events_get_dest(handle);
	nReturn = address->port;
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_getDestPort(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents
 * Method:    getChannel
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_getChannel
(JNIEnv* env, jobject obj)
{
	snd_seq_remove_events_t*	handle;
	int				nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_getChannel(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_seq_remove_events_get_channel(handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_getChannel(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents
 * Method:    getEventType
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_getEventType
(JNIEnv* env, jobject obj)
{
	snd_seq_remove_events_t*	handle;
	int				nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_getEventType(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_seq_remove_events_get_event_type(handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_getEventType(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents
 * Method:    getTag
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_getTag
(JNIEnv* env, jobject obj)
{
	snd_seq_remove_events_t*	handle;
	int				nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_getTag(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_seq_remove_events_get_tag(handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_getTag(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents
 * Method:    setCondition
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_setCondition
(JNIEnv* env, jobject obj, jint nCondition)
{
	snd_seq_remove_events_t*	handle;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_setCondition(): begin\n"); }
	handle = getHandle(env, obj);
	snd_seq_remove_events_set_condition(handle, nCondition);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_setCondition(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents
 * Method:    setQueue
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_setQueue
(JNIEnv* env, jobject obj, jint nQueue)
{
	snd_seq_remove_events_t*	handle;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_setQueue(): begin\n"); }
	handle = getHandle(env, obj);
	snd_seq_remove_events_set_queue(handle, nQueue);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_setQueue(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents
 * Method:    setTime
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_setTime
(JNIEnv* env, jobject obj, jlong lTime)
{
	snd_seq_remove_events_t*	handle;
	int				nCondition;
	snd_seq_timestamp_t		time;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_setTime(): begin\n"); }
	handle = getHandle(env, obj);
	nCondition = snd_seq_remove_events_get_condition(handle);
	if ((nCondition & SND_SEQ_REMOVE_TIME_TICK) == SND_SEQ_REMOVE_TIME_TICK)
	{
		time.tick = lTime;
	}
	else
	{
		time.time.tv_sec = lTime / 1000000000;
		time.time.tv_nsec = lTime % 1000000000;
	}
	snd_seq_remove_events_set_time(handle, &time);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_setTime(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents
 * Method:    setDest
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_setDest
(JNIEnv* env, jobject obj, jint nClient, jint nPort)
{
	snd_seq_remove_events_t*	handle;
	snd_seq_addr_t			newAddress;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_setDest(): begin\n"); }
	handle = getHandle(env, obj);
	newAddress.client = nClient;
	newAddress.port = nPort;
	snd_seq_remove_events_set_dest(handle, &newAddress);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_setDest(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents
 * Method:    setDestPort
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_setDestPort
(JNIEnv* env, jobject obj, jint nPort)
{
	snd_seq_remove_events_t*	handle;
	const snd_seq_addr_t*		oldAddress;
	snd_seq_addr_t			newAddress;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_setDestPort(): begin\n"); }
	handle = getHandle(env, obj);
	oldAddress = snd_seq_remove_events_get_dest(handle);
	newAddress.client = oldAddress->client;
	newAddress.port = nPort;
	snd_seq_remove_events_set_dest(handle, &newAddress);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_setDestPort(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents
 * Method:    setChannel
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_setChannel
(JNIEnv* env, jobject obj, jint nChannel)
{
	snd_seq_remove_events_t*	handle;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_setChannel(): begin\n"); }
	handle = getHandle(env, obj);
	snd_seq_remove_events_set_channel(handle, nChannel);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_setChannel(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents
 * Method:    setEventType
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_setEventType
(JNIEnv* env, jobject obj, jint nType)
{
	snd_seq_remove_events_t*	handle;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_setEventType(): begin\n"); }
	handle = getHandle(env, obj);
	snd_seq_remove_events_set_event_type(handle, nType);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_setEventType(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents
 * Method:    setTag
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_setTag
(JNIEnv* env, jobject obj, jint nTag)
{
	snd_seq_remove_events_t*	handle;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_setTag(): begin\n"); }
	handle = getHandle(env, obj);
	snd_seq_remove_events_set_tag(handle, nTag);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_setTag(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents
 * Method:    setTrace
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents_setTrace
(JNIEnv* env UNUSED, jclass cls UNUSED, jboolean bTrace)
{
	debug_flag = bTrace;
	debug_file = stderr;
}



/*** org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents.c ***/
