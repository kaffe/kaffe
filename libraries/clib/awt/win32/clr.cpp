/**
* clr.cpp - color management
*
* Copyright (c) 1999
*      Transvirtual Technologies, Inc.  All rights reserved.
*
* See the file "license.terms" for information on usage and redistribution 
* of this file. 
*/

#include "toolkit.hpp"
/********************************************************************************
* exported functions
*/

extern "C" {
	
	jint __cdecl
		Java_java_awt_Toolkit_clrGetPixelValue ( JNIEnv* env, jclass clazz, jint rgb )
	{
		// This is the wrong way round because Kaffe stores them in a different order.
		return RGB( GetBValue( rgb), GetGValue( rgb), GetRValue( rgb));
	}
	
	void __cdecl
		Java_java_awt_Toolkit_clrSetSystemColors ( JNIEnv* env, jclass clazz, jintArray sysClrs )
	{
#ifdef NEVER /* maybe this could be initialized via X resources */
		jboolean isCopy;
		jint *rgbs = env->GetIntArrayElements( sysClrs, &isCopy);
		int  len = env->GetArrayLength( sysClrs);
		
		rgbs[ 0] = 0;  /* desktop */
		rgbs[ 1] = 0;  /* active_caption */
		rgbs[ 2] = 0;  /* active_caption_text */
		rgbs[ 3] = 0;  /* active_caption_border */
		rgbs[ 4] = 0;  /* inactive_caption */
		rgbs[ 5] = 0;  /* inactive_caption_text */
		rgbs[ 6] = 0;  /* inactive_caption_border */
		rgbs[ 7] = 0;  /* window */
		rgbs[ 8] = 0;  /* window_border */
		rgbs[ 9] = 0;  /* window_text */
		rgbs[10] = 0;  /* menu */
		rgbs[11] = 0;  /* menu_text */
		rgbs[12] = 0;  /* text */
		rgbs[13] = 0;  /* text_text */
		rgbs[14] = 0;  /* text_highlight */
		rgbs[15] = 0;  /* text_highlight_text */
		rgbs[16] = 0;  /* text_inactive_text */
		rgbs[17] = 0;  /* control */
		rgbs[18] = 0;  /* control_text */
		rgbs[19] = 0;  /* control_highlight */
		rgbs[20] = 0;  /* control_lt_highlight */
		rgbs[21] = 0;  /* control_shadow */
		rgbs[22] = 0;  /* control_dk_shadow */
		rgbs[23] = 0;  /* scrollbar */
		rgbs[24] = 0;  /* info */
		rgbs[25] = 0;  /* info_text */
		
		env->ReleaseIntArrayElements( sysClrs, rgbs, 0);
#endif
	}
	
	/*
	* we need to implement brighter / darker in the native layer because the usual
	* arithmetic Rgb brightening/darkening isn't really useful if it comes to
	* limited PseudoColor visuals (e.g. a primitive 16 color VGA wouldn't produce any
	* usable results). Even 256 colormaps suffer from that
	*/
	jlong __cdecl
		Java_java_awt_Toolkit_clrBright ( JNIEnv* env, jclass clazz, jint jrgb )
	{
		unsigned int r, g, b;
		
		r = GetBValue( jrgb);
		g = GetGValue( jrgb);
		b = GetRValue( jrgb);
		
		r = (r * 4) / 3;
		g = (g * 4) / 3;
		b = (b * 4) / 3;
		
		if ( r > 0xff ) r = 0xff;
		if ( g > 0xff ) g = 0xff;
		if ( b > 0xff ) b = 0xff;
		
		// Kaffe's RGB are the opposite order to Windows
		return RGB( b, g, r);
	}
	
	jlong __cdecl
		Java_java_awt_Toolkit_clrDark ( JNIEnv* env, jclass clazz, jint jrgb )
	{
		unsigned int r, g, b;
		
		r = GetBValue( jrgb);
		g = GetGValue( jrgb);
		b = GetRValue( jrgb);
		
		r = (r * 2) / 3;
		g = (g * 2) / 3;
		b = (b * 2) / 3;
		
		// Kaffe's RGB are the opposite order to Windows
		return RGB( b, g, r);
	}
	
}
