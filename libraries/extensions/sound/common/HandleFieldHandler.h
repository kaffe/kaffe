/*
 *	HandleFieldHandler.h
 */

/*
 *  Copyright (c) 1999 - 2002 by Matthias Pfisterer <Matthias.Pfisterer@web.de>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as published
 *   by the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details.
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef _HANDLE_FIELD_HANDLER_H
#define _HANDLE_FIELD_HANDLER_H


#include <jni.h>


#define HandleFieldHandler(_type)            \
HandleFieldHandlerDeclaration(_handler, _type)

#define HandleFieldHandlerDeclaration(_variableName, _type)            \
static jfieldID  _variableName ## FieldID = NULL;                      \
                                                                       \
static jfieldID                                                        \
getNativeHandleFieldID(JNIEnv *env, jobject obj)                       \
{                                                                      \
	if (_variableName ## FieldID == NULL)                          \
	{                                                              \
		jclass	cls = (*env)->GetObjectClass(env, obj);                \
		if (cls == NULL)                                       \
		{                                                      \
			throwRuntimeException(env, "cannot get class"); \
		}                                                      \
		_variableName ## FieldID = (*env)->GetFieldID(env, cls, "m_lNativeHandle", "J"); \
		if (_variableName ## FieldID == NULL)                  \
		{                                                      \
			throwRuntimeException(env, "cannot get field ID for m_lNativeHandle"); \
		}                                                      \
	}                                                              \
	return _variableName ## FieldID;                               \
}                                                                      \
                                                                       \
static void                                                            \
setHandle(JNIEnv *env, jobject obj, _type handle)                      \
{                                                                      \
	jfieldID	fieldID = getNativeHandleFieldID(env, obj);    \
	(*env)->SetLongField(env, obj, fieldID, (jlong) (int) handle);       \
}                                                                      \
                                                                       \
static _type                                                           \
getHandle(JNIEnv *env, jobject obj)                                    \
{                                                                      \
	jfieldID	fieldID = getNativeHandleFieldID(env, obj);    \
	_type	handle = (_type) (int) (*env)->GetLongField(env, obj, fieldID); \
	return handle;                                                 \
}



#endif /* _HANDLE_FIELD_HANDLER_H */


/*** HandleFieldHandler.h ***/
