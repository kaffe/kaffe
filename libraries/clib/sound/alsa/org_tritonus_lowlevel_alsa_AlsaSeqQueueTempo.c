/*
 *	org_tritonus_lowlevel_alsa_AlsaSeqQueueTempo.c
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
#include "org_tritonus_lowlevel_alsa_AlsaSeqQueueTempo.h"


HandleFieldHandler(snd_seq_queue_tempo_t*)


snd_seq_queue_tempo_t*
getQueueTempoNativeHandle(JNIEnv *env, jobject obj)
{
	return getHandle(env, obj);
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqQueueTempo
 * Method:    malloc
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqQueueTempo_malloc
(JNIEnv* env, jobject obj)
{
	snd_seq_queue_tempo_t*	handle;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqQueueTempo_malloc(): begin\n"); }
	nReturn = snd_seq_queue_tempo_malloc(&handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqQueueTempo_malloc(): handle: %p\n", handle); }
	setHandle(env, obj, handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqQueueTempo_malloc(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqQueueTempo
 * Method:    free
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqQueueTempo_free
(JNIEnv* env, jobject obj)
{
	snd_seq_queue_tempo_t*	handle;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqQueueTempo_free(): begin\n"); }
	handle = getHandle(env, obj);
	snd_seq_queue_tempo_free(handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqQueueTempo_free(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqQueueTempo
 * Method:    getQueue
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqQueueTempo_getQueue
(JNIEnv* env, jobject obj)
{
	snd_seq_queue_tempo_t*	handle;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqQueueTempo_getQueue(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_seq_queue_tempo_get_queue(handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqQueueTempo_getQueue(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqQueueTempo
 * Method:    getTempo
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqQueueTempo_getTempo
(JNIEnv* env, jobject obj)
{
	snd_seq_queue_tempo_t*	handle;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqQueueTempo_getTempo(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_seq_queue_tempo_get_tempo(handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqQueueTempo_getTempo(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqQueueTempo
 * Method:    getPpq
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqQueueTempo_getPpq
(JNIEnv* env, jobject obj)
{
	snd_seq_queue_tempo_t*	handle;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqQueueTempo_getPpq(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_seq_queue_tempo_get_ppq(handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqQueueTempo_getPpq(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqQueueTempo
 * Method:    setTempo
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqQueueTempo_setTempo
(JNIEnv* env, jobject obj, jint nTempo)
{
	snd_seq_queue_tempo_t*	handle;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqQueueTempo_setTempo(): begin\n"); }
	handle = getHandle(env, obj);
	snd_seq_queue_tempo_set_tempo(handle, nTempo);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqQueueTempo_setTempo(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqQueueTempo
 * Method:    setPpq
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqQueueTempo_setPpq
(JNIEnv* env, jobject obj, jint nPpq)
{
	snd_seq_queue_tempo_t*	handle;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqQueueTempo_setPpq(): begin\n"); }
	handle = getHandle(env, obj);
	snd_seq_queue_tempo_set_ppq(handle, nPpq);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqQueueTempo_setPpq(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqQueueTempo
 * Method:    setTrace
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqQueueTempo_setTrace
(JNIEnv* env UNUSED, jclass cls UNUSED, jboolean bTrace)
{
	debug_flag = bTrace;
	debug_file = stderr;
}



/*** org_tritonus_lowlevel_alsa_AlsaSeqQueueTempo.c ***/
