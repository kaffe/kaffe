/**
* cbd.cpp - clipboard handling
*
* Copyright (c) 1999
*      Transvirtual Technologies, Inc.  All rights reserved.
*
* See the file "license.terms" for information on usage and redistribution 
* of this file. 
*/

#include "toolkit.hpp"


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
	env->CallStaticVoidMethod( NativeClipboard, lostOwnership);
	
	return 0; /* don't pass event to Java */
}

jobject 
selectionRequest ( JNIEnv* env, Toolkit* X )
{
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
getRawData ( Toolkit* X, void* target, unsigned char** pData )
{
	return -1; /* no selection owner at all */
}


/*****************************************************************************************
* exported functions
*/

extern "C" {
	jobject __cdecl
		Java_java_awt_Toolkit_cbdInitClipboard ( JNIEnv* env, jclass clazz )
	{
		return 0;
	}
	
	void __cdecl
		Java_java_awt_Toolkit_cbdFreeClipboard ( JNIEnv* env, jclass clazz, ClipBoard* cbd )
	{
	}
	
	jboolean __cdecl
		Java_java_awt_Toolkit_cbdSetOwner ( JNIEnv* env, jclass clazz, ClipBoard* cbd )
	{
		return 1;
	}
	
	
	/*
	* this is the incoming handler
	* there is no notion of a type request from Java, we therefore have to settle on
	* a stack of suitable formats (with most specific type on top, "fallback" type at bottom)
	*/
	
	jobject __cdecl
		Java_java_awt_Toolkit_cbdGetContents ( JNIEnv* env, jclass clazz, ClipBoard* cbd )
	{
		return 0;
	}
	
}
