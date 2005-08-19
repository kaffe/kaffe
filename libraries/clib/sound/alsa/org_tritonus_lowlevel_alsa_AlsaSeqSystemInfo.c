/*
 *	org_tritonus_lowlevel_alsa_AlsaSeqSystemInfo.c
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
#include "org_tritonus_lowlevel_alsa_AlsaSeqSystemInfo.h"


HandleFieldHandler(snd_seq_system_info_t*)


snd_seq_system_info_t*
getSystemInfoNativeHandle(JNIEnv *env, jobject obj)
{
	return getHandle(env, obj);
}


/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqSystemInfo
 * Method:    malloc
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqSystemInfo_malloc
(JNIEnv* env, jobject obj)
{
	snd_seq_system_info_t*	handle;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqSystemInfo_malloc(): begin\n"); }
	nReturn = snd_seq_system_info_malloc(&handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqSystemInfo_malloc(): handle: %p\n", handle); }
	setHandle(env, obj, handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqSystemInfo_malloc(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqSystemInfo
 * Method:    free
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqSystemInfo_free
(JNIEnv* env, jobject obj)
{
	snd_seq_system_info_t*	handle;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqSystemInfo_free(): begin\n"); }
	handle = getHandle(env, obj);
	snd_seq_system_info_free(handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqSystemInfo_free(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqSystemInfo
 * Method:    getQueues
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqSystemInfo_getQueues
(JNIEnv* env, jobject obj)
{
	snd_seq_system_info_t*	handle;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqSystemInfo_getQueues(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_seq_system_info_get_queues(handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqSystemInfo_getQueues(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqSystemInfo
 * Method:    getClients
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqSystemInfo_getClients
(JNIEnv* env, jobject obj)
{
	snd_seq_system_info_t*	handle;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqSystemInfo_getClients(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_seq_system_info_get_clients(handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqSystemInfo_getClients(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqSystemInfo
 * Method:    getPorts
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqSystemInfo_getPorts
(JNIEnv* env, jobject obj)
{
	snd_seq_system_info_t*	handle;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqSystemInfo_getPorts(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_seq_system_info_get_ports(handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqSystemInfo_getPorts(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqSystemInfo
 * Method:    getChannels
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqSystemInfo_getChannels
(JNIEnv* env, jobject obj)
{
	snd_seq_system_info_t*	handle;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqSystemInfo_getChannels(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_seq_system_info_get_channels(handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqSystemInfo_getChannels(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqSystemInfo
 * Method:    getCurrentClients
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqSystemInfo_getCurrentClients
(JNIEnv* env, jobject obj)
{
	snd_seq_system_info_t*	handle;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqSystemInfo_getCurrentClients(): begin\n"); }
	handle = getHandle(env, obj);
	// TODO: disabled because not available in ALSA 0.9.0beta6
	nReturn = -1;	// snd_seq_system_info_get_cur_clients(handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqSystemInfo_getCurrentClients(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqSystemInfo
 * Method:    getCurrentQueues
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqSystemInfo_getCurrentQueues
(JNIEnv* env, jobject obj)
{
	snd_seq_system_info_t*	handle;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqSystemInfo_getCurrentQueues(): begin\n"); }
	handle = getHandle(env, obj);
	// TODO: disabled because not available in ALSA 0.9.0beta6
	nReturn = -1;	// snd_seq_system_info_get_cur_queues(handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqSystemInfo_getCurrentQueues(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqSystemInfo
 * Method:    setTrace
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqSystemInfo_setTrace
(JNIEnv* env UNUSED, jclass cls UNUSED, jboolean bTrace)
{
	debug_flag = bTrace;
	debug_file = stderr;
}



/*** org_tritonus_lowlevel_alsa_AlsaSeqSystemInfo.c ***/
