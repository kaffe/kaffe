/**
* filedlg.cpp - 
*
* Copyright (c) 1999
*      Transvirtual Technologies, Inc.  All rights reserved.
*
* See the file "license.terms" for information on usage and redistribution 
* of this file. 
*/

#include "toolkit.hpp"
#include "cmnwnd.hpp"
#include "commdlg.h"

extern "C" {
	UINT WINAPI FDlgProc ( HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam) {
		//	printf( "%x %x\n", hdlg, msg);
		switch ( msg) {
		case WM_INITDIALOG:
			//		printf( "handle is: %x\n", GetParent( hdlg) );
			break;
		}
		
		return 0;
	}
	
	
	static void init ( OPENFILENAME* pof )
	{
		pof->lStructSize = sizeof( OPENFILENAME);
		pof->hInstance = NULL;
		pof->lpstrFilter = TEXT("All Files (*.*)\0*.*\0\0");
		pof->lpstrCustomFilter = NULL;
		pof->nMaxCustFilter = 0;
		pof->nFilterIndex = 0;
		pof->nMaxFile = _MAX_PATH;
		pof->lpstrFileTitle = NULL;
		pof->nMaxFileTitle = _MAX_FNAME + _MAX_EXT;
		pof->lpstrInitialDir = NULL;
		pof->lpstrTitle = NULL;
		pof->Flags = 0;
		pof->nFileOffset = 0;
		pof->nFileExtension = 0;
		pof->lpstrDefExt = 0;
		pof->lCustData = NULL;
		pof->lpfnHook = (LPOFNHOOKPROC)FDlgProc;
		pof->lpTemplateName = NULL;
	}
	
	
	
	jstring
		Java_java_awt_Toolkit_fdlgLoad ( JNIEnv* env, jclass clazz, KWnd* owner, jstring title, jstring dir, jstring file, jstring filter )
	{
		OPENFILENAME ofn;
		TCHAR bd[ _MAX_PATH];
		TCHAR bf[ _MAX_FNAME];
		TCHAR be[ _MAX_EXT];
		
		init (&ofn);
		_tcscpy( bd, java2WinString( env, X, dir));
		_tcscpy( bf, java2WinString( env, X, file));
		_tcscpy( be, java2WinString( env, X, filter));
		
		ofn.hwndOwner = owner ? owner->hwnd : NULL;
		ofn.lpstrFile = bf;
		ofn.lpstrInitialDir = bd;
		ofn.lpstrTitle = title ? java2WinString( env, X, title) : NULL;
		//		ofn.lpstrFilter = be;
		ofn.Flags = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_ENABLEHOOK;
		
		if ( ! GetOpenFileName( &ofn) ) {
			return NULL;
		}
		
		return env->NewStringUTF( winString2UTF(X, ofn.lpstrFile, _tcslen(ofn.lpstrFile)));
	}
	
	jstring
		Java_java_awt_Toolkit_fdlgSave ( JNIEnv* env, jclass clazz, KWnd* owner, jstring title, jstring dir, jstring file, jstring filter )
	{
		OPENFILENAME ofn;
		TCHAR bd[ _MAX_PATH];
		TCHAR bf[ _MAX_FNAME];
		TCHAR be[ _MAX_EXT];
		
		init (&ofn);
		_tcscpy( bd, java2WinString( env, X, dir));
		_tcscpy( bf, java2WinString( env, X, file));
		_tcscpy( be, java2WinString( env, X, filter));
		
		ofn.hwndOwner = owner ? owner->hwnd : NULL;
		ofn.lpstrFile = bf;
		ofn.lpstrInitialDir = bd;
		ofn.lpstrFileTitle = title ? java2WinString( env, X, title) : NULL;
		//		ofn.lpstrFilter = be;
		ofn.Flags = OFN_OVERWRITEPROMPT;
		
		if ( ! GetSaveFileName( &ofn) ) {
			return NULL;
		}
		
		return env->NewStringUTF( winString2UTF(X, ofn.lpstrFile, _tcslen(ofn.lpstrFile)));
	}
}