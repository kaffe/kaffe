/**
* btn.cpp - 
*
* Copyright (c) 1999
*      Transvirtual Technologies, Inc.  All rights reserved.
*
* See the file "license.terms" for information on usage and redistribution 
* of this file. 
*/

#include "toolkit.hpp"

class KButton:public KWnd {
public:	
	bool blockCmd;
	
	KButton( HWND parent, TCHAR* lbl, int style) {
		style |= DEF_WND_STYLE;
		hwnd = CreateWindow( TEXT("BUTTON"), lbl, style, 0, 0, 0, 0,
			parent, (HMENU)0, NULL, NULL);
		defWndProc = (WNDPROC)SetWindowLong( hwnd, GWL_WNDPROC, (LONG)WndProc);
		blockCmd = false;
	}
	
	bool KButton::OnCommand( int cmd) {
		jstring aCmd;
		int style;
		
		switch ( cmd) {
		case BN_CLICKED:
			style = GetWindowLong( hwnd, GWL_STYLE);
			
			//Checkboxes have no AUTO style, leave state model and checking
			//to java
			
			//Grouped CheckBox
			if ( style & BS_RADIOBUTTON ) {
				// workaround for notifications caused by WM_SETFOCUS

				// ( tab stepping )
				if ( blockCmd ) {
					blockCmd = false;
					return false;
				}
				X->jEvt = JniEnv->CallStaticObjectMethod( ItemEvent, getItemEvent,
					srcIdx, getCheck() ? DESELECTED : SELECTED, 0 );
			}
			//Simple CheckBox
			else if ( style & BS_CHECKBOX ) {
				X->jEvt = JniEnv->CallStaticObjectMethod( ItemEvent, getItemEvent,
					srcIdx, getCheck() ? DESELECTED : SELECTED, 0 );
			}
			//PushButton
			else {
				GetWindowText( hwnd, X->buf, X->nBuf);
				aCmd = JniEnv->NewStringUTF( winString2UTF(X, X->buf, _tcslen(X->buf)) );
				X->jEvt = JniEnv->CallStaticObjectMethod( ActionEvent, getActionEvent,
					srcIdx, aCmd, 0 );
			}
			break;
		case BN_PUSHED:
			break;
		case BN_UNPUSHED:
			break;
		}
		return true;
	}
	
	bool KButton::OnFocusChange ( bool gain) {
		X->jEvt = JniEnv->CallStaticObjectMethod( FocusEvent, getFocusEvent,
			srcIdx, gain ? FOCUS_GAINED : FOCUS_LOST, false);
		// workaround for notifications caused by WM_SETFOCUS ( tab stepping )

		blockCmd = true;
		return false;
	}
	

	bool KButton::OnMouseUp( int btn, int x, int y) {
		KWnd::OnMouseUp( btn, x, y);

		// workaround for notifications caused by WM_SETFOCUS ( tab stepping )
		blockCmd = false;
		return false;

	}

	void KButton::setCheck( jboolean check) {
		int cs = check ? BST_CHECKED : BST_UNCHECKED;
		SendMessage( hwnd, BM_SETCHECK, (WPARAM)cs, 0);
	}
	
	bool KButton::getCheck() {
		int s = SendMessage( hwnd, BM_GETCHECK, 0, 0);
		return ( s == BST_CHECKED);
	}
	
	void KButton::setGrouped( jboolean grouped) {
		int style = GetWindowLong( hwnd, GWL_STYLE);
		if ( grouped) {
			style |= BS_RADIOBUTTON;
			style &= ~BS_CHECKBOX;
		}
		else {
			style |= BS_CHECKBOX;
			style &= ~BS_RADIOBUTTON;
		}
		SetWindowLong( hwnd, GWL_STYLE, style);
	}
};



/*************************
*	exported functions
**************************/
extern "C" {
	
	void* __cdecl
		Java_java_awt_Toolkit_btnCreatePushbutton ( JNIEnv* env, jclass clazz, KWnd* parent, jstring label )
	{
		return ( new KButton( parent->hwnd, java2WinString( env, X, label), BS_PUSHBUTTON) );
	}
	
	void* __cdecl
		Java_java_awt_Toolkit_btnCreateCheckbox ( JNIEnv* env, jclass clazz, KWnd* parent, jstring label )
	{
		return ( new KButton( parent->hwnd, java2WinString( env, X, label), BS_CHECKBOX) );
	}
	
	void* __cdecl
		Java_java_awt_Toolkit_btnCreateRadiobutton ( JNIEnv* env, jclass clazz, KWnd* parent, jstring label )
	{
		return ( new KButton( parent->hwnd, java2WinString( env, X, label), BS_RADIOBUTTON) );
	}
	
	void __cdecl
		Java_java_awt_Toolkit_btnSetLabel ( JNIEnv* env, jclass clazz, KButton* wnd, jstring label)
	{
		wnd->setText( label);
	}
	
	void __cdecl
		Java_java_awt_Toolkit_btnSetCheck ( JNIEnv* env, jclass clazz, KButton* wnd, jboolean check )
	{
		wnd->setCheck( check);
	}
	
	jboolean __cdecl
		Java_java_awt_Toolkit_btnGetCheck ( JNIEnv* env, jclass clazz, KButton* wnd )
	{
		return wnd->getCheck();
	}
	
	void __cdecl
		Java_java_awt_Toolkit_btnSetGrouped ( JNIEnv* env, jclass clazz, KButton* wnd, jboolean grouped )
	{
		wnd->setGrouped( grouped);
	}
}
