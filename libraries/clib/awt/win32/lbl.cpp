/**
* lbl.cpp - static text
*
* Copyright (c) 1999
*      Transvirtual Technologies, Inc.  All rights reserved.
*
* See the file "license.terms" for information on usage and redistribution 
* of this file. 
*/

#include "toolkit.hpp"

class KLabel:public KWnd {
public:		
	KLabel( HWND parent, TCHAR* lbl) {
		int style = DEF_WND_STYLE | SS_LEFT;
#if !defined(UNDER_CE)
		style |= SS_CENTERIMAGE;
#endif
		hwnd = CreateWindow( TEXT("STATIC"), lbl, style, 0, 0, 0, 0,
			parent, (HMENU)0, NULL, NULL);
		defWndProc = (WNDPROC)SetWindowLong( hwnd, GWL_WNDPROC, (LONG)WndProc);
	}

	void KLabel::setJustify( int adjust) {
		int style = GetWindowLong( hwnd, GWL_STYLE);
		switch( adjust) {
		case 0:	//left
			style &= ~(SS_CENTER | SS_RIGHT);
			style |= SS_LEFT;
			break;
		case 1:	//center

			style &= ~(SS_LEFT | SS_RIGHT);

			style |= SS_CENTER;

			break;

		case 2:	//right
			style &= ~(SS_CENTER | SS_LEFT);
			style |= SS_RIGHT;
			break;
		}
		
		SetWindowLong( hwnd, GWL_STYLE, style);
	}
};



/*************************
*	exported functions
**************************/
extern "C" {
	
	void* __cdecl
		Java_java_awt_Toolkit_lblCreateLabel ( JNIEnv* env, jclass clazz, KWnd* parent, jstring label )
	{
		return ( new KLabel( parent->hwnd, java2WinString( env, X, label) ));
	}
	
	void __cdecl
		Java_java_awt_Toolkit_lblSetText ( JNIEnv* env, jclass clazz, KLabel* wnd, jstring label)
	{
		wnd->setText( label);
	}
	
	void __cdecl
		Java_java_awt_Toolkit_lblSetJustify ( JNIEnv* env, jclass clazz, KLabel* wnd, jint adjust )
	{
		wnd->setJustify( adjust);
	}
	
}
