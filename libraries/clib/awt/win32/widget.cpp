/**
 * widget.cpp - generic widget class used for canvas and panel
 *
 * Copyright (c) 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "toolkit.hpp"

	class KWidget:public KWnd {
	public:		
		KWidget( HWND parent) {
			int style = WS_CHILD | WS_VISIBLE;
			hwnd = CreateWindow( TEXT("widget"), NULL, style, 0, 0, 0, 0,
				parent, (HMENU)0, NULL, NULL);
			defWndProc = (WNDPROC)DefWindowProc;
		}

	bool KWidget::OnPaint( int x, int y, int width, int height){
		PAINTSTRUCT ps;
		HDC hdc;

		hdc = BeginPaint( hwnd, &ps);

		if ( ps.fErase )
			FillRect( ps.hdc, &ps.rcPaint, brush);

		EndPaint( hwnd, &ps);

		X->jEvt = JniEnv->CallStaticObjectMethod( PaintEvent, getPaintEvent,
							srcIdx, PAINT, x, y, width, height );			

		return true;
	}

	};


	
	/*************************
	*	exported functions
	**************************/
extern "C" {

	void* __cdecl
		Java_java_awt_Toolkit_widgetCreateWidget ( JNIEnv* env, jclass clazz, KWnd* parent)
	{
		return ( new KWidget( parent->hwnd));
	}

}
