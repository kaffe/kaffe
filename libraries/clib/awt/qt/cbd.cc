/**
 * cbd.c - clipboard handling
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2002, 2003, 2004
 *	Kaffe.org contributors, see ChangeLog for details.  All rights reserved.
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

/**
 * exported functions
 */

jobject
Java_java_awt_Toolkit_cbdInitClipboard ( JNIEnv* env, jclass clazz )
{
  NativeClipboard = env->FindClass( "java/awt/NativeClipboard");
  lostOwnership   = env->GetStaticMethodID( NativeClipboard,
											   "lostOwnership", "()V");
  getNativeData = env->GetStaticMethodID( NativeClipboard,
											   "getNativeData", "(Ljava/lang/String;)[B");
  createTransferable = env->GetStaticMethodID( NativeClipboard,
												  "createTransferable",
												  "(Ljava/lang/String;[B)Ljava/awt/datatransfer/Transferable;");

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
	AWT_DBG(qqDebug("Only text copy and paste is supported...\n"));
	//KEN : FIXIT
	//Do we need this?
  }
#endif
  if ( data ) {
	jMimeType = env->NewStringUTF( (const char*)mime);
	jdata = env->NewByteArray( ret);
	env->SetByteArrayRegion( jdata, 0, ret, (jbyte*)data);

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
	qFatal("!!!Posting to Clipboard %s\n",data);
    } else {
        qFatal("!!!Clipboard EMPTY!!!!\n");
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
            qFatal("!!!lostOwnership!!!!\n");
            cbdText = text;
	    return env->CallStaticVoidMethod( NativeClipboard, lostOwnership);
	  }
        }  
    }
  }	  
#endif
  return NULL;
}

