/**
* fnt.cpp - 
*
* Copyright (c) 1999
*      Transvirtual Technologies, Inc.  All rights reserved.
*
* See the file "license.terms" for information on usage and redistribution 
* of this file. 
*/


#include <string.h>
#include "toolkit.hpp"

/*******************************************************************************
* Font support
*/
extern "C" {
	
	void*
		Java_java_awt_Toolkit_fntInitFont ( JNIEnv* env, jclass clazz, jstring jSpec, jint style, jint size )
	{
		LOGFONT fnt;
		HFONT	fh;
		Font    *pFnt;
		TCHAR    *spec = java2WinString( env, X, jSpec);
		
		_tcscpy( fnt.lfFaceName, spec);
		
		fnt.lfHeight = -size;
		fnt.lfWidth = 0; 
		fnt.lfEscapement = 0; 
		fnt.lfOrientation = 0; 
		fnt.lfWeight = ( style & 1) ? FW_BOLD : FW_NORMAL; 
		fnt.lfItalic = ( style & 2) ? 1 : 0; 
		fnt.lfUnderline = 0; 
		fnt.lfStrikeOut = 0; 
		fnt.lfCharSet = 0; //ANSI_CHARSET; 
		fnt.lfOutPrecision = 0; //OUT_TT_PRECIS; 
		fnt.lfClipPrecision = 0; 
		fnt.lfQuality = 0; 
		fnt.lfPitchAndFamily = 0; 
		
		fh =  CreateFontIndirect( &fnt);
		
		pFnt = new Font();
		pFnt->fnt = fh;
		
		return pFnt;
	}
	
	void
		Java_java_awt_Toolkit_fntFreeFont ( JNIEnv* env, jclass clazz, Font* pFont )
	{
		DeleteObject( pFont->fnt);
		delete pFont;
	}
	
	/*******************************************************************************
	* FontMetrics support
	*/
	
	void*
		Java_java_awt_Toolkit_fntInitFontMetrics ( JNIEnv* env, jclass clazz, Font* pFont )
	{
		SelectObject( X->display, pFont->fnt);
		GetTextMetrics( X->display, &(pFont->fm));
		
		return pFont;
	}
	
	void
		Java_java_awt_Toolkit_fntFreeFontMetrics ( JNIEnv* env, jclass clazz, Font* pFont )
	{
	}
	
	
	jint
		Java_java_awt_Toolkit_fntGetAscent ( JNIEnv* env, jclass clazz, Font* pFont )
	{
		return pFont->fm.tmAscent;
	}
	
	jint
		Java_java_awt_Toolkit_fntGetDescent ( JNIEnv* env, jclass clazz, Font* pFont )
	{
		return pFont->fm.tmDescent;
	}
	
	jint
		Java_java_awt_Toolkit_fntGetFixedWidth ( JNIEnv* env, jclass clazz, Font* pFont )
	{
		//		return pFont->fm.tmMaxCharWidth;
		
		return 0;
	}
	
	jint
		Java_java_awt_Toolkit_fntGetHeight ( JNIEnv* env, jclass clazz, Font* pFont )
	{
		return pFont->fm.tmHeight;
	}
	
	jint
		Java_java_awt_Toolkit_fntGetLeading ( JNIEnv* env, jclass clazz, Font* pFont )
	{
		return pFont->fm.tmInternalLeading;
	}
	
	jint
		Java_java_awt_Toolkit_fntGetMaxAdvance ( JNIEnv* env, jclass clazz, Font* pFont )
	{
		return pFont->fm.tmOverhang;
	}
	
	jint
		Java_java_awt_Toolkit_fntGetMaxAscent ( JNIEnv* env, jclass clazz, Font* pFont )
	{
		return pFont->fm.tmAscent;
	}
	
	jint
		Java_java_awt_Toolkit_fntGetMaxDescent ( JNIEnv* env, jclass clazz, Font* pFont )
	{
		return pFont->fm.tmDescent;
	}
	
	jboolean
		Java_java_awt_Toolkit_fntIsWideFont ( JNIEnv* env, jclass clazz, Font* pFont )
	{
		//		return ((pFont->fm.tmPitchAndFamily & 0xf0) == TMPF_VECTOR);
		
		return FALSE;
	}
	
	jobject
		Java_java_awt_Toolkit_fntGetWidths ( JNIEnv* env, jclass clazz, Font* pFont )
	{
		jintArray widths;
		jint      *jw;
		jboolean isCopy;
		
		widths = env->NewIntArray( 256);
		jw = env->GetIntArrayElements( widths, &isCopy);
		
		SelectObject( X->display, pFont->fnt);
		
		//GetCharWidth32 not supported by Win95
		//		GetCharWidth( X->display, pFont->fm.tmFirstChar, pFont->fm.tmLastChar, jw);
#if !defined(UNDER_CE)
		GetCharWidth( X->display, 0, 255, jw);
#endif
		
		
		env->ReleaseIntArrayElements( widths, jw, 0);
		
		return widths;
	}
	
	
	jint
		Java_java_awt_Toolkit_fntBytesWidth ( JNIEnv* env, jclass clazz, Font* pFont,
		jbyteArray jBytes, jint off, jint len )
	{
		SIZE sz;
		jboolean  isCopy;
		jbyte    *jb = env->GetByteArrayElements( jBytes, &isCopy);
		int       n = env->GetArrayLength( jBytes);
		TCHAR	 *cs;
		
		if ( off+len > n ) len = n - off;
		
		cs = jchar2WinString( X, (jchar*)jb+off, len);
		SelectObject( X->display, pFont->fnt);
		GetTextExtentExPoint( X->display, cs, len, 0, NULL, NULL, &sz);
		
		env->ReleaseByteArrayElements( jBytes, jb, JNI_ABORT);
		return sz.cx;
	}
	
	jint
		Java_java_awt_Toolkit_fntCharWidth ( JNIEnv* env, jclass clazz, Font* pFont, jchar jChar )
	{
		SIZE sz;
		SelectObject( X->display, pFont->fnt);
		GetTextExtentExPoint( X->display, (TCHAR*)&jChar, 1, 0, NULL, NULL, &sz);
		return sz.cx;
	}
	
	jint
		Java_java_awt_Toolkit_fntCharsWidth ( JNIEnv* env, jclass clazz, Font* pFont,
		jcharArray jChars, jint off, jint len )
	{
		SIZE      sz;
		jboolean  isCopy;
		jchar    *jc = env->GetCharArrayElements( jChars, &isCopy);
		int      n = env->GetArrayLength( jChars);
		TCHAR	 *cs;
		
		if ( off+len > n ) len = n - off;
		
		cs = jchar2WinString( X, jc+off, len);
		SelectObject( X->display, pFont->fnt);
		GetTextExtentExPoint( X->display, cs, len, 0, NULL, NULL, &sz);
		
		env->ReleaseCharArrayElements( jChars, jc, JNI_ABORT);
		return sz.cx;
	}
	
	jint
		Java_java_awt_Toolkit_fntStringWidth ( JNIEnv* env, jclass clazz, Font* pFont, jstring jStr )
	{
		SIZE sz;
		TCHAR *jc = java2WinString( env, X, jStr);
		
		SelectObject( X->display, pFont->fnt);
		GetTextExtentExPoint( X->display, jc, _tcslen(jc), 0, NULL, NULL, &sz);
		
		return sz.cx;
	}
	
}
