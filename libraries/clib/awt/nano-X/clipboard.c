/**
 * nano-X AWT backend for Kaffe.
 *
 * Copyright (c) 2001
 *	Exor International Inc. All rights reserved.
 *
 * Copyright (c) 2001
 *	Sinby Corporatin, All rights reserved.
 *
 * Copyright (c) 2005
 *	Kaffe.org contributors, see ChangeLog for details.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "config-setjmp.h"

#include "toolkit.h"

#include "jni.h"
#include <ctype.h>

static jclass     NativeClipboard;
static jmethodID  lostOwnership;
static jmethodID  createTransferable;
static jmethodID  getNativeData;

#define MIME0 "application/x-java-serialized-object"
#define MIME1 "text/plain; charset=unicode"

jobject
Java_java_awt_Toolkit_cbdInitClipboard(JNIEnv* envP, jclass clazz )
{
	GR_WINDOW_ID wid;

	wid = GrNewWindow(GR_ROOT_WINDOW_ID, 0, 0, 1, 1, 0, 0, 0);
	GrSelectEvents(wid, GR_EVENT_TYPE_CLIENT_DATA_REQ);

	NativeClipboard = (*envP)->FindClass( envP, "java/awt/NativeClipboard");

	getNativeData = (*envP)->GetStaticMethodID( envP, NativeClipboard, "getNativeData", "(Ljava/lang/String;)[B");
	lostOwnership   = (*envP)->GetStaticMethodID( envP, NativeClipboard, "lostOwnership", "()V");
	createTransferable = (*envP)->GetStaticMethodID( envP, NativeClipboard, "createTransferable", "(Ljava/lang/String;[B)Ljava/awt/datatransfer/Transferable;");

	return (jobject)wid;
}

void
Java_java_awt_Toolkit_cbdFreeClipboard( JNIEnv* env, jclass clazz, jobject _jwindow)
{
	GR_WINDOW_ID wid;

	if ( _jwindow == NULL ) {
		SignalError("java.lang.NullPointerException", "no window object");
		return;
	}
	wid = (GR_WINDOW_ID)_jwindow;
	GrDestroyWindow(wid);
}

jboolean
Java_java_awt_Toolkit_cbdSetOwner( JNIEnv* env, jclass clazz, jobject _jwindow)
{
	GR_WINDOW_ID wid, owid;
	GR_CHAR *cp;

	if ( _jwindow == NULL ) {
		SignalError("java.lang.NullPointerException", "no window object");
		return JNI_FALSE;
	}
	wid = (GR_WINDOW_ID)_jwindow;

	GrSetSelectionOwner(wid, MIME0 " " MIME1);
	
	owid = GrGetSelectionOwner(&cp);
	free(cp);

	if ( owid == wid ) {
		return JNI_TRUE;
	} else {
		return JNI_FALSE;
	}
}

void
sendClipboardData(JNIEnv *envP, GR_EVENT_CLIENT_DATA_REQ *eventP)
{
	jstring jMimeType;
	jbyteArray jData;
	jbyte *data;
	int len;
	const char *mime;
	jboolean isCopy;

	switch ( eventP->mimetype == 0 ) {
	case 0:
		mime = "application/x-java-serialized-object";
		break;
	case 1:
		mime = "text/plain; charset=unicode";
		break;
	default:
		return;
	}

	jMimeType = (*envP)->NewStringUTF( envP, mime );
	jData = (*envP)->CallStaticObjectMethod( envP, NativeClipboard, getNativeData, jMimeType);

	if ( jData ) {
		data = (*envP)->GetByteArrayElements( envP, jData, &isCopy);
		len = (*envP)->GetArrayLength( envP, jData);

		(*envP)->ReleaseByteArrayElements( envP, jData, data, JNI_ABORT);
	} else {
		data = NULL;
		len = 0;
	}

	GrSendClientData(eventP->wid, eventP->rid, eventP->serial, len, len, data);

	return;
}

static inline char*
getMime(char **icp)
{
	char c,*cp,*rv;
	cp = *icp;
	while(((( c = *cp ) == ' ') || ( c == '\t') || ( c == '\n' )) && (c != NULL)) {
		cp++;
	}
	if ( c == NULL ) {
		return NULL;
	}
	rv = cp;

retry:
	while((( c = *cp ) != ' ') && ( c != '\t') && ( c != '\n' ) && (c != NULL)&& c != ';') {
		cp++;
	}
	if ( c != ';' ) {
		if ( c == NULL ) {
			*icp = cp;
		} else {
			*cp = NULL;
			*icp = cp+1;
		}
		return rv;
	}
	cp++;
	while(((( c = *cp ) == ' ') || ( c == '\t') || ( c == '\n' )) && (c != NULL)) {
		cp++;
	}

	if ( c == NULL ) {
		*icp = cp;
		return rv;
	}
goto retry;
}

static inline int
equaleMime(char *mime0, char *mime1) 
{
	char c0,c1;

retry:
	while(((( c0 = *mime0 ) == ' ') || ( c0 == '\t') || ( c0 == '\n' )) && (c0 != NULL)) {
		mime0++;
	}
	while(((( c1 = *mime1 ) == ' ') || ( c1 == '\t') || ( c1 == '\n' )) && (c1 != NULL)) {
		mime1++;
	}
	if (( c0 == NULL ) && ( c1 == NULL )) {
		return 1;
	}
	if ( tolower(c0) != tolower(c1) ) {
		return 0;
	}
	mime0++;
	mime1++;
	goto retry;
}

jobject
Java_java_awt_Toolkit_cbdGetContents(JNIEnv* envP, jclass clazz, jobject _jwindow)
{
	GR_WINDOW_ID wid, owid;
	GR_MIMETYPE mimetype;
	static GR_SERIALNO serialNo = 0;
	char *mime,*icp;
	GR_EVENT event;
	jstring jMimeType;
	jbyteArray jdata;
	GR_CHAR *typelist;

	if ( _jwindow == NULL ) {
		SignalError("java.lang.NullPointerException", "no window object");
		return NULL;
	}
	wid = (GR_WINDOW_ID)_jwindow;

	owid = GrGetSelectionOwner(&typelist);
	if (( owid == NULL ) || ( typelist == NULL )) {
		return NULL;
	}
	mimetype = 0;
	icp = typelist;
	while (( mime = getMime(&icp) ) != NULL) {
		if ( equaleMime(mime, MIME0)) {
			break;
		}
		if ( equaleMime(mime, MIME1)) {
			break;
		}
		
		mimetype++; 
	}
	jMimeType = (*envP)->NewStringUTF(envP, (const char*)mime);
	free(typelist);

	if ( mime == NULL ) {
		return NULL;
	}

	GrSelectEvents(wid, GR_EVENT_MASK_CLIENT_DATA);

	serialNo++;
	GrRequestClientData(wid, owid, serialNo, mimetype);

	GrGetNextEventTimeout(&event, 1500);
	switch(event.clientdata.type) {
		case GR_EVENT_TYPE_CLIENT_DATA:
			break;
		case GR_EVENT_TYPE_TIMEOUT:
			return NULL;
		default:
			return NULL;
	}
	if ( event.clientdata.serial != serialNo ) {
		return NULL;
	}
	if ( event.clientdata.len != event.clientdata.datalen ) {
		return NULL;
	}

	if ( event.clientdata.datalen == 0 ) {
		return NULL;
	}
	if ( event.clientdata.data == 0 ) {
		SignalError("java.lang.NullInternalError", "illegal data pointer");
		return NULL;
	}
	GrSelectEvents(wid, 0);

	jdata = (*envP)->NewByteArray(envP, event.clientdata.datalen);
	(*envP)->SetByteArrayRegion( envP, jdata, 0, event.clientdata.datalen, (jbyte*)event.clientdata.data);

	return (*envP)->CallStaticObjectMethod( envP, NativeClipboard, createTransferable, jMimeType, jdata);
}

