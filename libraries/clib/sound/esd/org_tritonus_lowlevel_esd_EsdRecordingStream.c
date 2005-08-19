/*
 *	org_tritonus_lowlevel_esd_EsdRecordingStream.cc
 */

#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include "common.h"
#include "org_tritonus_lowlevel_esd_EsdRecordingStream.h"


// static HandleFieldHandler<int>	handler;
HandleFieldHandlerDeclaration(handler,int)


/*
 * Class:     org_tritonus_lowlevel_esd_EsdRecordingStream
 * Method:    open
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_esd_EsdRecordingStream_open
  (JNIEnv *env, jobject obj, jint nFormat, jint nSampleRate)
{
	int		nFd = -1;
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_esd_EsdRecordingStream_open(): begin\n"); }
	nFd = esd_record_stream/*_fallback*/(nFormat, nSampleRate, NULL, "abc");
	if (nFd < 0)
	{
		// cerr << "cannot create esd stream\n";
		// jclass	cls = env->FindClass("org/tritonus_lowlevel/nas/DeviceAttributes");
		// env->ThrowNew(cls, "exc: cannot create esd stream");
	}
	setHandle(env, obj, nFd);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_esd_EsdRecordingStream_open(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_esd_EsdRecordingStream
 * Method:    read
 * Signature: ([BII)I
 */
JNIEXPORT jint JNICALL
Java_org_tritonus_lowlevel_esd_EsdRecordingStream_read
(JNIEnv *env, jobject obj, jbyteArray abData, jint nOffset, jint nLength)
{
	int		nFd = -1;
	int		nBytesRead = 0;
	jbyte		buffer[nLength];

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_esd_EsdRecordingStream_read(): begin\n"); }
	nFd = getHandle(env, obj);
	nBytesRead = read(nFd, buffer, nLength);
	if (nBytesRead > 0)
	{
		(*env)->SetByteArrayRegion(env, abData, nOffset, nBytesRead, buffer);
	}
	if (debug_flag)
	{
		printf("Java_org_tritonus_lowlevel_esd_EsdRecordingStream_read: Length: %d\n", (int) nLength);
		printf("Java_org_tritonus_lowlevel_esd_EsdRecordingStream_read: Read: %d\n", nBytesRead);
	}
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_esd_EsdRecordingStream_read(): end\n"); }
	return nBytesRead;
}



/*
 * Class:     org_tritonus_lowlevel_esd_EsdRecordingStream
 * Method:    close
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_esd_EsdRecordingStream_close
(JNIEnv *env, jobject obj)
{
	int		nFd = -1;

	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_esd_EsdRecordingStream_close(): begin\n"); }
	nFd = getHandle(env, obj);
	close(nFd);
	setHandle(env, obj, -1);
	if (debug_flag) { fprintf(debug_file, "Java_org_tritonus_lowlevel_esd_EsdRecordingStream_close(): end\n"); }
}



/*
 * Class:     org_tritonus_lowlevel_esd_EsdStream
 * Method:    setTrace
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL
Java_org_tritonus_lowlevel_esd_EsdRecordingStream_setTrace
(JNIEnv* env UNUSED, jclass cls UNUSED, jboolean bTrace)
{
	debug_flag = bTrace;
	debug_file = stderr;
}


/*** org_tritonus_lowlevel_esd_EsdRecordingStream.cc ***/
