/*
 *	org_tritonus_lowlevel_alsa_AlsaSeq.c
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
#include "org_tritonus_lowlevel_alsa_AlsaSeq.h"
#include "constants_check.h"


HandleFieldHandler(snd_seq_t*)



snd_seq_client_info_t*
getClientInfoNativeHandle(JNIEnv* env, jobject obj);
snd_seq_event_t*
getEventNativeHandle(JNIEnv* env, jobject obj);
snd_seq_port_info_t*
getPortInfoNativeHandle(JNIEnv* env, jobject obj);
snd_seq_port_subscribe_t*
getPortSubscribeNativeHandle(JNIEnv* env, jobject obj);
snd_seq_queue_info_t*
getQueueInfoNativeHandle(JNIEnv* env, jobject obj);
snd_seq_queue_status_t*
getQueueStatusNativeHandle(JNIEnv* env, jobject obj);
snd_seq_queue_tempo_t*
getQueueTempoNativeHandle(JNIEnv* env, jobject obj);
snd_seq_queue_timer_t*
getQueueTimerNativeHandle(JNIEnv* env, jobject obj);
snd_seq_remove_events_t*
getRemoveEventsNativeHandle(JNIEnv* env, jobject obj);
snd_seq_system_info_t*
getSystemInfoNativeHandle(JNIEnv *env, jobject obj);




/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeq
 * Method:    open
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeq_open
(JNIEnv* env, jobject obj)
{
	snd_seq_t*	seq;
	int		nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_open(): begin\n"); }
	check_constants();
	nReturn = snd_seq_open(&seq, "hw", SND_SEQ_OPEN_DUPLEX, 0);
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_open(): snd_seq_open() returns: %d\n", nReturn); }
	if (nReturn < 0)
	{
		throwRuntimeException(env, "snd_seq_open() failed");
	}
	setHandle(env, obj, seq);
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_open(): end\n"); }
	return (jint) nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeq
 * Method:    close
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeq_close
(JNIEnv* env, jobject obj)
{
	snd_seq_t*	seq;
	int		nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_close(): begin\n"); }
	seq = getHandle(env, obj);
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_close(): now closing ALSA seq (client %d)\n", snd_seq_client_id(seq)); }
	nReturn = snd_seq_close(seq);
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_close(): closed\n"); }
	if (nReturn < 0)
	{
		throwRuntimeException(env, "snd_seq_close() failed");
	}
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_close(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeq
 * Method:    getName
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeq_getName
(JNIEnv* env, jobject obj)
{
	snd_seq_t*	seq;
	const char*	pName;
	jstring		strName;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_getName(): begin\n"); }
	seq = getHandle(env, obj);
	pName = snd_seq_name(seq);
	if (pName == NULL)
	{
		throwRuntimeException(env, "snd_seq_name() failed");
	}
	strName = (*env)->NewStringUTF(env, pName);
	// TODO: check return value
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_getName(): end\n"); }
	return strName;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeq
 * Method:    getType
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeq_getType
(JNIEnv* env, jobject obj)
{
	snd_seq_t*	seq;
	int		nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_getType(): begin\n"); }
	seq = getHandle(env, obj);
	nReturn = snd_seq_type(seq);
	if (nReturn < 0)
	{
		throwRuntimeException(env, "snd_seq_type() failed");
	}
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_getType(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeq
 * Method:    setNonblock
 * Signature: (Z)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeq_setNonblock
(JNIEnv* env, jobject obj, jboolean bNonblock)
{
	snd_seq_t*	seq;
	int		nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_setNonblock(): begin\n"); }
	seq = getHandle(env, obj);
	nReturn = snd_seq_nonblock(seq, bNonblock);
	if (nReturn < 0)
	{
		throwRuntimeException(env, "snd_seq_nonblock() failed");
	}
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_setNonblock(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeq
 * Method:    getClientId
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeq_getClientId
(JNIEnv* env, jobject obj)
{
	snd_seq_t*	seq;
	int		nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_getClientId(): begin\n"); }
	seq = getHandle(env, obj);
	nReturn = snd_seq_client_id(seq);
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_getClientId(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeq
 * Method:    getOutputBufferSize
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeq_getOutputBufferSize
(JNIEnv* env, jobject obj)
{
	snd_seq_t*	seq;
	int		nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_getOutputBufferSize(): begin\n"); }
	seq = getHandle(env, obj);
	nReturn = snd_seq_get_output_buffer_size(seq);
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_getOutputBufferSize(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeq
 * Method:    getInputBufferSize
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeq_getInputBufferSize
(JNIEnv* env, jobject obj)
{
	snd_seq_t*	seq;
	int		nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_getInputBufferSize(): begin\n"); }
	seq = getHandle(env, obj);
	nReturn = snd_seq_get_input_buffer_size(seq);
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_getInputBufferSize(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeq
 * Method:    setOutputBufferSize
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeq_setOutputBufferSize
(JNIEnv* env, jobject obj, jint nBufferSize)
{
	snd_seq_t*	seq;
	int		nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_setOutputBufferSize(): begin\n"); }
	seq = getHandle(env, obj);
	nReturn = snd_seq_set_output_buffer_size(seq, nBufferSize);
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_setOutputBufferSize(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeq
 * Method:    setInputBufferSize
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeq_setInputBufferSize
(JNIEnv* env, jobject obj, jint nBufferSize)
{
	snd_seq_t*	seq;
	int		nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_setInputBufferSize(): begin\n"); }
	seq = getHandle(env, obj);
	nReturn = snd_seq_set_input_buffer_size(seq, nBufferSize);
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_setInputBufferSize(): end\n"); }
	return nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeq
 * Method:    getSystemInfo
 * Signature: (Lorg/tritonus/lowlevel/alsa/AlsaSeq$SystemInfo;)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeq_getSystemInfo
(JNIEnv* env, jobject obj, jobject systemInfoObj)
{
	snd_seq_t*		seq;
	snd_seq_system_info_t*	systemInfo;
	int			nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_getSystemInfo(): begin\n"); }
	seq = getHandle(env, obj);
	systemInfo = getSystemInfoNativeHandle(env, systemInfoObj);
	nReturn = snd_seq_system_info(seq, systemInfo);
	if (nReturn < 0)
	{
		throwRuntimeException(env, "snd_seq_system_info() failed");
	}
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_getSystemInfo(): end\n"); }
	return (jint) nReturn;
}


/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeq
 * Method:    getClientInfo
 * Signature: (ILorg/tritonus/lowlevel/alsa/AlsaSeq$ClientInfo;)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeq_getClientInfo
(JNIEnv* env, jobject obj, jint nClient, jobject clientInfoObj)
{
	snd_seq_t*		seq;
	snd_seq_client_info_t*	clientInfo;
	int			nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_getClientInfo(): begin\n"); }
	seq = getHandle(env, obj);
	clientInfo = getClientInfoNativeHandle(env, clientInfoObj);
	if (nClient >= 0)
	{
		nReturn = snd_seq_get_any_client_info(seq, nClient, clientInfo);
	}
	else
	{
		nReturn = snd_seq_get_client_info(seq, clientInfo);
	}
	if (nReturn < 0)
	{
		throwRuntimeException(env, "snd_seq_get_client_info() failed");
	}
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_getClientInfo(): end\n"); }
	return (jint) nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeq
 * Method:    setClientInfo
 * Signature: (Lorg/tritonus/lowlevel/alsa/AlsaSeq$ClientInfo;)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeq_setClientInfo
(JNIEnv* env, jobject obj, jobject clientInfoObj)
{
	snd_seq_t*		seq;
	snd_seq_client_info_t*	clientInfo;
	int			nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_setClientInfo(): begin\n"); }
	seq = getHandle(env, obj);
	clientInfo = getClientInfoNativeHandle(env, clientInfoObj);
	nReturn = snd_seq_set_client_info(seq, clientInfo);
	if (nReturn < 0)
	{
		throwRuntimeException(env, "snd_seq_set_client_info() failed");
	}
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_setClientInfo(): end\n"); }
	return (jint) nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeq
 * Method:    getNextClient
 * Signature: (I[I)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeq_getNextClient
(JNIEnv* env, jobject obj, jint nClient, jintArray anValues)
{
	snd_seq_t*		seq;
	snd_seq_client_info_t*	clientInfo;
	int			nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_getNextClient(): begin\n"); }
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_getNextClient(): passed client: %d\n", (int) nClient); }
	seq = getHandle(env, obj);
	snd_seq_client_info_alloca(&clientInfo);
	snd_seq_client_info_set_client(clientInfo, nClient);
	nReturn = snd_seq_query_next_client(seq, clientInfo);
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_getNextClient(): return value from snd_seq_query_next_client(): %d\n", nReturn); }
	if (nReturn < 0)
	{
		// -2 (no such file or directory): returned when no more client is available
		if (nReturn != -2)
		{
			throwRuntimeException(env, "snd_seq_query_next_client() failed");
		}
	}
	else
	{
		jint	nThisClient;
		checkArrayLength(env, anValues, 1);
		nThisClient = snd_seq_client_info_get_client(clientInfo);
		(*env)->SetIntArrayRegion(env, anValues, 0, 1, &nThisClient);
	}
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_getNextClient(): end\n"); }
	return (jint) nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeq
 * Method:    getPortInfo
 * Signature: (IILorg/tritonus/lowlevel/alsa/AlsaSeq$PortInfo;)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeq_getPortInfo
(JNIEnv* env, jobject obj, jint nClient, jint nPort, jobject portInfoObj)
{
	snd_seq_t*		seq;
	snd_seq_port_info_t*	portInfo;
	int			nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_getPortInfo(): begin\n"); }
	seq = getHandle(env, obj);
	portInfo = getPortInfoNativeHandle(env, portInfoObj);
	if (nClient >= 0)
	{
		nReturn = snd_seq_get_any_port_info(seq, nClient, nPort, portInfo);
	}
	else
	{
		nReturn = snd_seq_get_port_info(seq, nPort, portInfo);
	}
	if (nReturn < 0)
	{
		throwRuntimeException(env, "snd_seq_get_[any]_port_info() failed");
	}
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_getPortInfo(): end\n"); }
	return (jint) nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeq
 * Method:    getNextPort
 * Signature: (II[I)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeq_getNextPort
(JNIEnv* env, jobject obj, jint nClient, jint nPort, jintArray anValues)
{
	snd_seq_t*		seq;
	snd_seq_port_info_t*	portInfo;
	int			nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_getNextPort(): begin\n"); }
	seq = getHandle(env, obj);
	snd_seq_port_info_alloca(&portInfo);
	snd_seq_port_info_set_client(portInfo, nClient);
	snd_seq_port_info_set_port(portInfo, nPort);
	nReturn = snd_seq_query_next_port(seq, portInfo);
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_getNextPort(): snd_seq_query_next_port() returns: %d\n", nReturn); }
	if (nReturn < 0)
	{
		// -2 (no such file or directory): returned when no more port is available
		if (nReturn != -2)
		{
			throwRuntimeException(env, "snd_seq_query_next_port() failed");
		}
	}
	else
	{
		jint	pnValues[2];

		checkArrayLength(env, anValues, 2);
		pnValues[0] = snd_seq_port_info_get_client(portInfo);
		pnValues[1] = snd_seq_port_info_get_port(portInfo);
		(*env)->SetIntArrayRegion(env, anValues, 0, 2, pnValues);
	}
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_getNextPort(): end\n"); }
	return (jint) nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeq
 * Method:    createPort
 * Signature: (Ljava/lang/String;IIIIII)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeq_createPort
(JNIEnv* env, jobject obj, jstring strName, jint nCapabilities, jint nGroupPermissions, jint nType, jint nMidiChannels, jint nMidiVoices, jint nSynthVoices)
{
	snd_seq_t*		seq;
	snd_seq_port_info_t*	portInfo;
	const char*		name;
	int			nReturn;
	int			nPort;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_createPort(): begin\n");	}
	snd_seq_port_info_alloca(&portInfo);
	seq = getHandle(env, obj);
	// TODO: check if another action is required instead
	// memset(&portInfo, 0, sizeof(portInfo));
	name = (*env)->GetStringUTFChars(env, strName, NULL);
	if (name == NULL)
	{
		throwRuntimeException(env, "GetStringUTFChars() failed");
	}
	snd_seq_port_info_set_name(portInfo, name);
	(*env)->ReleaseStringUTFChars(env, strName, name);
	snd_seq_port_info_set_capability(portInfo, nCapabilities);
	snd_seq_port_info_set_port(portInfo, nGroupPermissions);
	snd_seq_port_info_set_type(portInfo, nType);
	snd_seq_port_info_set_midi_channels(portInfo, nMidiChannels);
	snd_seq_port_info_set_midi_voices(portInfo, nMidiVoices);
	snd_seq_port_info_set_synth_voices(portInfo, nSynthVoices);
	//portInfo.write_use = 1;	// R/O attrs?
	//portInfo.read_use = 1;

	// errno = 0;
	nReturn = snd_seq_create_port(seq, portInfo);
	if (nReturn < 0)
	{
		throwRuntimeException(env, "snd_seq_create_port() failed");
	}
	nPort = snd_seq_port_info_get_port(portInfo);
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_createPort(): end\n"); }
	return (jint) nPort;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeq
 * Method:    allocQueue
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeq_allocQueue
(JNIEnv* env, jobject obj)
{
	snd_seq_t*	seq;
	int		nQueue;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_allocQueue(): begin\n"); }
	seq = getHandle(env, obj);
	nQueue = snd_seq_alloc_queue(seq);
	if (nQueue < 0)
	{
		throwRuntimeException(env, "snd_seq_alloc_queue() failed");
	}
// 	snd_seq_queue_timer_t*	timer;
// 	const snd_timer_id_t*	id;
// 	snd_seq_queue_timer_alloca(&timer);
// 	snd_seq_get_queue_timer(seq, nQueue, timer);
// 	id = snd_seq_queue_timer_get_id(timer);
// 	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_allocQueue(): timer resolution: %d\n", snd_seq_queue_timer_get_resolution(timer)); }
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_allocQueue(): end\n"); }
	return (jint) nQueue;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeq
 * Method:    freeQueue
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeq_freeQueue
(JNIEnv* env, jobject obj, jint nQueue)
{
	snd_seq_t*	seq;
	int		nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_freeQueue(): begin\n"); }
	seq = getHandle(env, obj);
	nReturn = snd_seq_free_queue(seq, nQueue);
	if (nReturn < 0)
	{
		throwRuntimeException(env, "snd_seq_free_queue() failed");
	}
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_freeQueue(): end\n"); }
	return (jint) nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeq
 * Method:    getQueueUsage
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeq_getQueueUsage
(JNIEnv* env, jobject obj, jint nQueue)
{
	snd_seq_t*	seq;
	int		nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_getQueueUsage(): begin\n"); }
	seq = getHandle(env, obj);
	nReturn = snd_seq_get_queue_usage(seq, nQueue);
	if (nReturn < 0)
	{
		throwRuntimeException(env, "snd_seq_get_queue_usage() failed");
	}
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_getQueueUsage(): end\n"); }
	return (jboolean) nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeq
 * Method:    setQueueUsage
 * Signature: (IZ)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeq_setQueueUsage
(JNIEnv* env, jobject obj, jint nQueue, jboolean bUsageAllowed)
{
	snd_seq_t*	seq;
	int		nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_setQueueUsage(): begin\n"); }
	seq = getHandle(env, obj);
	nReturn = snd_seq_set_queue_usage(seq, nQueue, bUsageAllowed);
	if (nReturn < 0)
	{
		throwRuntimeException(env, "snd_seq_set_queue_usage() failed");
	}
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_setQueueUsage(): end\n"); }
	return (jint) nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeq
 * Method:    getQueueInfo
 * Signature: (ILorg/tritonus/lowlevel/alsa/AlsaSeq$QueueInfo;)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeq_getQueueInfo
(JNIEnv* env, jobject obj, jint nQueue, jobject queueInfoObj)
{
	snd_seq_t*		seq;
	snd_seq_queue_info_t*	queueInfo;
	int			nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_getQueueInfo(): begin\n"); }
	seq = getHandle(env, obj);
	queueInfo = getQueueInfoNativeHandle(env, queueInfoObj);
	nReturn = snd_seq_get_queue_info(seq, nQueue, queueInfo);
	if (nReturn < 0)
	{
		throwRuntimeException(env, "snd_seq_get_queue_info() failed");
	}
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_getQueueInfo(): end\n"); }
	return (jint) nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeq
 * Method:    setQueueInfo
 * Signature: (ILorg/tritonus/lowlevel/alsa/AlsaSeq$QueueInfo;)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeq_setQueueInfo
(JNIEnv* env, jobject obj, jint nQueue, jobject queueInfoObj)
{
	snd_seq_t*		seq;
	snd_seq_queue_info_t*	queueInfo;
	int			nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_setQueueInfo(): begin\n"); }
	seq = getHandle(env, obj);
	queueInfo = getQueueInfoNativeHandle(env, queueInfoObj);
	nReturn = snd_seq_set_queue_info(seq, nQueue, queueInfo);
	if (nReturn < 0)
	{
		throwRuntimeException(env, "snd_seq_set_queue_info() failed");
	}
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_setQueueInfo(): end\n"); }
	return (jint) nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeq
 * Method:    getQueueStatus
 * Signature: (ILorg/tritonus/lowlevel/alsa/AlsaSeq$QueueStatus;)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeq_getQueueStatus
(JNIEnv* env, jobject obj, jint nQueue, jobject queueStatusObj)
{
	snd_seq_t*		seq;
	snd_seq_queue_status_t*	queueStatus;
	int			nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_getQueueStatus(): begin\n"); }
	seq = getHandle(env, obj);
	queueStatus = getQueueStatusNativeHandle(env, queueStatusObj);
	nReturn = snd_seq_get_queue_status(seq, nQueue, queueStatus);
	if (nReturn < 0)
	{
		throwRuntimeException(env, "snd_seq_get_queue_status() failed");
	}
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_getQueueStatus(): end\n"); }
	return (jint) nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeq
 * Method:    getQueueTempo
 * Signature: (ILorg/tritonus/lowlevel/alsa/AlsaSeq$QueueTempo;)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeq_getQueueTempo
(JNIEnv* env, jobject obj, jint nQueue, jobject queueTempoObj)
{
	snd_seq_t*		seq;
	snd_seq_queue_tempo_t*	pQueueTempo;
	int			nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_getQueueTempo(): begin\n"); }
	seq = getHandle(env, obj);
	pQueueTempo = getQueueTempoNativeHandle(env, queueTempoObj);
	nReturn = snd_seq_get_queue_tempo(seq, nQueue, pQueueTempo);
	if (nReturn < 0)
	{
		throwRuntimeException(env, "snd_seq_get_queue_tempo() failed");
	}
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_getQueueTempo(): end\n"); }
	return (jint) nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeq
 * Method:    setQueueTempo
 * Signature: (ILorg/tritonus/lowlevel/alsa/AlsaSeq$QueueTempo;)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeq_setQueueTempo
(JNIEnv* env, jobject obj, jint nQueue, jobject queueTempoObj)
{
	snd_seq_t*		seq;
	snd_seq_queue_tempo_t*	pQueueTempo;
	int			nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_setQueueTempo(): begin\n"); }
	seq = getHandle(env, obj);
	pQueueTempo = getQueueTempoNativeHandle(env, queueTempoObj);
	//fprintf(debug_file, "pQueueTempo: %p\n", pQueueTempo); fflush(debug_file);
	nReturn = snd_seq_set_queue_tempo(seq, nQueue, pQueueTempo);
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_setQueueTempo(): snd_seq_set_queue_tempo() returns %d\n", nReturn); }
	if (nReturn < 0)
	{
		//throwRuntimeException(env, "snd_seq_set_queue_tempo() failed");
		throwRuntimeException(env, snd_strerror(nReturn));
	}
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_setQueueTempo(): end\n"); }
	return (jint) nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeq
 * Method:    getQueueTimer
 * Signature: (ILorg/tritonus/lowlevel/alsa/AlsaSeq$QueueTimer;)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeq_getQueueTimer
(JNIEnv* env, jobject obj, jint nQueue, jobject queueTimerObj UNUSED)
{
	snd_seq_t*		seq;
	snd_seq_queue_timer_t*	pQueueTimer;
	int			nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_getQueueTimer(): begin\n"); }
	seq = getHandle(env, obj);
	// TODO: implement timer API, then re-enable
	pQueueTimer = NULL;	// getQueueTimerNativeHandle(env, queueTimerObj);
	nReturn = snd_seq_get_queue_timer(seq, nQueue, pQueueTimer);
	if (nReturn < 0)
	{
		throwRuntimeException(env, "snd_seq_get_queue_timer() failed");
	}
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_getQueueTimer(): end\n"); }
	return (jint) nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeq
 * Method:    setQueueTimer
 * Signature: (ILorg/tritonus/lowlevel/alsa/AlsaSeq$QueueTimer;)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeq_setQueueTimer
(JNIEnv* env, jobject obj, jint nQueue, jobject queueTimerObj UNUSED)
{
	snd_seq_t*		seq;
	snd_seq_queue_timer_t*	pQueueTimer;
	int			nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_setQueueTimer(): begin\n"); }
	seq = getHandle(env, obj);
	// TODO:
	pQueueTimer = NULL;	// getQueueTimerNativeHandle(env, queueTimerObj);
	nReturn = snd_seq_set_queue_timer(seq, nQueue, pQueueTimer);
	if (nReturn < 0)
	{
		throwRuntimeException(env, "snd_seq_set_queue_timer() failed");
	}
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_setQueueTimer(): end\n"); }
	return (jint) nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeq
 * Method:    getPortSubscription
 * Signature: (Lorg/tritonus/lowlevel/alsa/AlsaSeq$PortSubscribe;)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeq_getPortSubscription
(JNIEnv* env, jobject obj, jobject portSubscribeObj)
{
	snd_seq_t*			seq;
	snd_seq_port_subscribe_t*	pPortSubscribe;
	int				nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_getPortSubscription(): begin\n"); }
	seq = getHandle(env, obj);
	pPortSubscribe = getPortSubscribeNativeHandle(env, portSubscribeObj);
	nReturn = snd_seq_get_port_subscription(seq, pPortSubscribe);
	if (nReturn < 0)
	{
		throwRuntimeException(env, "snd_seq_get_port_subscription() failed");
	}
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_getPortSubscription(): end\n"); }
	return (jint) nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeq
 * Method:    subscribePort
 * Signature: (Lorg/tritonus/lowlevel/alsa/AlsaSeq$PortSubscribe;)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeq_subscribePort
(JNIEnv* env, jobject obj, jobject portSubscribeObj)
{
	snd_seq_t*			seq;
	snd_seq_port_subscribe_t*	pPortSubscribe;
	int				nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_subscribePort(): begin\n"); }
	seq = getHandle(env, obj);
	pPortSubscribe = getPortSubscribeNativeHandle(env, portSubscribeObj);
	nReturn = snd_seq_subscribe_port(seq, pPortSubscribe);
	if (nReturn < 0)
	{
		throwRuntimeException(env, "snd_seq_subscribe_port() failed");
	}
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_subscribePort(): end\n"); }
	return (jint) nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeq
 * Method:    unsubscribePort
 * Signature: (Lorg/tritonus/lowlevel/alsa/AlsaSeq$PortSubscribe;)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeq_unsubscribePort
(JNIEnv* env, jobject obj, jobject portSubscribeObj)
{
	snd_seq_t*			seq;
	snd_seq_port_subscribe_t*	pPortSubscribe;
	int				nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_unsubscribePort(): begin\n"); }
	seq = getHandle(env, obj);
	pPortSubscribe = getPortSubscribeNativeHandle(env, portSubscribeObj);
	nReturn = snd_seq_unsubscribe_port(seq, pPortSubscribe);
	if (nReturn < 0)
	{
		throwRuntimeException(env, "snd_seq_unsubscribe_port() failed");
	}
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_unsubscribePort(): end\n"); }
	return (jint) nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeq
 * Method:    setTrace
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeq_setTrace
(JNIEnv* env UNUSED, jclass cls UNUSED, jboolean bTrace)
{
	debug_flag = bTrace;
	debug_file = stderr;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeq
 * Method:    eventOutput
 * Signature: (Lorg/tritonus/lowlevel/alsa/AlsaSeq$Event;)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeq_eventOutput
(JNIEnv* env, jobject obj, jobject eventObj)
{
	snd_seq_t*		seq;
	snd_seq_event_t*	event;
	int			nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_eventOutput(): begin\n"); }
	seq = getHandle(env, obj);
	event = getEventNativeHandle(env, eventObj);
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_eventOutput(): length of event: %d\n", snd_seq_event_length(event)); }
	nReturn = snd_seq_event_output(seq, event);
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_eventOutput(): snd_seq_event_output() returns %d\n", nReturn); }
	if (nReturn < 0)
	{
		throwRuntimeException(env, "snd_seq_event_output() failed");
	}
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_eventOutput(): end\n"); }
	return (jint) nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeq
 * Method:    eventOutputBuffer
 * Signature: (Lorg/tritonus/lowlevel/alsa/AlsaSeq$Event;)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeq_eventOutputBuffer
(JNIEnv* env, jobject obj, jobject eventObj)
{
	snd_seq_t*		seq;
	snd_seq_event_t*	event;
	int			nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_eventOutputBuffer(): begin\n"); }
	seq = getHandle(env, obj);
	event = getEventNativeHandle(env, eventObj);
	nReturn = snd_seq_event_output_buffer(seq, event);
	if (nReturn < 0)
	{
		throwRuntimeException(env, "snd_seq_event_output_buffer() failed");
	}
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_eventOutputBuffer(): end\n"); }
	return (jint) nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeq
 * Method:    eventOutputDirect
 * Signature: (Lorg/tritonus/lowlevel/alsa/AlsaSeq$Event;)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeq_eventOutputDirect
(JNIEnv* env, jobject obj, jobject eventObj)
{
	snd_seq_t*		seq;
	snd_seq_event_t*	event;
	int			nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_eventOutputDirect(): begin\n"); }
	seq = getHandle(env, obj);
	event = getEventNativeHandle(env, eventObj);
	nReturn = snd_seq_event_output_direct(seq, event);
	if (nReturn < 0)
	{
		throwRuntimeException(env, "snd_seq_event_output_direct() failed");
	}
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_eventOutputDirect(): end\n"); }
	return (jint) nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeq
 * Method:    eventInput
 * Signature: (Lorg/tritonus/lowlevel/alsa/AlsaSeq$Event;)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeq_eventInput
(JNIEnv* env, jobject obj, jobject eventObj)
{
	snd_seq_t*		seq;
	snd_seq_event_t*	event;
	snd_seq_event_t*	handle;
	int			nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_eventInput(): begin\n"); }
	seq = getHandle(env, obj);
	/*
	 *	snd_seq_event_input() results in a blocking read on a
	 *	device file. There are two problems:
	 *	1. green threads VMs do no blocking read. Therefore, this
	 *	code doesn't work with green threads at all. A solution is
	 *	outstanding.
	 *	2. In some cases, the read is interrupted by a signal. This
	 *	is the reason for the do..while.
	 */
	do
	{
		// printf("1\n");
		//errno = 0;
		event = NULL;
		nReturn = snd_seq_event_input(seq, &event);
		//printf("return: %d\n", nReturn);
		// printf("event: %p\n", pEvent);
		// printf("errno: %d\n", errno);
		//perror("abc");
		// printf("2\n");
	}
	// TODO: should catch -EAGAIN, too?
	while (nReturn == -EINTR);
// 	if (nReturn < 0)
// 	{
// 		throwRuntimeException(env, "snd_seq_event_input() failed");
// 	}
	if (event != NULL)
	{
		/*
		  - get snd_seq_event_t handle of Event instance
		  - if varlength, free associated mem
		  - copy event to handle's memory
		  - if varlength, allocate new memory for var part
		  - copy data to new memory
		  - set new var pointer
		  TODO: make thread-safe (should acquire a lock)
		  or: document behaviour
		 */
		handle = getEventNativeHandle(env, eventObj);
		if (snd_seq_ev_is_variable(handle))
		{
			free(handle->data.ext.ptr);
		}
		*handle = *event;
		if (snd_seq_ev_is_variable(handle))
		{
			char*	ptr = (char*) malloc(handle->data.ext.len);
			memcpy(ptr, handle->data.ext.ptr, handle->data.ext.len);
			handle->data.ext.ptr = ptr;
		}
	}
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_eventInput(): end\n"); }
	return (jint) nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeq
 * Method:    eventInputPending
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeq_eventInputPending
(JNIEnv* env, jobject obj, jint nFetchSequencer)
{
	snd_seq_t*		seq;
	int			nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_eventInputPending(): begin\n"); }
	seq = getHandle(env, obj);
	nReturn = snd_seq_event_input_pending(seq, nFetchSequencer);
	if (nReturn < 0)
	{
		throwRuntimeException(env, "snd_seq_event_input_pending() failed");
	}
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_eventInputPending(): end\n"); }
	return (jint) nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeq
 * Method:    drainOutput
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeq_drainOutput
(JNIEnv* env, jobject obj)
{
	snd_seq_t*		seq;
	int			nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_drainOutput(): begin\n"); }
	seq = getHandle(env, obj);
	nReturn = snd_seq_drain_output(seq);
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_drainOutput(): snd_seq_drain_output() returned %d\n", nReturn); }
	if (nReturn < 0)
	{
		throwRuntimeException(env, "snd_seq_drain_output() failed");
	}
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_drainOutput(): end\n"); }
	return (jint) nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeq
 * Method:    eventOutputPending
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeq_eventOutputPending
(JNIEnv* env, jobject obj)
{
	snd_seq_t*		seq;
	int			nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_eventOutputPending(): begin\n"); }
	seq = getHandle(env, obj);
	nReturn = snd_seq_event_output_pending(seq);
	if (nReturn < 0)
	{
		throwRuntimeException(env, "snd_seq_event_output_direct() failed");
	}
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_eventOutputPending(): end\n"); }
	return (jint) nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeq
 * Method:    extractOutput
 * Signature: (Lorg/tritonus/lowlevel/alsa/AlsaSeq$Event;)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeq_extractOutput
(JNIEnv* env, jobject obj, jobject eventObj)
{
	snd_seq_t*		seq;
	snd_seq_event_t*	event;
	snd_seq_event_t*	handle;
	int			nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_extractOutput(): begin\n"); }
	seq = getHandle(env, obj);
	nReturn = snd_seq_extract_output(seq, &event);
	if (nReturn < 0)
	{
		throwRuntimeException(env, "snd_seq_extrct_output() failed");
	}
	handle = getEventNativeHandle(env, eventObj);
	if (snd_seq_ev_is_variable(handle))
	{
		free(handle->data.ext.ptr);
	}
	*handle = *event;
	if (snd_seq_ev_is_variable(handle))
	{
		char*	ptr = (char*) malloc(handle->data.ext.len);
		memcpy(ptr, handle->data.ext.ptr, handle->data.ext.len);
		handle->data.ext.ptr = ptr;
	}
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_extractOutput(): end\n"); }
	return (jint) nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeq
 * Method:    dropOutput
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeq_dropOutput
(JNIEnv* env, jobject obj)
{
	snd_seq_t*		seq;
	int			nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_dropOutput(): begin\n"); }
	seq = getHandle(env, obj);
	nReturn = snd_seq_drop_output(seq);
	if (nReturn < 0)
	{
		throwRuntimeException(env, "snd_seq_drop_output() failed");
	}
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_dropOutput(): end\n"); }
	return (jint) nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeq
 * Method:    dropOutputBuffer
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeq_dropOutputBuffer
(JNIEnv* env, jobject obj)
{
	snd_seq_t*		seq;
	int			nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_dropOutputBuffer(): begin\n"); }
	seq = getHandle(env, obj);
	nReturn = snd_seq_drop_output_buffer(seq);
	if (nReturn < 0)
	{
		throwRuntimeException(env, "snd_seq_drop_output_buffer() failed");
	}
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_dropOutputBuffer(): end\n"); }
	return (jint) nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeq
 * Method:    dropInput
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeq_dropInput
(JNIEnv* env, jobject obj)
{
	snd_seq_t*		seq;
	int			nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_dropInput(): begin\n"); }
	seq = getHandle(env, obj);
	nReturn = snd_seq_drop_input(seq);
	if (nReturn < 0)
	{
		throwRuntimeException(env, "snd_seq_drop_input() failed");
	}
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_dropInput(): end\n"); }
	return (jint) nReturn;
}



/*
 * Class:     org_tritonus_lowlevel_alsa_AlsaSeq
 * Method:    dropInputBuffer
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_alsa_AlsaSeq_dropInputBuffer
(JNIEnv* env, jobject obj)
{
	snd_seq_t*		seq;
	int			nReturn;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_dropInputBuffer(): begin\n"); }
	seq = getHandle(env, obj);
	nReturn = snd_seq_drop_input_buffer(seq);
	if (nReturn < 0)
	{
		throwRuntimeException(env, "snd_seq_drop_input_buffer() failed");
	}
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_AlsaSeq_dropInputBuffer(): end\n"); }
	return (jint) nReturn;
}



/*** org_tritonus_lowlevel_alsa_AlsaSeq.c ***/
