/**
 * cbd.c - clipboard handling
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include <qapplication.h>
#include <qclipboard.h>
#include <qdragobject.h>

#include "toolkit.h"

jclass     NativeClipboard;
jmethodID  lostOwnership;
jmethodID  createTransferable;
jmethodID  getNativeData;

typedef void ClipBoard;
QString cbdText;
int cbdDirty;
/*****************************************************************************************
 * internal event handling functions
 *  (note that non of them gets back to Java via events (->return 0);
 */
#if 0
jobject
selectionClear ( JNIEnv* env, Toolkit* X )
{
  env->CallStaticVoidMethod( NativeClipboard, lostOwnership);

  return 0; /* don't pass event to Java */
}
jobject
selectionRequest ( JNIEnv* env, Toolkit* X )
{
  //XEvent     e;
  char       *mime;
  jstring    jMimeType;
  jbyteArray jData;
  jbyte      *data;
  jboolean   isCopy;
  int        len;
  //Atom       target = X->event.xselectionrequest.target;

  if ( target == JAVA_OBJECT )
	mime = "application/x-java-serialized-object";
  else if ( target == XA_STRING )
	mime = "text/plain; charset=unicode";
  else
	mime = XGetAtomName( X->dsp, target);

  jMimeType = env->NewStringUTF( (const char*) mime);
  jData = env->CallStaticObjectMethod( NativeClipboard, getNativeData, jMimeType);

  if ( jData ) {
	data = env->GetByteArrayElements( jData, &isCopy);
	len = env->GetArrayLength( jData);

	//XChangeProperty( X->dsp,
	//				 X->event.xselectionrequest.requestor,
	//				 X->event.xselectionrequest.property,
	//				 X->event.xselectionrequest.target,
	//				 8, PropModeReplace, data, len);

	env->ReleaseByteArrayElements( jData, data, JNI_ABORT);
	//e.xselection.property  = X->event.xselectionrequest.property;
  }
  else {
	//e.xselection.property  = None;  /* format not supported */
  }
/*
  e.xselection.type      = SelectionNotify;
  e.xselection.requestor = X->event.xselectionrequest.requestor;
  e.xselection.selection = X->event.xselectionrequest.selection;
  e.xselection.target    = target;
  e.xselection.time      = X->event.xselectionrequest.time;

  XSendEvent( X->dsp, e.xselection.requestor, False, 0, &e);
*/
  return 0; /* don't pass event to Java */
}
#endif

/*
 * this is the low level helper - get the raw data of the requested 'target' format
 * returns :
 *   -1 if there is no selection owner
 *    0 if the owner cannot prvide the requested format
 *    length of *pData otherwise
 */
#if 0
static int
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
#endif

/*****************************************************************************************
 * exported functions
 */

jobject
Java_java_awt_Toolkit_cbdInitClipboard ( JNIEnv* env, jclass clazz )
{
#if 0
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

#endif

  NativeClipboard = env->FindClass( "java/awt/NativeClipboard");
  lostOwnership   = env->GetStaticMethodID( NativeClipboard,
											   "lostOwnership", "()V");
  getNativeData = env->GetStaticMethodID( NativeClipboard,
											   "getNativeData", "(Ljava/lang/String;)[B");
  createTransferable = env->GetStaticMethodID( NativeClipboard,
												  "createTransferable",
												  "(Ljava/lang/String;[B)Ljava/awt/datatransfer/Transferable;");

//  SELECTION_DATA   = XInternAtom( X->dsp, "SELECTION_DATA", False);
//  JAVA_OBJECT      = XInternAtom( X->dsp, "application/x-java-serialized-object", False);
  return 0;
}

void
Java_java_awt_Toolkit_cbdFreeClipboard ( JNIEnv* env, jclass clazz, ClipBoard* cbd )
{
}

jboolean
Java_java_awt_Toolkit_cbdSetOwner ( JNIEnv* env, jclass clazz, ClipBoard* cbd )
{

  cbdDirty = 1;  
  return 1;

#if 0
  XSetSelectionOwner( X->dsp, XA_PRIMARY, X->cbdOwner, CurrentTime);
  if ( XGetSelectionOwner( X->dsp, XA_PRIMARY) != X->cbdOwner )
        return 0;
  else
        return 1;
#endif
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
  jbyteArray      jdata;
  jstring         jMimeType;

#if 0   
  if ( (ret = getRawData( X, JAVA_OBJECT, &data)) ){
	mime = "application/x-java-serialized-object";
  }
  else if ( (ret == 0) && (ret = getRawData( X, XA_STRING, &data)) ){
	mime = "text/plain; charset=unicode";
  }
#endif
#if 0
  if(QTextDrag::canDecode(QApplication::clipboard()->data())) {
	mime = "text/plain; charset=unicode";
	QString text;
	if(QTextDrag::decode(QApplication::clipboard()->data(), text))
	{
	  data = (unsigned char*)AWT_MALLOC(text.length());
	  memcpy(data,text.latin1(),text.length());
	  ret = text.length();
	}
  }	
  else {
	mime = "application/x-java-serialized-object";
	AWT_DBG(printf("Only text copy and paste is supported...\n"));
	//KEN : FIXIT
	//Do we need this?
  }
#endif
  if ( data ) {
	jMimeType = env->NewStringUTF( (const char*)mime);
	jdata = env->NewByteArray( ret);
	env->SetByteArrayRegion( jdata, 0, ret, (jbyte*)data);
	//XFree( data);

	return env->CallStaticObjectMethod( NativeClipboard, createTransferable,
										   jMimeType, jdata);
  }
  else {
	return 0;
  }
}

void pollJavaClipboard(JNIEnv *env)
{        
  char	        *mime;
  jstring	jMimeType;
  jbyteArray	jData;
  jbyte     	*data;
  jboolean  	isCopy;
  int       	len;   
#if 0
  mime = "text/plain; charset=unicode";	 
  if ( cbdDirty ) {
    //Post to QT clipboard
    jMimeType = env->NewStringUTF( (const char*) mime);
    jData = env->CallStaticObjectMethod( NativeClipboard, getNativeData,jMimeType);

    if ( jData ) {
        data = env->GetByteArrayElements( jData, &isCopy);
        len = env->GetArrayLength( jData);
        cbdText = QString((char *)data);  
        //QApplication::clipboard()->setData(new QTextDrag( cbdText));
	fprintf(stderr,"!!!Posting to Clipboard %s\n",data);
    } else {
        fprintf(stderr,"!!!Clipboard EMPTY!!!!\n");
    }
	cbdDirty = 0;
  }
#endif     
}    

jobject clearJavaClipboard(JNIEnv* env)
{
#if 0
  if(NativeClipboard && lostOwnership) {
    QString text;
    if(QTextDrag::canDecode(QApplication::clipboard()->data())) {
        if(QTextDrag::decode(QApplication::clipboard()->data(), text)) {
          //Poll native clipboard for new data
          if(text.compare(cbdText) != 0) {
            fprintf(stderr,"!!!lostOwnership!!!!\n");
            cbdText = text;
	    return env->CallStaticVoidMethod( NativeClipboard, lostOwnership);
	  }
        }  
    }
  }	  
#endif
  return NULL;
}
