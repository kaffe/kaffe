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

jobject
Java_java_awt_Toolkit_imgCreateImage(JNIEnv* env, jclass clazz, jint width, jint height )
{
	GR_WINDOW_ID image;

	image = GrNewPixmap( width, height, 0);

	return (jobject)image;
}

jobject
Java_java_awt_Toolkit_imgCreateScreenImage(JNIEnv* env, jclass clazz, jint width, jint height )
{
	GR_WINDOW_ID image;

	image = GrNewPixmap( width, height, 0);

	return (jobject)image;
}

void
Java_java_awt_Toolkit_imgSetIdxPels ( JNIEnv* env, jclass clazz, jobject _jimg, jint x, jint y, jint w, jint h, jintArray clrMap, jbyteArray idxPels, jint trans, jint off, jint scan)
{
	// NIY
}

void
Java_java_awt_Toolkit_imgSetRGBPels(JNIEnv* envP, jclass clazz, jobject _jimage, jint x, jint y, jint w, jint h, jintArray _jrgbs, jint off, jint scan)
{
	jboolean isCopy;
	jint *rgbs;
	int yi,xi,i, i0;
	GR_WINDOW_ID image;
	GR_GC_ID gc;

	if (( _jimage == NULL ) || ( _jrgbs == NULL )) {
		SignalError("java.lang.NullPointerException", "no object");
		return;
	}
	rgbs = (*envP)->GetIntArrayElements(envP, _jrgbs, &isCopy);
	if ( rgbs == NULL ) {
		SignalError("java.lang.OutOfMemoryError", "can't alloc");
		return;
	}

	gc = GrNewGC();
	image = (GR_WINDOW_ID)_jimage;
	i0 = y * scan + off;
	for ( yi = y; yi < (y + h); yi++, i0 +=scan) {
		i = i0;
		for ( xi = x; xi < (x + w); xi++, i++) {
			GrSetGCForeground(gc, rgbs[i]);
			GrPoint(image, gc, xi, yi);
		}
	}
	GrDestroyGC(gc);

	(*envP)->ReleaseIntArrayElements(envP, _jrgbs, rgbs, JNI_ABORT);
}

void
Java_java_awt_Toolkit_imgComplete(JNIEnv* envP, jclass clazz, jobject _jobject, jint status )
{
	// NIY
}

void
Java_java_awt_Toolkit_imgFreeImage(JNIEnv* envP, jclass clazz, jobject _jimage)
{
	GR_WINDOW_ID image;

	if ( _jimage == NULL ) {
		SignalError("java.lang.NullPointerException", "no object");
		return;
	}
	image = (GR_WINDOW_ID)_jimage;

	GrDestroyWindow(image);
}

jobject
Java_java_awt_Toolkit_imgCreateScaledImage( JNIEnv* env, jclass clazz, jobject _jimage, int width, int height )
{
	return (GR_WINDOW_ID)_jimage;
#if 0
	GR_WINDOW_ID image, newImage;
	GR_WINDOW_INFO image_info;
	GR_PIXELVAL *pixelP;
	GR_GC_ID gc;
	int x, y, xi, yi, off_y;
	float scale_x, scale_y;

	if ( _jimage == NULL ) {
		SignalError("java.lang.NullPointerException", "no object");
		return NULL;
	}
	image = (GR_WINDOW_ID)_jimage;
	newImage = GrNewPixmap( width, height, 0);
	if ( newImage == NULL ) {
		SignalError("java.lang.OutOfMemoryError", "can't alloc newimage");
		return NULL;
	}
	GrGetWindowInfo(image, &image_info);
	pixelP = TLK_MALLOC( image_info.width * image_info.height);
	if ( pixelP == NULL ) {
		SignalError("java.lang.OutOfMemoryError", "can't alloc");
		return NULL;
	}
	GrReadArea(image, 0, 0, image_info.width, image_info.height, pixelP);

	scale_y = (image_info.height / height);
	scale_x = (image_info.width / width);
	gc = GrNewGC();
	for( yi = 0 ; yi < height; yi++ ) {
		y = yi * scale_y;
		off_y = y * image_info.height;
		for( xi = 0 ; xi < width; xi++ ) {
			x = xi * scale_x;
			GrSetGCForeground(gc, pixelP[off_y + x]);
			GrPoint(newImage, gc, xi, yi);
		}
	}
	GrDestroyGC(gc);
	TLK_FREE(pixelP);
	return (jobject)newImage;
#endif
}

jobject
Java_java_awt_Toolkit_imgCreateFromFile(JNIEnv* envP, jclass clazz, jstring _jfilename)
{
	GR_GC_ID gc;
	GR_WINDOW_ID image;
	GR_IMAGE_ID native_image;
	GR_IMAGE_INFO iinfo;
	const char *filename;

	if ( _jfilename == NULL ) {
		SignalError("java.lang.NullPointerException", "no string object");
		return NULL;
	}
	filename = (*envP)->GetStringUTFChars(envP, _jfilename, NULL);
	if ( filename == NULL ) {
		SignalError("java.lang.OutOfMemoryError", "can't alloc");
		return NULL;
	}

	native_image = GrLoadImageFromFile((char *)filename, 0);
	if ( native_image == NULL ) {
		return NULL;
	}
	GrGetImageInfo(native_image, &iinfo);
	image = GrNewPixmap(iinfo.width, iinfo.height, 0);
	if ( image == NULL ) {
		GrFreeImage(native_image);
		return NULL;
	}

	gc = GrNewGC();
	GrDrawImageToFit(image, gc, 0, 0, iinfo.width, iinfo.height, native_image);
	GrDestroyGC(gc);
	GrFreeImage(native_image);

	return (jobject)image;
}

jobject
Java_java_awt_Toolkit_imgCreateFromData(JNIEnv* env, jclass clazz, jbyteArray jbuffer, jint off, jint len )
{
	return NULL;
}

jobject
Java_java_awt_Toolkit_imgSetFrame ( JNIEnv* env, jclass clazz, jobject _jimage, int frameNo)
{
	return _jimage;
}

jint
Java_java_awt_Toolkit_imgGetWidth( JNIEnv* env, jclass clazz, jobject _jimage)
{
	GR_WINDOW_ID image;
	GR_WINDOW_INFO image_info;

	if ( _jimage == NULL ) {
		SignalError("java.lang.NullPointerException", "no object");
		return -1;
	}
	image = (GR_WINDOW_ID)_jimage;

	GrGetWindowInfo(image, &image_info);

	return image_info.width;
}

jint
Java_java_awt_Toolkit_imgGetHeight( JNIEnv* env, jclass clazz, jobject _jimage)
{
	GR_WINDOW_ID image;
	GR_WINDOW_INFO image_info;

	if ( _jimage == NULL ) {
		SignalError("java.lang.NullPointerException", "no object");
		return -1;
	}
	image = (GR_WINDOW_ID)_jimage;

	GrGetWindowInfo(image, &image_info);

	return image_info.height;
}

jboolean
Java_java_awt_Toolkit_imgIsMultiFrame( JNIEnv* env, jclass clazz, jobject _jimage)
{
	return JNI_FALSE;
}

jint
Java_java_awt_Toolkit_imgGetLatency( JNIEnv* env, jclass clazz, jobject _jimage)
{
	return 0;
}

jobject
Java_java_awt_Toolkit_imgGetNextFrame( JNIEnv* env, jclass clazz, jobject _jimage)
{
	return NULL;
}



void
Java_java_awt_Toolkit_imgProduceImage(JNIEnv* envP, jclass clazz, jobject _producer, jobject _jimage)
{
	jintArray pixelArray;
	jint *pixelP;
	jboolean isCopy;
	jclass prodClazz;
	jmethodID setDim, setCM, setHints, setPix, imgCompl;
	jobject model;

	static jclass modelClazz;
	static jmethodID modelCtor;

	GR_WINDOW_ID image;
	GR_WINDOW_INFO image_info;
	int x,y;

	if (( _jimage == NULL ) || ( _producer )) {
		SignalError("java.lang.NullPointerException", "no object");
		return;
	}
	image = (GR_WINDOW_ID)_jimage;

	if ( modelClazz == NULL ) {
		modelClazz = (*envP)->FindClass( envP, "kaffe/awt/JavaColorModel");
		modelCtor = (*envP)->GetStaticMethodID( envP, modelClazz, "getSingleton", "()Lkaffe/awt/JavaColorModel;");
	}

	prodClazz  = (*envP)->GetObjectClass( envP, _producer);
	setDim    = (*envP)->GetMethodID( envP, prodClazz, "setDimensions", "(II)V");
	setCM     = (*envP)->GetMethodID( envP, prodClazz, "setColorModel", "(Ljava/awt/image/ColorModel;)V");
	setHints  = (*envP)->GetMethodID( envP, prodClazz, "setHints", "(I)V");
	setPix    = (*envP)->GetMethodID( envP, prodClazz, "setPixels", "(IIIILjava/awt/image/ColorModel;[III)V");
	imgCompl  = (*envP)->GetMethodID( envP, prodClazz, "imageComplete", "(I)V");
	model     = (*envP)->CallStaticObjectMethod( envP, modelClazz, modelCtor);

	GrGetWindowInfo(image, &image_info);
	pixelArray  = (*envP)->NewIntArray(envP, image_info.width * image_info.height);
	if (pixelArray == NULL) {
		SignalError("java.lang.OutOfMemoryError", "can't alloc");
		return;
	}
	pixelP = (*envP)->GetIntArrayElements( envP, pixelArray, &isCopy);
	if ( pixelP == NULL ) {
		SignalError("java.lang.OutOfMemoryError", "can't alloc");
		return;
	}

	(*envP)->CallVoidMethod(envP, _producer, setDim, image_info.width, image_info.height);
	(*envP)->CallVoidMethod(envP, _producer, setCM, model);
	(*envP)->CallVoidMethod(envP, _producer, setHints, TLK_AWT_IC_TOPDOWNLEFTRIGHT | TLK_AWT_IC_COMPLETESCANLINES);

	assert( sizeof(GR_PIXELVAL) == sizeof(unsigned long));
	GrReadArea(image, 0, 0, image_info.height, image_info.width, (GR_PIXELVAL *)pixelP);
	for( y = 0 ; y < image_info.height ; y++ ) {
		for( x = 0 ; y < image_info.width ; y++ ) {
			*pixelP = (
				((*pixelP & 0xFF0000) >> 16) |
				((*pixelP & 0x00FF00)) |
				((*pixelP & 0x0000FF) << 16));
		}
	}

	if ( isCopy ) {
		(*envP)->ReleaseIntArrayElements(envP, pixelArray, pixelP, JNI_COMMIT);
	}

	(*envP)->CallVoidMethod(envP, _producer, setPix, 0, 0, image_info.width, image_info.height, model, pixelArray, 0, image_info.width);
	(*envP)->CallVoidMethod(envP, _producer, imgCompl, TLK_AWT_IC_STATICIMAGEDONE);
}
