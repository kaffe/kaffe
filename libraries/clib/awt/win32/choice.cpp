/**
* choice.cpp - 
*
* Copyright (c) 1999
*      Transvirtual Technologies, Inc.  All rights reserved.
*
* See the file "license.terms" for information on usage and redistribution 
* of this file. 
*/

#include "toolkit.hpp"


class KChoice:public KWnd {
public:		
	KChoice( HWND parent) {
		int style = DEF_WND_STYLE | WS_VSCROLL | CBS_DROPDOWNLIST | CBS_AUTOHSCROLL;
		hwnd = CreateWindowEx( WS_EX_CLIENTEDGE, TEXT("COMBOBOX"), NULL, style, 0, 0, 0, 0,
			parent, (HMENU)0, NULL, NULL);
		defWndProc = (WNDPROC)SetWindowLong( hwnd, GWL_WNDPROC, (LONG)WndProc);
	}
	
	bool KChoice::OnCommand( int cmd) {
		int idx;
		
		switch ( cmd) {
		case CBN_CLOSEUP:
			break;
		case CBN_DROPDOWN:
			break;
		case CBN_EDITCHANGE:
			break;
		case CBN_EDITUPDATE:
			break;
		case CBN_SELCHANGE:
			break;
		case CBN_SELENDCANCEL:
			break;
		case CBN_SELENDOK:
			idx = getSelectionIdx();
			X->jEvt = JniEnv->CallStaticObjectMethod( ItemEvent, getItemEvent,
				srcIdx, SELECTED, idx );
			break;
		case CBN_DBLCLK:
			break;
		case CBN_SETFOCUS:
			break;
		case CBN_KILLFOCUS:
			break;
		case CBN_ERRSPACE:
			break;
		}
		
		return true;
	}
	
	jobject KChoice::getPreferredSize() {
		jclass dc = JniEnv->FindClass( "java/awt/Dimension");
		jmethodID ct = JniEnv->GetMethodID( dc, "<init>", "(II)V;");
		int height = SendMessage( hwnd, CB_GETITEMHEIGHT, (WPARAM)-1, 0);
		return (JniEnv->NewObject( dc, ct, 100, height));
	}
	
	void KChoice::setBounds(int x, int y, int width, int height) {
		//extends include pull down list
		int ih = (int)SendMessage( hwnd, CB_GETITEMHEIGHT, 0, 0);
		KWnd::setBounds( x, y, width, height + 8*ih);
		//			SendMessage( hwnd, CB_SETITEMHEIGHT, (WPARAM)-1, (LPARAM)height);
	}
	
	int KChoice::getSelectionIdx() {
		return (int)SendMessage( hwnd, CB_GETCURSEL, 0, 0);
	}
	
};



/*************************
*	exported functions
**************************/
extern "C" {
	
	void* __cdecl
		Java_java_awt_Toolkit_choiceCreateChoice ( JNIEnv* env, jclass clazz, KWnd* parent)
	{
		return ( new KChoice( parent->hwnd));
	}
	
	void __cdecl
		Java_java_awt_Toolkit_choiceAppendItem ( JNIEnv* env, jclass clazz, KChoice* wnd, jstring item)
	{
		TCHAR* buf = java2WinString( env, X, item);
		SendMessage( wnd->hwnd, CB_ADDSTRING, 0, (LPARAM)buf);
	}
	
	void __cdecl
		Java_java_awt_Toolkit_choiceInsertItem ( JNIEnv* env, jclass clazz, KChoice* wnd, jstring item, jint idx)
	{
		TCHAR* buf = java2WinString( env, X, item);
		SendMessage( wnd->hwnd, CB_INSERTSTRING, idx, (LPARAM)buf);
	}
	
	void __cdecl
		Java_java_awt_Toolkit_choiceSelectItem ( JNIEnv* env, jclass clazz, KChoice* wnd, jint idx)
	{
		SendMessage( wnd->hwnd, CB_SETCURSEL, (WPARAM)idx, 0);
	}
	
	void __cdecl
		Java_java_awt_Toolkit_choiceRemoveAll ( JNIEnv* env, jclass clazz, KChoice* wnd)
	{
		SendMessage( wnd->hwnd, CB_RESETCONTENT, 0, 0);
	}
	
	void __cdecl
		Java_java_awt_Toolkit_choiceRemoveItem ( JNIEnv* env, jclass clazz, KChoice* wnd, jint idx)
	{
		SendMessage( wnd->hwnd, CB_DELETESTRING, (WPARAM)idx, 0);
	}
	
	
}
