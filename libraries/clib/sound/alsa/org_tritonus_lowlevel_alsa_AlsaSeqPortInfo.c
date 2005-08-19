/*
 *	org_tritonus_lowlevel_alsa_AlsaSeqPortInfo.c
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
#include "org_tritonus_lowlevel_alsa_AlsaSeqPortInfo.h"


HandleFieldHandler(snd_seq_port_info_t*)


snd_seq_port_info_t*
getPortInfoNativeHandle(JNIEnv *env, jobject obj)
{
	return getHandle(env, obj);
}




/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqPortInfo
 * Method:    malloc
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqPortInfo_malloc
(JNIEnv* env, jobject obj)
{
	snd_seq_port_info_t*	handle;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortInfo_malloc(): begin\n"); }
	nReturn = snd_seq_port_info_malloc(&handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortInfo_malloc(): handle: %p\n", handle); }
	setHandle(env, obj, handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortInfo_malloc(): end\n"); }
	return nReturn;
}

/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqPortInfo
 * Method:    free
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqPortInfo_free
(JNIEnv* env, jobject obj)
{
	snd_seq_port_info_t*	handle;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortInfo_free(): begin\n"); }
	handle = getHandle(env, obj);
	snd_seq_port_info_free(handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortInfo_free(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqPortInfo
 * Method:    getClient
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqPortInfo_getClient
(JNIEnv* env, jobject obj)
{
	snd_seq_port_info_t*	handle;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortInfo_getClient(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_seq_port_info_get_client(handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortInfo_getClient(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqPortInfo
 * Method:    getPort
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqPortInfo_getPort
(JNIEnv* env, jobject obj)
{
	snd_seq_port_info_t*	handle;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortInfo_getPort(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_seq_port_info_get_port(handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortInfo_getPort(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqPortInfo
 * Method:    getName
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqPortInfo_getName
(JNIEnv* env, jobject obj)
{
	snd_seq_port_info_t*	handle;
	const char*		name;
	jstring			nameObj;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortInfo_getName(): begin\n"); }
	handle = getHandle(env, obj);
	name = snd_seq_port_info_get_name(handle);
	nameObj = (*env)->NewStringUTF(env, name);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortInfo_getName(): end\n"); }
	return nameObj;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqPortInfo
 * Method:    getCapability
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqPortInfo_getCapability
(JNIEnv* env, jobject obj)
{
	snd_seq_port_info_t*	handle;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortInfo_getCapability(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_seq_port_info_get_capability(handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortInfo_getCapability(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqPortInfo
 * Method:    getType
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqPortInfo_getType
(JNIEnv* env, jobject obj)
{
	snd_seq_port_info_t*	handle;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortInfo_getType(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_seq_port_info_get_type(handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortInfo_getType(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqPortInfo
 * Method:    getMidiChannels
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqPortInfo_getMidiChannels
(JNIEnv* env, jobject obj)
{
	snd_seq_port_info_t*	handle;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortInfo_getMidiChannels(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_seq_port_info_get_midi_channels(handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortInfo_getMidiChannels(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqPortInfo
 * Method:    getMidiVoices
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqPortInfo_getMidiVoices
(JNIEnv* env, jobject obj)
{
	snd_seq_port_info_t*	handle;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortInfo_getMidiVoices(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_seq_port_info_get_midi_voices(handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortInfo_getMidiVoices(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqPortInfo
 * Method:    getSynthVoices
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqPortInfo_getSynthVoices
(JNIEnv* env, jobject obj)
{
	snd_seq_port_info_t*	handle;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortInfo_getSynthVoices(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_seq_port_info_get_synth_voices(handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortInfo_getSynthVoices(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqPortInfo
 * Method:    getReadUse
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqPortInfo_getReadUse
(JNIEnv* env, jobject obj)
{
	snd_seq_port_info_t*	handle;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortInfo_getReadUse(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_seq_port_info_get_read_use(handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortInfo_getReadUse(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqPortInfo
 * Method:    getWriteUse
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqPortInfo_getWriteUse
(JNIEnv* env, jobject obj)
{
	snd_seq_port_info_t*	handle;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortInfo_getWriteUse(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_seq_port_info_get_write_use(handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortInfo_getWriteUse(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqPortInfo
 * Method:    getPortSpecified
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqPortInfo_getPortSpecified
(JNIEnv* env, jobject obj)
{
	snd_seq_port_info_t*	handle;
	int			nReturn;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortInfo_getPortSpecified(): begin\n"); }
	handle = getHandle(env, obj);
	nReturn = snd_seq_port_info_get_port_specified(handle);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeqPortInfo_getPortSpecified(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeqPortInfo
 * Method:    setTrace
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeqPortInfo_setTrace
(JNIEnv* env UNUSED, jclass cls UNUSED, jboolean bTrace)
{
	debug_flag = bTrace;
	debug_file = stderr;
}



/*** org_tritonus_lowlevel_alsa_AlsaSeqPortInfo.c ***/
