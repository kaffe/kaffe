/**
* tlk.cpp - 
*
* Copyright (c) 1999
*      Transvirtual Technologies, Inc.  All rights reserved.
*
* See the file "license.terms" for information on usage and redistribution 
* of this file. 
*/


#define MAIN

#include "toolkit.hpp"
#include "tlkprops.hpp"

void
registerClasses()
{
	WNDCLASS	wc;
	
	// generic toplevel class
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = 0;
	wc.hIcon = APPICON;
	wc.lpszMenuName = 0;
	wc.hCursor = (HCURSOR)0;
	wc.hbrBackground = (HBRUSH)0;
	wc.lpszClassName = TEXT("window");
	
	RegisterClass ( &wc);
	
	//generic widget class ( used for canvas and panel)
	wc.style = CS_OWNDC;
	wc.hIcon = (HICON)0;
	wc.lpszClassName = TEXT("widget");
	
	RegisterClass ( &wc);
}


void enterDispatcherLoop() {
#if defined(D_LOOP_MODE)
	jmethodID initToolkit;
	MSG msg;
	
	X->dspTid = GetCurrentThreadId();
    initToolkit = JniEnv->GetStaticMethodID( Tlk, "initToolkit", "()V");
    JniEnv->CallStaticVoidMethod( Tlk, initToolkit);
	
    while ( GetMessage( &msg, NULL, 0, 0) == TRUE) {
		TranslateMessage( &msg);
		DispatchMessage( &msg);
	}
#endif
}

/********************************************************************************
* exported functions
*/

extern "C" {
	
	jint __cdecl
		Java_java_awt_Toolkit_tlkProperties ( JNIEnv* env, jclass clazz )
	{
		
#ifdef D_LOOP_MODE
		return (TLK_NATIVE_DISPATCHER_LOOP | TLK_IS_DISPATCH_EXCLUSIVE );
#else
		return (TLK_IS_BLOCKING | TLK_IS_DISPATCH_EXCLUSIVE );
#endif
	}
	
	jboolean __cdecl
		Java_java_awt_Toolkit_tlkInit ( JNIEnv* env, jclass clazz, jstring name )
	{
		X->nBuf = 128;
		X->buf = (TCHAR*)AWT_MALLOC( X->nBuf * sizeof(TCHAR));
		X->lastHwnd = (HWND)-1;
		
		Tlk = clazz;
		JniEnv = env;
		
		registerClasses();
		X->wakeUp = CreateWindow( TEXT("window"), TEXT(""), WS_OVERLAPPEDWINDOW, 0, 0, 0, 0,
			NULL, NULL, NULL, NULL);

		X->display = GetDC( X->wakeUp);

		enterDispatcherLoop();
		
		return JNI_TRUE;
	}
	
	void __cdecl
		Java_java_awt_Toolkit_tlkTerminate ( JNIEnv* env, jclass clazz )
	{
		DeleteDC( X->display);
		if ( X->wakeUp ) {
			DestroyWindow( X->wakeUp);
			X->wakeUp = 0;
		}
	}
	
	
	jstring __cdecl
		Java_java_awt_Toolkit_tlkVersion ( JNIEnv* env, jclass clazz )
	{
#if !defined(UNDER_CE)
		return env->NewStringUTF( "Win32");
#else
		return env->NewStringUTF( "WinCE");
#endif
	}
	
	jint __cdecl
		Java_java_awt_Toolkit_tlkGetResolution ( JNIEnv* env, jclass clazz )
	{
		return GetDeviceCaps( X->display, LOGPIXELSX );
	}
	
	jint __cdecl
		Java_java_awt_Toolkit_tlkGetScreenHeight ( JNIEnv* env, jclass clazz )
	{
		return GetSystemMetrics( SM_CYSCREEN);
	}
	
	jint __cdecl
		Java_java_awt_Toolkit_tlkGetScreenWidth ( JNIEnv* env, jclass clazz )
	{
		return GetSystemMetrics( SM_CYSCREEN);
	}
	
	jint __cdecl
		Java_java_awt_Toolkit_tlkGetMenubarHeight ( JNIEnv* env, jclass clazz )
	{
		return GetSystemMetrics( SM_CYMENU);
	}
	
	jint __cdecl
		Java_java_awt_Toolkit_tlkGetScrollbarWidth ( JNIEnv* env, jclass clazz )
	{
		return GetSystemMetrics( SM_CXVSCROLL);
	}
	
	void __cdecl
		Java_java_awt_Toolkit_tlkSync ( JNIEnv* env, jclass clazz )
	{
	}
	
	void __cdecl
		Java_java_awt_Toolkit_tlkFlush ( JNIEnv* env, jclass clazz )
	{
		GFLUSH();
	}
	
	
	void __cdecl
		Java_java_awt_Toolkit_tlkBeep ( JNIEnv* env, jclass clazz )
	{
		BEEP();
	}
	
	void __cdecl
		Java_java_awt_Toolkit_tlkDisplayBanner ( JNIEnv* env, jclass clazz, jstring banner )
	{
	}
	
}
