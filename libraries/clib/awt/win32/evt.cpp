/**
* evt.cpp - 
*
* Copyright (c) 1999
*      Transvirtual Technologies, Inc.  All rights reserved.
*
* See the file "license.terms" for information on usage and redistribution 
* of this file. 
*/


#include "toolkit.hpp"

UINT	KWM_WND_EVENT;
UINT	KWM_REQUEST_FOCUS;
UINT	KWM_CONTEXTMENU;
UINT	KWM_DESTROY;



/*******************************************************************************
*
*/

extern "C" {
	jobject __cdecl
		Java_java_awt_Toolkit_evtInit ( JNIEnv* env, jclass clazz )
	{
		jclass Component;
		
		ComponentEvent = env->FindClass( "java/awt/ComponentEvt");
		MouseEvent     = env->FindClass( "java/awt/MouseEvt");
		FocusEvent     = env->FindClass( "java/awt/FocusEvt");
		WindowEvent    = env->FindClass( "java/awt/WindowEvt");
		KeyEvent       = env->FindClass( "java/awt/KeyEvt");
		PaintEvent     = env->FindClass( "java/awt/PaintEvt");
		WMEvent        = env->FindClass( "java/awt/WMEvent");
		ItemEvent      = env->FindClass( "java/awt/ItemEvt");
		ActionEvent    = env->FindClass( "java/awt/ActionEvt");
		AdjustmentEvent= env->FindClass( "java/awt/AdjustmentEvt");
		
		getComponentEvent = env->GetStaticMethodID( ComponentEvent, "getEvent", 
			"(IIIIII)Ljava/awt/ComponentEvt;");
		getMouseEvent     = env->GetStaticMethodID( MouseEvent, "getEvent",
			"(IIIII)Ljava/awt/MouseEvt;");
		getFocusEvent     = env->GetStaticMethodID( FocusEvent, "getEvent",
			"(IIZ)Ljava/awt/FocusEvt;");
		getWindowEvent    = env->GetStaticMethodID( WindowEvent, "getEvent",
			"(II)Ljava/awt/WindowEvt;");
		getKeyEvent       = env->GetStaticMethodID( KeyEvent, "getEvent",
			"(IIIII)Ljava/awt/KeyEvt;");
		getPaintEvent     = env->GetStaticMethodID( PaintEvent, "getEvent",
			"(IIIIII)Ljava/awt/PaintEvt;");
		getWMEvent        = env->GetStaticMethodID( WMEvent, "getEvent",
			"(II)Ljava/awt/WMEvent;");
		getItemEvent      = env->GetStaticMethodID( ItemEvent, "getEvent",
			"(III)Ljava/awt/ItemEvt;");
		getActionEvent    = env->GetStaticMethodID( ActionEvent, "getEvent",
			"(ILjava/lang/String;I)Ljava/awt/ActionEvt;");
		getAdjustmentEvent= env->GetStaticMethodID( AdjustmentEvent, "getEvent",
			"(III)Ljava/awt/AdjustmentEvt;");
		
		dispatch = env->GetMethodID( WMEvent, "dispatch", "()V;" );
		
		KWM_WND_EVENT = RegisterWindowMessage( TEXT("KWM_WND_EVENT") );
		KWM_REQUEST_FOCUS = RegisterWindowMessage( TEXT("KWM_REQUEST_FOCUS") );
		KWM_CONTEXTMENU= RegisterWindowMessage( TEXT("KWM_CONTEXTMENU") );
		KWM_DESTROY = RegisterWindowMessage( TEXT("KWM_DESTROY") );
		
		X->nWindows = 199;
		X->windows = (KWnd**)AWT_CALLOC( X->nWindows, sizeof(KWnd*));
		
		Component = env->FindClass("java/awt/Component");
		return env->NewObjectArray( X->nWindows, Component, NULL);
	}
	
	
	jobject __cdecl
		Java_java_awt_Toolkit_evtGetNextEvent ( JNIEnv* env, jclass clazz )
	{
#ifndef D_LOOP_MODE
		MSG msg;
		
		UNBLOCK_EXECUTE( clazz,
			if ( GetMessage( &msg, NULL, 0, 0) == true) {
				TranslateMessage( &msg);
				X->jEvt = NULL;
				DispatchMessage( &msg);
			}
		);
		return X->jEvt;
#else
		return NULL;
#endif
	}
	
	jobject __cdecl
		Java_java_awt_Toolkit_evtPeekEvent ( JNIEnv* env, jclass clazz )
	{
		//just a dummy for the linker
		return NULL;
	}
	
	
	jobject __cdecl
		Java_java_awt_Toolkit_evtPeekEventId ( JNIEnv* env, jclass clazz, jint id )
	{
		//just a dummy for the linker
		return NULL;
	}
	
	void __cdecl
		Java_java_awt_Toolkit_evtSendWMEvent ( JNIEnv* env, jclass clazz, jobject evt )
	{
		// force context change to dispatcher thread
#ifdef D_LOOP_MODE
		PostMessage( X->wakeUp, KWM_WND_EVENT, (WPARAM)evt, 0);
#endif
	}
	
	void __cdecl
		Java_java_awt_Toolkit_evtWakeup ( JNIEnv* env, jclass clazz )
	{
#ifndef D_LOOP_MODE
		if ( X->dspTid )
			PostThreadMessage( X->dspTid, WM_NULL, 0, 0);
#endif
	}
	
	jint __cdecl
		Java_java_awt_Toolkit_evtRegisterSource ( JNIEnv* env, jclass clazz, KWnd* wnd )
	{
		register int i;
		int      n;
		
		/*
		* we don't use a double hashing here because collisions are very unlikely
		* (window IDs usually already are hashed, so it does not make sense to
		* hash them again - we just could make it worse
		*/
		for ( i = (unsigned long)wnd, n=0; n < X->nWindows; i++, n++ ) {
			i %= X->nWindows;
			if ( (int)X->windows[i] <= 0 ) {
				X->windows[i] = wnd;
				X->windows[i]->srcIdx = i;
				setWnd( X->windows[i]->hwnd, wnd);
				return i;
			}
		}
		
		kprintf( "window table out of space: %d", X->nWindows);
		return -1;
	}
	
	jint __cdecl
		Java_java_awt_Toolkit_evtUnregisterSource ( JNIEnv* env, jclass clazz, KWnd* wnd )
	{
		int i = wnd->srcIdx;

		/*
		* native destruction has to be async by creator thread.
		* window objects and childs are deleted by wndDestroyWindow
		* WM_DESTROY event processing
		*/
		
		if ( i >= 0 ) {
			X->windows[i] = (KWnd*)-1;
			return i;
		}
		else {
			return -1;
		}
	}
	
	
	LRESULT WINAPI WndProc( HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
		LPWINDOWPOS pos;
		RECT rcl;
		KWnd *wnd, *cwnd;
		HBRUSH brush;
		LRESULT ret;
		
		DBG(awt_evt, ("hwnd:%x message:%x wparam:%x laram:%x \n", hwnd, iMsg, wParam, lParam));
//		printf("hwnd:%x message:%x wparam:%x laram:%x \n", hwnd, iMsg, wParam, lParam);
		
		/*
		* KWM events redirected to create thread
		*/
		//<-X->wakeUp
		if ( iMsg == KWM_WND_EVENT ) {
			JniEnv->CallVoidMethod( (jobject)wParam, dispatch);
			return 0;
		}
		//<-cmnSetFocus
		if ( iMsg == KWM_REQUEST_FOCUS ) {
			SetFocus( (HWND)wParam);
			return 0;
		}
		//<-cmn/wndDestroyWindow
		if ( iMsg == KWM_DESTROY ) {
			DestroyWindow( (HWND)wParam);
			return 0;
		}
		//<-menuAssign
		if ( iMsg == KWM_CONTEXTMENU ) {
			if ( (wnd = getWnd( hwnd)) )
				wnd->OnContextMenu( LOWORD(lParam), HIWORD(lParam) );
			return 0;
		}
		
		if ( ! (wnd = getWnd( hwnd)) ) {
			if ( IsWindow( hwnd) ) {
				return DefWindowProc( hwnd, iMsg, wParam, lParam);
			}
			else {
				return 0;
			}
		}
		
		switch( iMsg) {
			
		// expose
		case WM_DRAWITEM:
			cwnd = (KWnd*)GetWindowLong( GetDlgItem( hwnd, (int)wParam), GWL_USERDATA);
			if ( cwnd->OnOwnerDraw( (LPDRAWITEMSTRUCT) lParam) ) {
				return 0;
			}
			break;
			
			// colors
		case WM_CTLCOLOREDIT:
		case WM_CTLCOLORBTN:
		case WM_CTLCOLORLISTBOX:
		case WM_CTLCOLORSCROLLBAR:
		case WM_CTLCOLORSTATIC:
			cwnd = (KWnd*)GetWindowLong( (HWND)lParam, GWL_USERDATA);
			if ( (cwnd) && (brush = cwnd->OnCtlColor( (HDC)wParam)) ) {
				return (LRESULT)brush;
			}
			break;
			
			// painting
		case WM_ERASEBKGND:
			if ( wnd->OnEraseBkgnd( (HDC)wParam) ) {
				return (LRESULT)1;
			}
			break;
			
		case WM_PAINT:
			if ( GetUpdateRect( hwnd, &rcl, false)) {
				if ( wnd->OnPaint( rcl.left, rcl.top, rcl.right-rcl.left, rcl.bottom-rcl.top) ) {
					return 0;
				}
			}
			break;
			
			// keyboard
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
			if ( wnd->OnKeyDown( (int)wParam, (int)lParam) )
				return 0;
			break;
		case WM_SYSKEYUP:
		case WM_KEYUP:
			if ( wnd->OnKeyUp( (int)wParam, (int)lParam) )
				return 0;
			break;
		case WM_CHAR:
			if ( wnd->OnChar( (TCHAR)wParam, (int)lParam) )
				return 0;
			break;
			
			// notifications
		case WM_COMMAND:
			//menu notification
			if ( lParam == 0) {
				if ( wnd->OnCommand( LOWORD( wParam) ))
					return 0;
			}
			else {
				cwnd = (KWnd*)GetWindowLong( (HWND)lParam, GWL_USERDATA);
				if ( cwnd && cwnd->OnCommand( HIWORD( wParam) ))
					return 0;
			}
			break;
		case WM_HSCROLL:
			cwnd = (KWnd*)GetWindowLong( (HWND)lParam, GWL_USERDATA);
			if ( cwnd && cwnd->OnScroll( HIWORD( wParam), LOWORD( wParam), false ))
				return 0;
			break;
		case WM_VSCROLL:
			cwnd = (KWnd*)GetWindowLong( (HWND)lParam, GWL_USERDATA);
			if ( cwnd && cwnd->OnScroll( HIWORD( wParam), LOWORD( wParam), true ))
				return 0;
			break;
			
			// sizeing
		case WM_WINDOWPOSCHANGED:
			pos = (LPWINDOWPOS) lParam;
			if ( wnd->OnPosChanged( pos->x, pos->y, pos->cx, pos->cy) ) {
				return 0;
			}
			break;
			
			// mouse motion
		case WM_MOUSEMOVE:
			if ( wnd->OnMouseMove( LOWORD( lParam), HIWORD( lParam)) ) {
				return 0;
			}
			break;
			
			// mouse
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
			if ( wnd->OnMouseDown( (int)wParam,LOWORD( lParam), HIWORD( lParam) ) ) {
				return 0;
			}
			break;
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
			if ( wnd->OnMouseUp( (int)wParam,LOWORD( lParam), HIWORD( lParam) ) ) {
				return 0;
			}
			break;
			
#if !defined(UNDER_CE)
		case WM_SETCURSOR:
		/*
		* do not set when entering menu mode or hittest
		* does not equal client
			*/
			if ( (HIWORD( lParam) != 0) && (LOWORD( lParam) == 1) ) {
				cwnd = (KWnd*)GetWindowLong( (HWND)wParam, GWL_USERDATA);
				if ( cwnd && cwnd->OnSetCursor() )
					return (LRESULT) 1;
			}
			break;
#endif
			// focus
		case WM_SETFOCUS:
			if ( wnd->OnFocusChange( true) ) {
				return 0;
			}
			break;
		case WM_KILLFOCUS:
			if ( wnd->OnFocusChange( false) ) {
				return 0;
			}
			break;
		case WM_ACTIVATE:
			if ( wnd->OnActivate( LOWORD( wParam), (bool)HIWORD( wParam), (HWND) lParam) ) {
				return 0;
			}
			break;

		case WM_CLOSE:
			if ( wnd->OnClose() ) {
				return 0;
			}
			break;
			
		case WM_CREATE:
			break;
			
		case WM_DESTROY:
			// unlink java objects
			SetWindowLong( wnd->hwnd, GWL_USERDATA, 0);
			ret = CallWindowProc( wnd->defWndProc, hwnd, iMsg, wParam, lParam);
			
			// cleanup
			X->lastHwnd = (HWND)-1;
			delete( wnd);
			
			return ret;
			
		default:
			break;
		}
		
		return CallWindowProc( wnd->defWndProc, hwnd, iMsg, wParam, lParam);
	}
	
}
