
/*
 * Copyright (c) 2006 - 2007
 *	Alper Akcan <alper@kaffe.org>, All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "toolkit.h"

jobject Java_java_awt_Toolkit_imgCreateFromFile (JNIEnv *env, jclass clazz UNUSED, jstring fileName)
{
	char *fname;
	s_image_t *img;
	DEBUGF("Enter");
	fname = java2CString(env, fileName);
	s_image_init(&img);
	if (s_image_img(fname, img)) {
		s_image_uninit(img);
		AWT_FREE(fname);
		return NULL;
	}
	AWT_FREE(fname);
	DEBUGF("Leave");
	return JCL_NewRawDataObject(env, img);
}

KAFFE_IMG_FUNC_DECL(jint, Java_java_awt_Toolkit_imgGetWidth)
{
	DEBUGF("Enter");
	DEBUGF("Leave");
	return UNVEIL_IMG(jimg)->w;
}

KAFFE_IMG_FUNC_DECL(jint, Java_java_awt_Toolkit_imgGetHeight)
{
	DEBUGF("Enter");
	DEBUGF("Leave");
	return UNVEIL_IMG(jimg)->h;
}

KAFFE_IMG_FUNC_DECL(jboolean, Java_java_awt_Toolkit_imgIsMultiFrame)
{
	DEBUGF("Enter");
	DEBUGF("Leave");
	return (UNVEIL_IMG(jimg)->layers->nb_elt != 0);
}
