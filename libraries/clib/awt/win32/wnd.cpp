/**
* wnd.cpp - native toplevel window related functions
*
* Copyright (c) 1999
*      Transvirtual Technologies, Inc.  All rights reserved.
*
* See the file "license.terms" for information on usage and redistribution 
* of this file. 
*/

#include "toolkit.hpp"
#if defined(UNDER_CE)
#include <commctrl.h>
extern "C" HINSTANCE appInstance;
#endif

extern UINT KWM_DESTROY;

class KWindow:public KWnd {
public:
	KWindow( HWND parent, TCHAR* title, int style, int x, int y, int width, int height) {
		RECT rcl = { x, y, x+width, y+height };
		style |= WS_CLIPCHILDREN;
		AdjustWindowRectEx( &rcl, style, FALSE, 0);
		setInsets( x-rcl.left, y-rcl.top, rcl.right-x-width, rcl.bottom-y-height, true);
		hwnd = CreateWindow( TEXT("window"), title, style,
					x, y, width, height,
					parent, (HMENU)0, NULL, NULL);
		defWndProc = (WNDPROC)DefWindowProc;
		
#if defined(UNDER_CE)
		hbar = NULL;
#endif
	}
	
	void KWindow::setInsets( int xl, int yt, int xr,int yb, bool notify) {
		jclass frCls;
		
		jmethodID setDecoInsets;
		
		insets.left = xl;
		insets.right = xr;
		insets.bottom = yb;
		insets.top = yt;
		
		if ( notify) {
			frCls = JniEnv->FindClass( "java/awt/Frame");
			setDecoInsets = JniEnv->GetStaticMethodID( frCls, "setDecoInsets", "(IIIII)V");
			
			X->frameInsets.left = xl;
			X->frameInsets.right = xr;
			X->frameInsets.bottom = yb;
			X->frameInsets.top = yt;
			
			JniEnv->CallStaticVoidMethod( frCls, setDecoInsets, yt, xl, yb, xr, -1);
		}
	}
	
	
	bool KWindow::OnPaint( int x, int y, int width, int height){
		PAINTSTRUCT ps;
		HDC hdc;
				
		//map to frame extends
		width += insets.left + insets.right;
		height += insets.bottom + insets.top;
		
		hdc = BeginPaint( hwnd, &ps);

		if ( ps.fErase )
			FillRect( ps.hdc, &ps.rcPaint, brush);
		
		EndPaint( hwnd, &ps);
		
		X->jEvt = JniEnv->CallStaticObjectMethod( PaintEvent, getPaintEvent,
					srcIdx, PAINT, //ps.fErase ? UPDATE : PAINT,
					x, y, width, height );			
		
		return true;
	}
	
	bool KWindow::OnActivate( int flags, bool min, HWND handle ) {
		//handled by java		
		return false;
	}

	
	bool KWindow::OnCommand( int cmd) {
		HMENU hmenu = GetMenu( hwnd);
#if defined(UNDER_CE)
		if ( !hmenu && hbar )
			hmenu = CommandBar_GetMenu( hbar, 0);
#endif
		if ( hmenu )
			processMenuAction( hmenu, cmd);
		return false;
	}
	
	
	bool KWindow::OnPosChanged( int x, int y, int width, int height) {
		X->jEvt = JniEnv->CallStaticObjectMethod( ComponentEvent, getComponentEvent,
			srcIdx, COMPONENT_RESIZED,
			x, y, width, height) ;		
		return true;
	}
	
	
	bool KWindow::OnClose() {
		X->jEvt = JniEnv->CallStaticObjectMethod( WindowEvent, getWindowEvent,
			srcIdx, WINDOW_CLOSING);		
		return true;
	}
		
	bool KWindow::OnSetCursor() {
		SetCursor( cursor);				
		return TRUE;
	}
	
	bool KWindow::updateInsets() {
	/*
	* called by Menu assign to detect appropriate
	* client insets
		*/
		RECT rcl, rclc;
		
		GetClientRect( hwnd, &rclc);
		
		rcl.left = rclc.left;
		rcl.top  = rclc.top;
		rcl.right = rclc.right;
		rcl.bottom = rclc.bottom;

#if !defined(UNDER_CE)
		AdjustWindowRectEx( &rclc, GetWindowLong( hwnd, GWL_STYLE), (GetMenu( hwnd) != NULL), 0);
#else
		AdjustWindowRectEx( &rclc, GetWindowLong( hwnd, GWL_STYLE), 0, 0);
		if ( hbar )
			rclc.top += CommandBar_Height( hbar);
#endif

		setInsets( rcl.left-rclc.left,
				rcl.top-rclc.top,
				rclc.right-rcl.right,
				rclc.bottom-rcl.bottom, false );
		
		return TRUE;
	}
	
};


extern "C" {
	
	void __cdecl
		Java_java_awt_Toolkit_wndSetTitle ( JNIEnv* env, jclass clazz, KWindow* wnd, jstring jTitle )
	{
		wnd->setText( jTitle);
	}
	
	
	void __cdecl
		Java_java_awt_Toolkit_wndSetResizable ( JNIEnv* env, jclass clazz, KWindow* wnd, jboolean isResizable,
		int x, int y, int width, int height )
	{
		int style;
		
		DBG( awt_wnd, ("setResizable: %x, %d, %d,%d,%d,%d\n", wnd, isResizable, x,y,width,height));
		
		style = GetWindowLong( wnd->hwnd, GWL_STYLE);
		
		if ( isResizable) {
			style &= ~WS_BORDER;
			style |= WS_THICKFRAME;
		}
		else {
			style |= WS_BORDER;
			style &= ~WS_THICKFRAME;
		}
		
		SetWindowLong( wnd->hwnd, GWL_STYLE, style);
	}
	
	void* __cdecl
		Java_java_awt_Toolkit_wndCreateFrame ( JNIEnv* env, jclass clazz, jstring jTitle,
		jint x, jint y, jint width, jint height,
		jint jCursor, jint clrBack, jboolean isResizable )
	{		
		DBG( awt_wnd, ("createFrame( %p, %d,%d,%d,%d,..)\n", jTitle,x,y,width,height));
		
		KWindow* wnd;
		TCHAR* title;

#if !defined(D_LOOP_MODE)
		if ( !X->dspTid )
			X->dspTid = GetCurrentThreadId();
#endif

		title = java2WinString( env, X, jTitle);
		wnd = new KWindow( NULL, title, WS_OVERLAPPEDWINDOW, x, y, width, height);
		DBG( awt_wnd, (" -> %x\n", wnd->hwnd));
		
		if ( !isResizable ) {
			Java_java_awt_Toolkit_wndSetResizable( env, clazz,
				wnd, FALSE, x, y, width, height);			
		}
		
		return wnd;
		
	}
	
	
	void* __cdecl
		Java_java_awt_Toolkit_wndCreateWindow ( JNIEnv* env, jclass clazz, KWindow* owner,
		jint x, jint y, jint width, jint height,
		jint jCursor, jint clrBack )
	{
		KWindow* wnd;
		
		DBG( awt_wnd, ("createWindow( %p, %d,%d,%d,%d,..)\n", owner,x,y,width,height));
		
		wnd = new KWindow( owner ? owner->hwnd : NULL, TEXT(""), WS_POPUP, x, y, width, height);
		
		DBG( awt_wnd, (" -> %x\n", wnd->hwnd));
		
		return wnd;
	}
	
	void* __cdecl		
		Java_java_awt_Toolkit_wndCreateDialog ( JNIEnv* env, jclass clazz, KWindow* owner, jstring jTitle,
		jint x, jint y, jint width, jint height,
		jint jCursor, jint clrBack, jboolean isResizable )
	{
		DBG( awt_wnd, ("createDialog( %p,%p, %d,%d,%d,%d,..)\n", owner,jTitle,x,y,width,height));
		
		KWindow* wnd;
		TCHAR* title;
				
		title = java2WinString( env, X, jTitle);
		wnd = new KWindow( owner->hwnd, title, WS_OVERLAPPEDWINDOW, x, y, width, height);
		
		DBG( awt_wnd, (" -> %x\n", wnd->hwnd));
				
		if ( !isResizable ) {			
			Java_java_awt_Toolkit_wndSetResizable( env, clazz,				
				wnd, FALSE, x, y, width, height);						
		}
				
		return wnd;
		
	}
	
	void __cdecl
		Java_java_awt_Toolkit_wndDestroyWindow ( JNIEnv* env, jclass clazz, KWindow* wnd )
	{
		PostMessage( X->wakeUp, KWM_DESTROY, (WPARAM)wnd->hwnd, 0);
	}
	
	
	void __cdecl
		Java_java_awt_Toolkit_wndRequestFocus ( JNIEnv* env, jclass clazz, KWindow* wnd )
	{
		DBG( awt_wnd, ("request focus: %x\n", wnd));
		wnd->setFocus();
//		SetFocus( wnd->hwnd);
	}
	
	
	void __cdecl
		Java_java_awt_Toolkit_wndSetFrameInsets ( JNIEnv* env, jclass clazz,
		jint top, jint left, jint bottom, jint right )
	{
		X->frameInsets.top  = top;
		X->frameInsets.left = left;
		X->frameInsets.bottom = bottom;
		X->frameInsets.right = right;
		X->frameInsets.guess = 1;
	}
	
	void __cdecl
		Java_java_awt_Toolkit_wndSetDialogInsets ( JNIEnv* env, jclass clazz,
		jint top, jint left, jint bottom, jint right )
	{
		X->dialogInsets.top  = top;
		X->dialogInsets.left = left;
		X->dialogInsets.bottom = bottom;
		X->dialogInsets.right = right;
		X->dialogInsets.guess = 1;
	}
	
	void __cdecl
		Java_java_awt_Toolkit_wndSetBounds ( JNIEnv* env, jclass clazz, KWindow* wnd,
		jint x, jint y, jint width, jint height,
		jboolean isResizable )
	{		
		DBG( awt_wnd, ("setBounds: %x %d,%d,%d,%d\n", wnd, x, y, width, height));
				
		if ( width < 0 )  width = 1;		
		if ( height < 0 ) height = 1;
		
		SetWindowPos( wnd->hwnd, NULL, x, y, width, height, SWP_NOZORDER);
		
		if ( !isResizable ) {
			Java_java_awt_Toolkit_wndSetResizable( env, clazz,
				wnd, FALSE, x, y, width, height);
		}
		
	}
	
	
	
	void __cdecl
		Java_java_awt_Toolkit_wndRepaint ( JNIEnv* env, jclass clazz, KWindow* wnd,
		jint x, jint y, jint width, jint height )
	{
		DBG( awt_wnd, ("wndRepaint: %x %d,%d,%d,%d\n", wnd, x, y, width, height));
		
		RECT rcl;
		
		rcl.left = x;
		rcl.top = y;
		rcl.right = x + width;
		rcl.bottom = y + height;
		
		InvalidateRect( wnd->hwnd, &rcl, TRUE);
		UpdateWindow( wnd->hwnd);
	}
	
	
	
	void __cdecl
		Java_java_awt_Toolkit_wndSetIcon ( JNIEnv* env, jclass clazz, KWindow* wnd, void* img )
	{
	}
	
	
	void __cdecl
		Java_java_awt_Toolkit_wndSetVisible ( JNIEnv* env, jclass clazz, KWindow* wnd, jboolean showIt )
	{
		DBG( awt_wnd, ("setVisible: %x %d\n", wnd, showIt));
		wnd->setVisible( showIt);
	}
	
	void __cdecl
		Java_java_awt_Toolkit_wndToBack ( JNIEnv* env, jclass clazz, KWindow* wnd )
	{
		DBG( awt_wnd, ("toBack: %x\n", wnd));
		SetWindowPos( wnd->hwnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
	
	void __cdecl
		Java_java_awt_Toolkit_wndToFront ( JNIEnv* env, jclass clazz, KWindow* wnd )
	{
		DBG( awt_wnd, ("toFront: %x\n", wnd));
		SetWindowPos( wnd->hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
	
	void __cdecl
		Java_java_awt_Toolkit_wndSetCursor ( JNIEnv* env, jclass clazz, KWindow* wnd, jint jCursor )
	{
		DBG( awt_wnd, ("setCursor: %x, %d\n", wnd, jCursor));
		wnd->setCursor( getCursor( jCursor));
	}
	
}
