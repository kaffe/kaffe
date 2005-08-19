/*
 *	org_tritonus_lowlevel_alsa_AlsaSeqEvent.c
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
#include "org_tritonus_lowlevel_alsa_AlsaSeqEvent.h"


HandleFieldHandler(snd_seq_event_t*)





snd_seq_event_t*
getEventNativeHandle(JNIEnv* env, jobject obj)
{
	return getHandle(env, obj);
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqEvent
 * Method:    malloc
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_malloc
(JNIEnv* env, jobject obj)
{
	snd_seq_event_t*	handle;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_malloc(): begin\n"); }
	handle = (snd_seq_event_t*) calloc(1, sizeof(snd_seq_event_t));
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_malloc(): handle: %p\n", handle); }
	setHandle(env, obj, handle);
	if (handle != NULL)
	{
		nReturn = 0;
	}
	else
	{
		nReturn = -1;
	}
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_malloc(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqEvent
 * Method:    free
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_free
(JNIEnv* env, jobject obj)
{
	snd_seq_event_t*	handle;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_free(): begin\n"); }
	handle = getHandle(env, obj);
	if (snd_seq_ev_is_variable(handle))
	{
		free(handle->data.ext.ptr);
	}
	free(handle);
	setHandle(env, obj, NULL);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_free(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqEvent
 * Method:    getType
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_getType
(JNIEnv* env, jobject obj)
{
	snd_seq_event_t*	handle;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_getType(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = handle->type;
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_getType(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqEvent
 * Method:    getFlags
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_getFlags
(JNIEnv* env, jobject obj)
{
	snd_seq_event_t*	handle;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_getFlags(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = handle->flags;
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_getFlags(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqEvent
 * Method:    getTag
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_getTag
(JNIEnv* env, jobject obj)
{
	snd_seq_event_t*	handle;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_getTag(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = handle->tag;
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_getTag(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqEvent
 * Method:    getQueue
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_getQueue
(JNIEnv* env, jobject obj)
{
	snd_seq_event_t*	handle;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_getQueue(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = handle->type;
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_getQueue(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqEvent
 * Method:    getTimestamp
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_getTimestamp
(JNIEnv* env, jobject obj)
{
	snd_seq_event_t*	handle;
	jlong			lTimestamp;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_getTimestamp(): begin\n"); }
	handle = getHandle(env, obj);
	if ((handle->flags & SND_SEQ_TIME_STAMP_MASK) == SND_SEQ_TIME_STAMP_TICK)
	{
		lTimestamp = handle->time.tick;
	}
	else	// time
	{
		lTimestamp = (jlong) handle->time.time.tv_sec * (jlong) 1000000000 + (jlong) handle->time.time.tv_nsec;
	}
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_getTimestamp(): end\n"); }
	return lTimestamp;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqEvent
 * Method:    getSourceClient
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_getSourceClient
(JNIEnv* env, jobject obj)
{
	snd_seq_event_t*	handle;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_getSourceClient(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = handle->source.client;
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_getSourceClient(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqEvent
 * Method:    getSourcePort
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_getSourcePort
(JNIEnv* env, jobject obj)
{
	snd_seq_event_t*	handle;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_getSourcePort(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = handle->source.port;
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_getSourcePort(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqEvent
 * Method:    getDestClient
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_getDestClient
(JNIEnv* env, jobject obj)
{
	snd_seq_event_t*	handle;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_getDestClient(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = handle->dest.client;
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_getDestClient(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqEvent
 * Method:    getDestPort
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_getDestPort
(JNIEnv* env, jobject obj)
{
	snd_seq_event_t*	handle;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_getDestPort(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = handle->dest.port;
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_getDestPort(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqEvent
 * Method:    getNote
 * Signature: ([I)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_getNote
(JNIEnv* env, jobject obj, jintArray anValues)
{
	snd_seq_event_t*	handle;
	jint*			panValues;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_getNote(): begin\n"); }
	handle = getHandle(env, obj);
	checkArrayLength(env, anValues, 5);
	panValues = (*env)->GetIntArrayElements(env, anValues, NULL);
	if (panValues == NULL)
	{
		throwRuntimeException(env, "GetIntArrayElements() failed");
	}
	panValues[0] = handle->data.note.channel;
	panValues[1] = handle->data.note.note;
	panValues[2] = handle->data.note.velocity;
	panValues[3] = handle->data.note.off_velocity;
	panValues[4] = handle->data.note.duration;
	(*env)->ReleaseIntArrayElements(env, anValues, panValues, 0);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_getNote(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqEvent
 * Method:    getControl
 * Signature: ([I)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_getControl
(JNIEnv* env, jobject obj, jintArray anValues)
{
	snd_seq_event_t*	handle;
	jint*			panValues;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_getControl(): begin\n"); }
	handle = getHandle(env, obj);
	checkArrayLength(env, anValues, 3);
	panValues = (*env)->GetIntArrayElements(env, anValues, NULL);
	if (panValues == NULL)
	{
		throwRuntimeException(env, "GetIntArrayElements() failed");
	}
	panValues[0] = handle->data.control.channel;
	panValues[1] = handle->data.control.param;
	panValues[2] = handle->data.control.value;
	(*env)->ReleaseIntArrayElements(env, anValues, panValues, 0);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_getControl(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqEvent
 * Method:    getQueueControl
 * Signature: ([I[J)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_getQueueControl
(JNIEnv* env, jobject obj, jintArray anValues, jlongArray alValues)
{
	snd_seq_event_t*	handle;
	jint*			panValues;
	jlong*			palValues;
	int			nType;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_getQueueControl(): begin\n"); }
	handle = getHandle(env, obj);
	checkArrayLength(env, anValues, 2);
	checkArrayLength(env, alValues, 1);
	panValues = (*env)->GetIntArrayElements(env, anValues, NULL);
	if (panValues == NULL)
	{
		throwRuntimeException(env, "GetIntArrayElements() failed");
	}
	palValues = (*env)->GetLongArrayElements(env, alValues, NULL);
	if (palValues == NULL)
	{
		throwRuntimeException(env, "GetLongArrayElements() failed");
	}
	panValues[0] = handle->data.queue.queue;
	nType = handle->type;

	if (nType == SND_SEQ_EVENT_TEMPO)
	{
		panValues[1] = handle->data.queue.param.value;
	}
	else if (nType == SND_SEQ_EVENT_SETPOS_TICK)
	{
		palValues[0] = handle->data.queue.param.time.tick;
	}
	else if (nType == SND_SEQ_EVENT_SETPOS_TIME)
	{
		palValues[0] = (jlong) handle->data.queue.param.time.time.tv_sec * (jlong) 1000000000 + (jlong) handle->data.queue.param.time.time.tv_nsec;
	}
	(*env)->ReleaseIntArrayElements(env, anValues, panValues, 0);
	(*env)->ReleaseLongArrayElements(env, alValues, palValues, 0);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_getQueueControl(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqEvent
 * Method:    getVar
 * Signature: ()[B
 */
JNIEXPORT jbyteArray JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_getVar
(JNIEnv* env, jobject obj)
{
	snd_seq_event_t*	handle;
	jbyteArray		abData;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_getVar(): begin\n"); }
	handle = getHandle(env, obj);
	abData = (*env)->NewByteArray(env, handle->data.ext.len);
	if (abData == NULL)
	{
		throwRuntimeException(env, "NewByteArray() failed");
	}
	(*env)->SetByteArrayRegion(env, abData, (jsize) 0, (jsize) handle->data.ext.len, (jbyte*) handle->data.ext.ptr);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_getVar(): end\n"); }
	return abData;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqEvent
 * Method:    setCommon
 * Signature: (IIIIJIIII)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_setCommon
(JNIEnv* env, jobject obj, jint nType, jint nFlags, jint nTag, jint nQueue, jlong lTimestamp, 
jint nSourceClient UNUSED, jint nSourcePort, jint nDestClient, jint nDestPort)
{
	snd_seq_event_t*	handle;
	int			nLengthFlags;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_setCommon(): begin\n"); }
	handle = getHandle(env, obj);
	handle->type = nType;
	// preserve the length flags
	nLengthFlags = handle->flags & SND_SEQ_EVENT_LENGTH_MASK;
	handle->flags = (nFlags & ~SND_SEQ_EVENT_LENGTH_MASK) | nLengthFlags;
	handle->tag = nTag;
	handle->queue = nQueue;
	if ((handle->flags & SND_SEQ_TIME_STAMP_MASK) == SND_SEQ_TIME_STAMP_TICK)
	{
		handle->time.tick = lTimestamp;
	}
	else
	{
		handle->time.time.tv_sec = lTimestamp / 1000000000;
		handle->time.time.tv_nsec = lTimestamp % 1000000000;
	}

	// source client is set by the sequencer to sending client
	handle->source.port = nSourcePort;
	handle->dest.client = nDestClient;
	handle->dest.port = nDestPort;
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_setCommon(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqEvent
 * Method:    setTimestamp
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_setTimestamp
(JNIEnv* env, jobject obj, jlong lTimestamp)
{
	snd_seq_event_t*	handle;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_setTimestamp(): begin\n"); }
	handle = getHandle(env, obj);
	if ((handle->flags & SND_SEQ_TIME_STAMP_MASK) == SND_SEQ_TIME_STAMP_TICK)
	{
		handle->time.tick = lTimestamp;
	}
	else
	{
		handle->time.time.tv_sec = lTimestamp / 1000000000;
		handle->time.time.tv_nsec = lTimestamp % 1000000000;
	}
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_setTimestamp(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqEvent
 * Method:    setNote
 * Signature: (IIIII)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_setNote
(JNIEnv* env, jobject obj, jint nChannel, jint nNote, jint nVelocity, jint nOffVelocity, jint nDuration)
{
	snd_seq_event_t*	handle;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_setNote(): begin\n"); }
	handle = getHandle(env, obj);
	snd_seq_ev_set_fixed(handle);
	handle->data.note.channel = nChannel;
	handle->data.note.note = nNote;
	handle->data.note.velocity = nVelocity;
	handle->data.note.off_velocity = nOffVelocity;
	handle->data.note.duration = nDuration;
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_setNote(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqEvent
 * Method:    setControl
 * Signature: (III)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_setControl
(JNIEnv* env, jobject obj, jint nChannel, jint nParam, jint nValue)
{
	snd_seq_event_t*	handle;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_setControl(): begin\n"); }
	handle = getHandle(env, obj);
	if (snd_seq_ev_is_variable(handle))
	{
		// printf("var data   free: %p\n", handle->data.ext.ptr);
		free(handle->data.ext.ptr);
	}
	snd_seq_ev_set_fixed(handle);
	handle->data.control.channel = nChannel;
	handle->data.control.param = nParam;
	handle->data.control.value = nValue;
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_setControl(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqEvent
 * Method:    setQueueControl
 * Signature: (IIJ)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_setQueueControl
(JNIEnv* env, jobject obj, jint nQueue, jint nValue, jlong lTime)
{
	snd_seq_event_t*	handle;
	int			nType;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_setQueueControl(): begin\n"); }
	handle = getHandle(env, obj);
	if (snd_seq_ev_is_variable(handle))
	{
		// printf("var data   free: %p\n", handle->data.ext.ptr);
		free(handle->data.ext.ptr);
	}
	snd_seq_ev_set_fixed(handle);
	nType = handle->type;
	handle->data.queue.queue = nQueue;
	if (nType == SND_SEQ_EVENT_TEMPO)
	{
		handle->data.queue.param.value = nValue;
	}
	else if (nType == SND_SEQ_EVENT_SETPOS_TICK)
	{
		handle->data.queue.param.time.tick = lTime;
	}
	else if (nType == SND_SEQ_EVENT_SETPOS_TIME)
	{
		handle->data.queue.param.time.time.tv_sec = lTime / 1000000000;
		handle->data.queue.param.time.time.tv_nsec = lTime % 1000000000;
	}
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_setQueueControl(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqEvent
 * Method:    setVar
 * Signature: ([BII)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_setVar
(JNIEnv* env, jobject obj, jbyteArray abData, jint nOffset, jint nLength)
{
	snd_seq_event_t*	handle;
	jbyte*			pData;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_setVar(): begin\n"); }
	handle = getHandle(env, obj);
	if (snd_seq_ev_is_variable(handle))
	{
		// printf("var data   free: %p\n", handle->data.ext.ptr);
		free(handle->data.ext.ptr);
	}
	pData = (jbyte*) malloc(nLength);
	// printf("var data malloc: %p\n", pData);
	if (pData == NULL)
	{
		if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_setVar(): malloc() failed, throwing exception\n"); }
		throwRuntimeException(env, "malloc() failed");
		return;
	}
	(*env)->GetByteArrayRegion(env, abData, nOffset, nLength, pData);
	snd_seq_ev_set_variable(handle, nLength, pData);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_setVar(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqEvent
 * Method:    setTrace
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqEvent_setTrace
(JNIEnv* env UNUSED, jclass cls UNUSED, jboolean bTrace)
{
	debug_flag = bTrace;
	debug_file = stderr;
}



/*** org_tritonus_lowlevel_alsa_AlsaSeqEvent.c ***/
