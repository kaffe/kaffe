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

#include <jni.h>

#define PIXEL_VALUE_RGB(_rgb_) ( \
		(((_rgb_) & 0xFF0000) >> 16 ) | \
		(((_rgb_) & 0x00FF00) ) | \
		(((_rgb_) & 0x0000FF) ) << 16 )

#define PIXEL_VALUE(_r_,_g_,_b_) ( \
		((_r_)) | \
		((_g_) << 8 ) | \
		((_b_) << 16 ))

/* java style rgb to native color value */
jint
Java_java_awt_Toolkit_clrGetPixelValue( JNIEnv* env, jclass clazz, jint rgb )
{
	return (jint)PIXEL_VALUE_RGB(rgb);
}

jlong
Java_java_awt_Toolkit_clrBright( JNIEnv* envP, jclass clazz, jint rgb )
{
	unsigned int r, g, b;
	jlong aRgb, aPix;

	r = JAVA_RED(rgb);
	g = JAVA_GREEN(rgb);
	b = JAVA_BLUE(rgb);

	r = (r * 4) / 3;
	g = (g * 4) / 3;
	b = (b * 4) / 3;

	if ( r > 0xff ) r = 0xff;
	if ( g > 0xff ) g = 0xff;
	if ( b > 0xff ) b = 0xff;

	aRgb = JAVA_RGB(r, g, b);
	aPix = PIXEL_VALUE(r, g , b);

	return ((aPix << 32) | aRgb);
}

jlong
Java_java_awt_Toolkit_clrDark( JNIEnv* env, jclass clazz, jint rgb )
{
	unsigned int r, g, b;
	jlong aRgb, aPix;

	r = JAVA_RED(rgb);
	g = JAVA_GREEN(rgb);
	b = JAVA_BLUE(rgb);

	r = (r * 2) / 3;
	g = (g * 2) / 3;
	b = (b * 2) / 3;

	aRgb = JAVA_RGB(r, g, b);
	aPix = PIXEL_VALUE(r, g, b);

	return ((aPix << 32) | aRgb);
}

void
Java_java_awt_Toolkit_clrSetSystemColors(JNIEnv *envP, jclass clazz, jintArray sysColors )
{
	// not implement yet
}

jobject
Java_java_awt_Toolkit_clrGetColorModel(JNIEnv* envP, jclass clazz )
{
	jclass colorModelClazz;
	jmethodID colorModelCtorId;

	colorModelClazz=(*envP)->FindClass(envP, "java/awt/image/DirectColorModel");
	colorModelCtorId = (*envP)->GetMethodID( envP, colorModelClazz, "<init>", "(IIIII)V");
	return (*envP)->NewObject( envP, colorModelClazz, colorModelCtorId,
		24,
		0x0000FF,
		0x00FF00,
		0xFF0000,
		0);
}
