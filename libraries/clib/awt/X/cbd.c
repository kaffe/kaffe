/**
 * cbd.c - clipboard handling
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include "toolkit.h"


jclass     NativeClipboard;
jmethodID  lostOwnership;
jmethodID  createTransferable;
jmethodID  getNativeData;

typedef void ClipBoard;


/*****************************************************************************************
 * internal event handling functions
 *  (note that non of them gets back to Java via events (->return 0);
 */

jobject
selectionClear ( JNIEnv* env, Toolkit* X )
{
  (*env)->CallStaticVoidMethod( env, NativeClipboard, lostOwnership);

  return 0; /* don't pass event to Java */
}

jobject
selectionRequest ( JNIEnv* env, Toolkit* X )
{
  XEvent     e;
  char       *mime;
  jstring    jMimeType;
  jarray     jData;
  jbyte      *data;
  jboolean   isCopy;
  int        len;
  Atom       target = X->event.xselectionrequest.target;

  if ( target == JAVA_OBJECT )
	mime = "application/x-java-serialized-object";
  else if ( target == XA_STRING )
	mime = "text/plain; charset=unicode";
  else
	mime = XGetAtomName( X->dsp, target);

  jMimeType = (*env)->NewStringUTF( env, (const char*) mime);
  jData = (*env)->CallStaticObjectMethod( env, NativeClipboard, getNativeData, jMimeType);

  if ( jData ) {
	data = (*env)->GetByteArrayElements( env, jData, &isCopy);
	len = (*env)->GetArrayLength( env, jData);

	XChangeProperty( X->dsp,
					 X->event.xselectionrequest.requestor,
					 X->event.xselectionrequest.property,
					 X->event.xselectionrequest.target,
					 8, PropModeReplace, data, len);

	(*env)->ReleaseByteArrayElements( env, jData, data, JNI_ABORT);
	e.xselection.property  = X->event.xselectionrequest.property;
  }
  else {
	e.xselection.property  = None;  /* format not supported */
  }

  e.xselection.type      = SelectionNotify;
  e.xselection.requestor = X->event.xselectionrequest.requestor;
  e.xselection.selection = X->event.xselectionrequest.selection;
  e.xselection.target    = target;
  e.xselection.time      = X->event.xselectionrequest.time;

  XSendEvent( X->dsp, e.xselection.requestor, False, 0, &e);

  return 0; /* don't pass event to Java */
}



/*
 * this is the low level helper - get the raw data of the requested 'target' format
 * returns :
 *   -1 if there is no selection owner
 *    0 if the owner cannot prvide the requested format
 *    length of *pData otherwise
 */
int
getRawData ( Toolkit* X, Atom target, unsigned char** pData )
{
  int              i, format;
  unsigned long    len = 8191, remain = 1;
  Atom             type;
  XEvent           e;

  XConvertSelection( X->dsp, XA_PRIMARY, target, SELECTION_DATA, X->cbdOwner, CurrentTime);

  for ( i=0; i<2; i++ ) {
	XSync( X->dsp, False);
	if ( XCheckTypedWindowEvent( X->dsp, X->cbdOwner, SelectionNotify, &e) ){
	  if ( e.xselection.property == None )  /* target type not supported by owner */
		return 0;

	  while ( remain ) {
		len += remain;
		XGetWindowProperty( X->dsp, X->cbdOwner, SELECTION_DATA, 0, len, False,
							AnyPropertyType, &type, &format, &len, &remain, pData);
	  }
	  return len;
	}
	sleep( 1);
  }

  return -1; /* no selection owner at all */
}


/*****************************************************************************************
 * exported functions
 */

jobject
Java_java_awt_Toolkit_cbdInitClipboard ( JNIEnv* env, jclass clazz )
{
  unsigned long mask = 0;
  XSetWindowAttributes attrs;

  attrs.override_redirect = True;
  mask |= CWOverrideRedirect;

  /*
   * We need an appropriate native owner/requestor (Window) since
   * Java requestors/owners can be non-native Windows. We also
   * don't want to introduce artificial events (with all their
   * required infrastructure)
   */
  X->cbdOwner = XCreateWindow( X->dsp, X->root, -10000, -10000, 1, 1, 0, CopyFromParent,
							   InputOutput, CopyFromParent, mask, &attrs);

  NativeClipboard = (*env)->FindClass( env, "java/awt/NativeClipboard");
  lostOwnership   = (*env)->GetStaticMethodID( env, NativeClipboard,
											   "lostOwnership", "()V");
  getNativeData = (*env)->GetStaticMethodID( env, NativeClipboard,
											   "getNativeData", "(Ljava/lang/String;)[B");
  createTransferable = (*env)->GetStaticMethodID( env, NativeClipboard,
												  "createTransferable",
												  "(Ljava/lang/String;[B)Ljava/awt/datatransfer/Transferable;");

  SELECTION_DATA   = XInternAtom( X->dsp, "SELECTION_DATA", False);
  JAVA_OBJECT      = XInternAtom( X->dsp, "application/x-java-serialized-object", False);

  return 0;
}

void
Java_java_awt_Toolkit_cbdFreeClipboard ( JNIEnv* env, jclass clazz, ClipBoard* cbd )
{
}

jboolean
Java_java_awt_Toolkit_cbdSetOwner ( JNIEnv* env, jclass clazz, ClipBoard* cbd )
{
  XSetSelectionOwner( X->dsp, XA_PRIMARY, X->cbdOwner, CurrentTime);
  if ( XGetSelectionOwner( X->dsp, XA_PRIMARY) != X->cbdOwner )
	return 0;
  else
	return 1;
}


/*
 * this is the incoming handler
 * there is no notion of a type request from Java, we therefore have to settle on
 * a stack of suitable formats (with most specific type on top, "fallback" type at bottom)
 */

jobject
Java_java_awt_Toolkit_cbdGetContents ( JNIEnv* env, jclass clazz, ClipBoard* cbd )
{
  int             ret;
  unsigned char   *data = 0;
  char            *mime = 0;
  jarray          jdata;
  jstring         jMimeType;

  if ( (ret = getRawData( X, JAVA_OBJECT, &data)) ){
	mime = "application/x-java-serialized-object";
  }
  else if ( (ret == 0) && (ret = getRawData( X, XA_STRING, &data)) ){
	mime = "text/plain; charset=unicode";
  }

  if ( data ) {
	jMimeType = (*env)->NewStringUTF( env, (const char*)mime);
	jdata = (*env)->NewByteArray( env, ret);
	(*env)->SetByteArrayRegion( env, jdata, 0, ret, (jbyte*)data);
	XFree( data);

	return (*env)->CallStaticObjectMethod( env, NativeClipboard, createTransferable,
										   jMimeType, jdata);
  }
  else {
	return 0;
  }
}
