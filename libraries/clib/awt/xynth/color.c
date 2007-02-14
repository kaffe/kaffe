
/*
 * Copyright (c) 2006 - 2007
 *	Alper Akcan <alper@kaffe.org>, All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "toolkit.h"

#define JRGB(_r, _g, _b) (((_r) << 16) | ((_g) << 8) | (_b))
#define JRED(_rgb)       (((_rgb) & 0xff0000) >> 16)
#define JGREEN(_rgb)     (((_rgb) & 0x00ff00) >> 8)
#define JBLUE(_rgb)      ((_rgb) & 0x0000ff)

jint Java_java_awt_Toolkit_clrGetPixelValue (JNIEnv *env, jclass clazz, jint rgb)
{
	jint pix;
	DEBUGF("Enter");
	pix = s_rgbcolor(xynth->root->surface, JRED(rgb), JGREEN(rgb), JBLUE(rgb));
	DEBUGF("Leave");
	return pix;
}

jlong Java_java_awt_Toolkit_clrBright (JNIEnv *env UNUSED, jclass clazz UNUSED, jint rgb)
{
	unsigned int r;
	unsigned int g;
	unsigned int b;
	jint modRgb;
	jint modPix;
	
	r = JRED(rgb);
	g = JGREEN(rgb);
	b = JBLUE(rgb);
	
	r = (r * 4) / 3;
	g = (g * 4) / 3;
	b = (b * 4) / 3;
	
	if (r > 0xff) r = 0xff;
	if (g > 0xff) g = 0xff;
	if (b > 0xff) b = 0xff;
	
	modRgb = JRGB(r, g, b);
	modPix = s_rgbcolor(xynth->root->surface, r, g, b);
	
	return (((jlong) modPix << 32) | modRgb);
}

jlong Java_java_awt_Toolkit_clrDark (JNIEnv *env UNUSED, jclass clazz UNUSED, jint rgb)
{
	unsigned int r;
	unsigned int g;
	unsigned int b;
	jint modRgb;
	jint modPix;
	
	r = JRED(rgb);
	g = JGREEN(rgb);
	b = JBLUE(rgb);
	
	r = (r * 2) / 3;
	g = (g * 2) / 3;
	b = (b * 2) / 3;
	
	modRgb = JRGB(r, g, b);
	modPix = s_rgbcolor(xynth->root->surface, r, g, b);
	
	return (((jlong) modPix << 32) | modRgb);
}
