/**
* edit.cpp - text edit classes
*
* Copyright (c) 1999
*      Transvirtual Technologies, Inc.  All rights reserved.
*
* See the file "license.terms" for information on usage and redistribution 
* of this file. 
*/

#include "toolkit.hpp"
#if !defined(UNDER_CE)
#include "richedit.h"
#endif

#define ESB_BOTH 0
#define ESB_VERT 1
#define ESB_HORZ 2
#define ESB_NONE 3


class KEdit:public KWnd {
public:		
	bool multi;

	KEdit( HWND parent, TCHAR* txt, int style) {
		multi = ( style & ES_MULTILINE ) ? true : false;
		style |= DEF_WND_STYLE;
		hwnd = CreateWindowEx( WS_EX_CLIENTEDGE, TEXT("EDIT"), txt, style, 0, 0, 0, 0,
			parent, (HMENU)0, NULL, NULL);
		defWndProc = (WNDPROC)SetWindowLong( hwnd, GWL_WNDPROC, (LONG)WndProc);
	}
	
	bool KEdit::OnChar( TCHAR c, int data) {
		jstring aCmd;

		if (multi == false) {
			switch (c) {
			case VK_RETURN:
				GetWindowText( hwnd, X->buf, X->nBuf);
				aCmd = JniEnv->NewStringUTF( winString2UTF(X, X->buf, _tcslen(X->buf)) );
				X->jEvt = JniEnv->CallStaticObjectMethod( ActionEvent, getActionEvent,
													srcIdx, aCmd, 0 );
				return true;
			case VK_TAB:
				//non printable char bell
				return true;
			}
		}
		else {
			switch (c) {
			case VK_TAB:
				//do not process tabbing keys ( shift / ctrl )
				if ( getKeyMods() ){
					return true;
				}
				break;
			}
		}

		return false;
	}

	bool KEdit::OnKeyDown( int virtKey, int data) {
		KWnd::OnKeyDown( virtKey, data);
		//do not process tabbing keys ( shift / ctrl )
		return ( virtKey == VK_TAB);
	}

	void KEdit::setEchoChar( jchar c) {
		SendMessage( hwnd, EM_SETPASSWORDCHAR, (WPARAM) c, 0);
	}
	
	void KEdit::setCaretPos( int idx) {
		select( idx, idx);
	}
	
	void KEdit::select( int start, int end) {
		SendMessage( hwnd, EM_SETSEL, (WPARAM)start, (LPARAM)end );
	}
	
	void KEdit::selectAll() {
		select( 0, -1);
	}
	
	void replaceSelection( TCHAR* txt) {
		SendMessage( hwnd, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)txt);
	}
};



/*************************
*	exported functions
**************************/
extern "C" {
	
	void* __cdecl
		Java_java_awt_Toolkit_editCreateField ( JNIEnv* env, jclass clazz, KWnd* parent, jstring txt )
	{
		return ( new KEdit( parent->hwnd, java2WinString( env, X, txt), ES_AUTOHSCROLL));
	}
	
	void* __cdecl
		Java_java_awt_Toolkit_editCreateArea ( JNIEnv* env, jclass clazz, KWnd* parent, jstring txt, jint style )
	{
		int ws = ES_MULTILINE | ES_AUTOVSCROLL;
		
		switch ( style ) {
		case ESB_VERT:
			ws |= WS_VSCROLL;
			break;
		case ESB_HORZ:
			ws |= WS_HSCROLL;
			break;
		case ESB_BOTH:
			ws |= WS_VSCROLL | WS_HSCROLL;
			break;
		}
		
		return ( new KEdit( parent->hwnd, java2WinString( env, X, txt), ws) );
	}
	
	void __cdecl
		Java_java_awt_Toolkit_editSetEditable ( JNIEnv* env, jclass clazz, KEdit* wnd, jboolean edit )
	{
		SendMessage( wnd->hwnd, EM_SETREADONLY, edit ? 0 : 1, 0);
	}
	
	jstring __cdecl
		Java_java_awt_Toolkit_editGetText ( JNIEnv* env, jclass clazz, KEdit* wnd)
	{
		TCHAR* str = wnd->getText();
		return (env->NewStringUTF( winString2UTF(X, str, _tcslen(str)) ));
	}
	
	void __cdecl
		Java_java_awt_Toolkit_editSetText ( JNIEnv* env, jclass clazz, KEdit* wnd, jstring txt )
	{
		SetWindowText( wnd->hwnd, java2WinString( env, X, txt) );
	}
	
	jstring __cdecl
		Java_java_awt_Toolkit_editGetSelectedText ( JNIEnv* env, jclass clazz, KEdit* wnd)
	{
#if !defined(UNDER_CE)
		jstring js;
		char* buf;
		int start, end;
		
		SendMessage( wnd->hwnd, EM_GETSEL, (WPARAM)&start, (LPARAM)&end);
		buf = ( char*)AWT_MALLOC( end-start);
		SendMessage( wnd->hwnd, EM_GETSELTEXT, 0, (LPARAM)buf);
		js = env->NewStringUTF( buf);
		AWT_FREE( buf);
		
		return js;
#else
		return (0);
#endif
	}
	
	jint __cdecl
		Java_java_awt_Toolkit_editGetSelectionStart ( JNIEnv* env, jclass clazz, KEdit* wnd)
	{
		int idx = -1;
		SendMessage( wnd->hwnd, EM_GETSEL, (WPARAM)&idx, NULL);
		return idx;
	}
	
	jint __cdecl
		Java_java_awt_Toolkit_editGetSelectionEnd ( JNIEnv* env, jclass clazz, KEdit* wnd)
	{
		int idx = -1;
		SendMessage( wnd->hwnd, EM_GETSEL, NULL, (LPARAM)&idx);
		return idx;
	}
	
	void __cdecl
		Java_java_awt_Toolkit_editSetSelectionStart ( JNIEnv* env, jclass clazz, KEdit* wnd, jint idx)
	{
		int eIdx;
		SendMessage( wnd->hwnd, EM_GETSEL, NULL, (LPARAM)&eIdx);
		wnd->select( idx, eIdx);
	}
	
	void __cdecl
		Java_java_awt_Toolkit_editSetSelectionEnd ( JNIEnv* env, jclass clazz, KEdit* wnd, jint idx )
	{
		int sIdx;
		SendMessage( wnd->hwnd, EM_GETSEL, (WPARAM)&sIdx, NULL);
		wnd->select( sIdx, idx);
	}
	
	void __cdecl
		Java_java_awt_Toolkit_editSelect ( JNIEnv* env, jclass clazz, KEdit* wnd, jstring txt, jint start, jint end )
	{
		wnd->select( start, end);
	}
	
	void __cdecl
		Java_java_awt_Toolkit_editSelectAll ( JNIEnv* env, jclass clazz, KEdit* wnd)
	{
		wnd->selectAll();
	}
	
	void __cdecl
		Java_java_awt_Toolkit_editSetCaretPosition ( JNIEnv* env, jclass clazz, KEdit* wnd,  jint idx)
	{
		wnd->setCaretPos( idx);
	}
	
	jint __cdecl
		Java_java_awt_Toolkit_editGetCaretPosition ( JNIEnv* env, jclass clazz, KEdit* wnd)
	{
		return -1;
	}
	
	void __cdecl
		Java_java_awt_Toolkit_editAppend ( JNIEnv* env, jclass clazz, KEdit* wnd, jstring txt )
	{
		TCHAR* buf = java2WinString( env, X, txt);
		wnd->setCaretPos( GetWindowTextLength( wnd->hwnd));
		wnd->replaceSelection( buf);
	}
	
	void __cdecl
		Java_java_awt_Toolkit_editInsert ( JNIEnv* env, jclass clazz, KEdit* wnd, jstring txt, jint idx )
	{
		TCHAR* buf = java2WinString( env, X, txt);
		wnd->setCaretPos( idx);
		wnd->replaceSelection( buf);
	}
	
	void __cdecl
		Java_java_awt_Toolkit_editReplaceRange ( JNIEnv* env, jclass clazz, KEdit* wnd, jstring txt, jint start, jint end )
	{
		TCHAR* buf = java2WinString( env, X, txt);
		wnd->select( start, end);
		wnd->replaceSelection( buf);
	}
	
	void __cdecl		
		Java_java_awt_Toolkit_editSetEchoChar ( JNIEnv* env, jclass clazz, KEdit* wnd, jchar c )		
	{
		wnd->setEchoChar( c);		
	}
	
}
