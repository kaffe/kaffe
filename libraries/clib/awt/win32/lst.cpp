/**
* lst.cpp - list classes
*
* Copyright (c) 1999
*      Transvirtual Technologies, Inc.  All rights reserved.
*
* See the file "license.terms" for information on usage and redistribution 
* of this file. 
*/

#include "toolkit.hpp"
#include "cmnwnd.hpp"

class KList:public KWnd {
public:		
	jboolean freezed;
	jboolean multiSel;
	
	KList( HWND parent, jboolean multi) {
		int style = multi ? LBS_MULTIPLESEL : 0;
		style |= DEF_WND_STYLE | WS_VSCROLL | WS_HSCROLL | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT;
		hwnd = CreateWindowEx( WS_EX_CLIENTEDGE, TEXT("LISTBOX"), NULL, style, 0, 0, 0, 0,
			parent, (HMENU)0, NULL, NULL);
		defWndProc = (WNDPROC)SetWindowLong( hwnd, GWL_WNDPROC, (LONG)WndProc);
		freezed = false;
		multiSel = multi;
	}
	
	bool KList::OnCommand( int cmd) {
		jstring aCmd;
		int idx, cmdId;
		TCHAR* str;
		
		switch ( cmd) {
		case LBN_SELCHANGE:
			//multiple mode gets focused item
			idx = getSelectionIdx();
			if (  multiSel && ! isSelected( idx)) {
				cmdId = DESELECTED;
			}
			else {
				cmdId = SELECTED;
			}
			JniEnv->CallStaticObjectMethod( ItemEvent, getItemEvent,
				srcIdx, cmdId, idx );
			break;
		case LBN_DBLCLK:
			str = getSelectionString();
			aCmd = JniEnv->NewStringUTF( winString2UTF(X, str, _tcslen(str)) );
			JniEnv->CallStaticObjectMethod( ActionEvent, getActionEvent,
				srcIdx, aCmd, 0 );
			break;
		case LBN_SELCANCEL:
			break;
		case LBN_SETFOCUS:
			break;
		case LBN_KILLFOCUS:
			break;
		case LBN_ERRSPACE:
			break;
		}
		
		return TRUE;
	}
	
	TCHAR* KList::getSelectionString() {
		int idx = SendMessage( hwnd, LB_GETCURSEL, 0, 0);
		if ( idx != LB_ERR ) {
			SendMessage( hwnd, LB_GETTEXT, (WPARAM)idx, (LPARAM)X->buf);
			return X->buf;
		}
		else {
			return (TCHAR*)0;
		}
	}
	
	bool KList::isSelected( int idx) {
		return (SendMessage( hwnd, LB_GETSEL, (WPARAM)idx, 0) > 0);
	}
	
	int KList::getSelectionIdx() {
		//for single selection lists
		return (int)SendMessage( hwnd, LB_GETCURSEL, 0, 0);
	}
	
	jobject KList::getSelectionIdxs() {
		//for multiple-selection lists
		jboolean	isCopy;
		jintArray	selArray;
		jint*		sels;
		int			numSel = (int)SendMessage( hwnd, LB_GETSELCOUNT, 0, 0);
		
		if ( numSel <= 0 )
			return NULL;
		
		selArray  = JniEnv->NewIntArray( numSel);
		sels = JniEnv->GetIntArrayElements( selArray, &isCopy);
		
		SendMessage( hwnd, LB_GETSELITEMS, (WPARAM)numSel, (LPARAM)sels);
		
		if ( isCopy ) {
			JniEnv->ReleaseIntArrayElements( selArray, sels, JNI_COMMIT);
		}
		
		return selArray;
	}
	
	bool KList::updateHScroll() {
		
		HDC hdc;
		SIZE sz;
		int idx, len, maxLen;
		
		if ( freezed ) {
			return FALSE;
		}
		
		maxLen = 0;
		hdc = GetDC( hwnd);
		
		for ( idx=0;;idx++) {
			len = (int)SendMessage( hwnd, LB_GETTEXT, (WPARAM)idx, (LPARAM)X->buf);
			if ( len == LB_ERR )
				break;
			GetTextExtentPoint32( hdc, X->buf, len, &sz);
			if ( sz.cx > maxLen ) {
				maxLen = sz.cx;
			}
		}
		
		ReleaseDC( hwnd, hdc);
		
		maxLen -= GetSystemMetrics( SM_CXVSCROLL);
		
		SendMessage( hwnd, LB_SETHORIZONTALEXTENT, (WPARAM)maxLen, 0);
		return TRUE;
	}
	};
	
	
	
	/*************************
	*	exported functions
	**************************/
	extern "C" {
		
		void* __cdecl
			Java_java_awt_Toolkit_lstCreateList ( JNIEnv* env, jclass clazz, KWnd* parent, jboolean multi)
		{
			return ( new KList( parent->hwnd, multi) );
		}
		
		void __cdecl
			Java_java_awt_Toolkit_lstAppendItem ( JNIEnv* env, jclass clazz, KList* wnd, jstring item)
		{
			TCHAR* buf = java2WinString( env, X, item);
			SendMessage( wnd->hwnd, LB_ADDSTRING, 0, (LPARAM)buf);
			wnd->updateHScroll();
		}
		
		jint __cdecl
			Java_java_awt_Toolkit_lstGetVisibleIndex ( JNIEnv* env, jclass clazz, KList* wnd)
		{
			return (jint)SendMessage( wnd->hwnd, LB_GETTOPINDEX, 0, 0);
		}
		
		jobject __cdecl
			Java_java_awt_Toolkit_lstGetSelectionIdxs ( JNIEnv* env, jclass clazz, KList* wnd)
		{
			return wnd->getSelectionIdxs();
		}
		
		void __cdecl
			Java_java_awt_Toolkit_lstInsertItem ( JNIEnv* env, jclass clazz, KList* wnd, jstring item, jint idx)
		{
			TCHAR* buf = java2WinString( env, X, item);
			SendMessage( wnd->hwnd, LB_INSERTSTRING, idx, (LPARAM)buf);
			wnd->updateHScroll();
		}
		
		void __cdecl
			Java_java_awt_Toolkit_lstFreeze ( JNIEnv* env, jclass clazz, KList* wnd)
		{
			SendMessage( wnd->hwnd, WM_SETREDRAW, (WPARAM) FALSE, 0);
			wnd->freezed = TRUE;
		}
		
		void __cdecl
			Java_java_awt_Toolkit_lstThaw ( JNIEnv* env, jclass clazz, KList* wnd)
		{
			SendMessage( wnd->hwnd, WM_SETREDRAW, (WPARAM) TRUE, 0);
			wnd->freezed = FALSE;
			wnd->updateHScroll();
		}
		
		void __cdecl
			Java_java_awt_Toolkit_lstUnselectItem ( JNIEnv* env, jclass clazz, KList* wnd, jint idx)
		{
			if ( wnd->multiSel ) {
				SendMessage( wnd->hwnd, LB_SETSEL, (WPARAM)false, (LPARAM)idx);
			}
			else if ( wnd->getSelectionIdx() == idx ) {
				//remove current selection
				SendMessage( wnd->hwnd, LB_SETCURSEL, (WPARAM)-1, 0);
			}
		}
		
		void __cdecl
			Java_java_awt_Toolkit_lstSelectItem ( JNIEnv* env, jclass clazz, KList* wnd, jint idx)
		{
			if ( wnd->multiSel ) {
				SendMessage( wnd->hwnd, LB_SETSEL, (WPARAM)true, (LPARAM)idx);
			}
			else {
				SendMessage( wnd->hwnd, LB_SETCURSEL, (WPARAM)idx, 0);
			}
		}
		
		void __cdecl
			Java_java_awt_Toolkit_lstRemoveAll ( JNIEnv* env, jclass clazz, KList* wnd)
		{
			SendMessage( wnd->hwnd, LB_RESETCONTENT, 0, 0);
			wnd->updateHScroll();
		}
		
		void __cdecl
			Java_java_awt_Toolkit_lstRemoveItem ( JNIEnv* env, jclass clazz, KList* wnd, jint idx)
		{
			SendMessage( wnd->hwnd, LB_DELETESTRING, (WPARAM)idx, 0);
			wnd->updateHScroll();
		}
		
		void __cdecl
			Java_java_awt_Toolkit_lstReplaceItem ( JNIEnv* env, jclass clazz, KList* wnd, jstring item, jint idx)
		{
			TCHAR* buf = java2WinString( env, X, item);
			SendMessage( wnd->hwnd, LB_DELETESTRING, (WPARAM)idx, 0);
			SendMessage( wnd->hwnd, LB_INSERTSTRING, idx, (LPARAM)buf);
			wnd->updateHScroll();
		}
		
		void __cdecl
			Java_java_awt_Toolkit_lstSetMultipleMode ( JNIEnv* env, jclass clazz, KList* wnd, jboolean mode)
		{
			int cs = GetWindowLong( wnd->hwnd, GWL_STYLE);
			
			wnd->multiSel = mode;
			if ( mode ) cs |= LBS_MULTIPLESEL;
			else		cs &= ~LBS_MULTIPLESEL;
			
			SetWindowLong( wnd->hwnd, GWL_STYLE, cs);
		}
		
		
}
