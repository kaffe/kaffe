/**
 * nano-X AWT backend for Kaffe.
 *
 * Copyright (c) 2001
 *	Exor International Inc. All rights reserved.
 *
 * Copyright (c) 2001
 *	Sinby Corporatin, All rights reserved.
 *
 * Copyright (c) 2005
 *	Kaffe.org contributors, see ChangeLog for details.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "config-setjmp.h"

#include "toolkit.h"

#include "jni.h"

#define getFontName(spec,style) (spec)
#define getDefaultFontName(style) (spec)

void*
Java_java_awt_Toolkit_fntInitFont( JNIEnv* envP, jclass clazz, jstring jSpec, jint style, jint size )
{
	GR_FONT_ID fontid;

#if 0
	const char *spec,*newspec;

	if ( jSpec == NULL ) {
		spec = NULL;
	} else {
		spec = (*envP)->GetStringUTFChars(envP , jSpec, NULL);
	}
	if ( spec == NULL ) {
		fontid = GrCreateFont(GR_FONT_SYSTEM_VAR, 0, NULL);
	} else {
		newspec = getFontName(spec,style);

		fontid = GrCreateFont((char *)newspec, size, NULL);
		if ( fontid == NULL ) {
			fontid = GrCreateFont((char *)spec, size, NULL);
		}
	}
#endif
	fontid = GrCreateFont(GR_FONT_SYSTEM_VAR, 0, NULL);
	
	return (void*)fontid;
}

void
Java_java_awt_Toolkit_fntFreeFont ( JNIEnv* env, jclass clazz, jobject _jfont )
{

	if ( _jfont == NULL) {
		SignalError("java.lang.NullPointerException", "no font object");
		return;
	}
	GrDestroyFont((GR_FONT_ID)_jfont);
}

void*
Java_java_awt_Toolkit_fntInitFontMetrics( JNIEnv* env, jclass clazz, jobject _jfont )
{
	return _jfont;
}

void 
Java_java_awt_Toolkit_fntFreeFontMetrics(JNIEnv *env, jclass k, jobject a1) 
{
}

jint
Java_java_awt_Toolkit_fntGetFixedWidth ( JNIEnv* env, jclass clazz, jobject _jfont )
{
	GR_FONT_ID fontid;
	GR_FONT_INFO fontinfo;

	if ( _jfont == NULL) {
		SignalError("java.lang.NullPointerException", "no font object");
		return 0;
	}
	fontid = (GR_FONT_ID)_jfont;
	GrGetFontInfo(fontid,&fontinfo);

	return fontinfo.maxwidth;
}

jint
Java_java_awt_Toolkit_fntGetHeight( JNIEnv* env, jclass clazz, jobject _jfont )
{
	GR_FONT_ID fontid;
	GR_FONT_INFO fontinfo;

	if ( _jfont == NULL) {
		SignalError("java.lang.NullPointerException", "no font object");
		return 0;
	}
	fontid = (GR_FONT_ID)_jfont;
	GrGetFontInfo(fontid,&fontinfo);

	return fontinfo.height;
}

jint
Java_java_awt_Toolkit_fntGetAscent( JNIEnv* env, jclass clazz, jobject _jfont )
{
	GR_FONT_ID fontid;
	GR_FONT_INFO fontinfo;

	if ( _jfont == NULL) {
		SignalError("java.lang.NullPointerException", "no font object");
		return 0;
	}
	fontid = (GR_FONT_ID)_jfont;
	GrGetFontInfo(fontid,&fontinfo);

	return fontinfo.baseline;
}

jint
Java_java_awt_Toolkit_fntGetDescent( JNIEnv* env, jclass clazz, jobject _jfont )
{
	GR_FONT_ID fontid;
	GR_FONT_INFO fontinfo;

	if ( _jfont == NULL) {
		SignalError("java.lang.NullPointerException", "no font object");
		return 0;
	}
	fontid = (GR_FONT_ID)_jfont;
	GrGetFontInfo(fontid,&fontinfo);

	return fontinfo.height - fontinfo.baseline;
}

jint
Java_java_awt_Toolkit_fntGetMaxAscent( JNIEnv* env, jclass clazz, jobject _jfont )
{
	GR_FONT_ID fontid;
	GR_FONT_INFO fontinfo;

	if ( _jfont == NULL) {
		SignalError("java.lang.NullPointerException", "no font object");
		return 0;
	}
	fontid = (GR_FONT_ID)_jfont;
	GrGetFontInfo(fontid,&fontinfo);

	return fontinfo.baseline;
}

jint
Java_java_awt_Toolkit_fntGetMaxDescent( JNIEnv* env, jclass clazz, jobject _jfont )
{
	GR_FONT_ID fontid;
	GR_FONT_INFO fontinfo;

	if ( _jfont == NULL) {
		SignalError("java.lang.NullPointerException", "no font object");
		return 0;
	}
	fontid = (GR_FONT_ID)_jfont;
	GrGetFontInfo(fontid,&fontinfo);

	return fontinfo.height - fontinfo.baseline;
}

jint 
Java_java_awt_Toolkit_fntGetLeading(JNIEnv *env, jclass k, jobject _jfont)
{
	if ( _jfont == NULL) {
		SignalError("java.lang.NullPointerException", "no font object");
		return 0;
	}
	return 0;
}

jint 
Java_java_awt_Toolkit_fntGetMaxAdvance(JNIEnv *env, jclass k, jobject _jfont)
{
	if ( _jfont == NULL) {
		SignalError("java.lang.NullPointerException", "no font object");
		return 0;
	}
	return 0;
}

jboolean
Java_java_awt_Toolkit_fntIsWideFont( JNIEnv* env, jclass clazz, jobject _jfont )
{
	GR_FONT_ID fontid;
	GR_FONT_INFO fontinfo;

	if ( _jfont == NULL) {
		SignalError("java.lang.NullPointerException", "no font object");
		return 0;
	}
	fontid = (GR_FONT_ID)_jfont;
	GrGetFontInfo(fontid,&fontinfo);

	return (fontinfo.lastchar < 256 )?JNI_FALSE:JNI_TRUE;
}

jint
Java_java_awt_Toolkit_fntStringWidth( JNIEnv* env, jclass clazz, jobject _jfont, jstring _jstr )
{
	jboolean isCopy;
	const jchar    *jc;
	int      len;
	int      i,w;
	GR_FONT_INFO fontinfo;
	GR_FONT_ID fontid;

	if (( _jfont == NULL) || (_jstr == NULL)) {
		SignalError("java.lang.NullPointerException", "no object");
		return 0;
	}

	jc = (*env)->GetStringChars( env, _jstr, &isCopy);
	if ( jc == NULL ) {
		SignalError("java.lang.OutOfMemoryError", "can't alocate chars");
		return 0;
	}
	len = (*env)->GetStringLength( env, _jstr);

	fontid = (GR_FONT_ID)_jfont;
	GrGetFontInfo(fontid,&fontinfo);
	w = 0;
	for( i = 0 ; i < len ; i++ ) {
		unsigned int index;
		index = (unsigned int)jc[i];
		if (( index < fontinfo.firstchar ) ||( index > fontinfo.lastchar )) {
			SignalError("java.lang.InternalError", "font id error");
		}
		w += fontinfo.widths[index];
	}

	(*env)->ReleaseStringChars( env, _jstr, jc);
	return w;
}

jobject
Java_java_awt_Toolkit_fntGetWidths(JNIEnv* envP, jclass clazz, jobject _jfont )
{
	GR_FONT_ID fontid;
	GR_FONT_INFO fontinfo;
	jintArray widths;
	jint *jw;
	jboolean isCopy;
	int i;

	if ( _jfont == NULL) {
		SignalError("java.lang.NullPointerException", "no font object");
		return 0;
	}
	fontid = (GR_FONT_ID)_jfont;
	GrGetFontInfo(fontid,&fontinfo);

	widths = (*envP)->NewIntArray(envP, fontinfo.lastchar + 1);
	if ( widths == NULL ) {
		SignalError("java.lang.OutOfMemoryError", "can't alocate int array");
		return 0;
	}
	jw = (*envP)->GetIntArrayElements(envP, widths, &isCopy);
	if ( jw == NULL ) {
		SignalError("java.lang.OutOfMemoryError","can't alocate int array");
		return 0;
	}


	if ( fontinfo.fixed ) {
		for ( i = fontinfo.firstchar ; i <= fontinfo.lastchar ; i++ ) {
			jw[i] = fontinfo.maxwidth;
		}
	} else {
		for ( i = fontinfo.firstchar ; i <= fontinfo.lastchar ; i++ ) {
			jw[i] = fontinfo.widths[i];
		}
	}

	(*envP)->ReleaseIntArrayElements(envP, widths, jw, 0);

	return widths;
}

jint 
Java_java_awt_Toolkit_fntBytesWidth(JNIEnv *envP, jclass k, jobject _jfont, 
jbyteArray _jbytes, jint off, jint len) 
{
	GR_FONT_ID fontid;
	GR_FONT_INFO fontinfo;
	jboolean isCopy;
	int w;
	int n,i;
	jbyte *jb;

	if ( _jfont == NULL) {
		SignalError("java.lang.NullPointerException", "no font object");
		return 0;
	}
	if ( _jbytes == NULL) {
		SignalError("java.lang.NullPointerException", "no byte array ");
		return 0;
	}
	fontid = (GR_FONT_ID)_jfont;
	GrGetFontInfo(fontid,&fontinfo);

	n = (*envP)->GetArrayLength(envP, _jbytes);
	if ( n < ( off + len )) {
		return 0;
	}
	if ( fontinfo.fixed ) {
		return len * fontinfo.maxwidth;
	} else {
		jb = (*envP)->GetByteArrayElements(envP, _jbytes, &isCopy);
		if ( jb == NULL ) {
			SignalError("java.lang.OutOfMemoryError","can't alocate byte array");
			return 0;
		}
		w = 0;
		for( i = off; i < off + len; i++ ) {
			if ( fontinfo.lastchar < jb[i] ) {
				return 0;
			}
			w += fontinfo.widths[(unsigned int)jb[i]];
		}
		(*envP)->ReleaseByteArrayElements(envP, _jbytes, jb, 0);
	}

	return w;
}

jint
Java_java_awt_Toolkit_fntCharWidth( JNIEnv* envP, jclass clazz, jobject _jfont, jchar jChar )
{
	GR_FONT_ID fontid;
	GR_FONT_INFO fontinfo;

	if ( _jfont == NULL) {
		SignalError("java.lang.NullPointerException", "no font object");
		return 0;
	}
	fontid = (GR_FONT_ID)_jfont;
	GrGetFontInfo(fontid,&fontinfo);

	if ( fontinfo.lastchar < jChar ) {
		return 0;
	} else if ( fontinfo.fixed ) {
		return fontinfo.maxwidth;
	} else {
		return fontinfo.widths[(unsigned int)jChar];
	}
}

jint
Java_java_awt_Toolkit_fntCharsWidth ( JNIEnv* envP, jclass clazz, jobject _jfont, jcharArray _jchars, jint off, jint len )
{
	GR_FONT_ID fontid;
	GR_FONT_INFO fontinfo;
	jboolean  isCopy;
	jchar *jc;
	int n, w, i;

	if ( _jfont == NULL) {
		SignalError("java.lang.NullPointerException", "no font object");
		return 0;
	}
	if ( _jchars == NULL ) {
		SignalError("java.lang.NullPointerException", "no char array object");
		return 0;
	}
	fontid = (GR_FONT_ID)_jfont;
	GrGetFontInfo(fontid,&fontinfo);
	
	n = (*envP)->GetArrayLength( envP, _jchars);
	if ( n < ( off + len )) {
		return 0;
	}

	if ( fontinfo.fixed ) {
		return len * fontinfo.maxwidth;
	} else {
		jc = (*envP)->GetCharArrayElements(envP, _jchars, &isCopy);
		if ( jc == NULL ) {
			SignalError("java.lang.OutOfMemoryError", "can't alocate char array");
			return 0;
		}
		w = 0;
		for( i = off; i < off + len; i++ ) {
			if ( fontinfo.lastchar < jc[i] ) {
				return 0;
			}
			w += fontinfo.widths[(unsigned int)jc[i]];
		}
		(*envP)->ReleaseCharArrayElements( envP, _jchars, jc, JNI_ABORT);
	}

	return w;
}
