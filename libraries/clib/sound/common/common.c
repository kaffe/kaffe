/*
 *	common.c
 */

/*
 *  Copyright (c) 1999 - 2001 by Matthias Pfisterer <Matthias.Pfisterer@gmx.de>
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


#include "common.h"
#include "debug.h"



void
throwRuntimeException(JNIEnv *env, const char* pStrMessage)
{
	static  jclass	runtimeExceptionClass = NULL;

	if ((*env)->ExceptionOccurred(env) != NULL)
	{
		if (debug_flag) { (*env)->ExceptionDescribe(env); }
		(*env)->ExceptionClear(env);
	}
	if (runtimeExceptionClass == NULL)
	{
		runtimeExceptionClass = (*env)->FindClass(env, "java/lang/RuntimeException");
		if (debug_flag) { fprintf(debug_file, "RTE: %p\n", runtimeExceptionClass); }
		if (runtimeExceptionClass == NULL)
		{
			(*env)->FatalError(env, "cannot get class object for java.lang.RuntimeException");
		}
	}
	(*env)->ThrowNew(env, runtimeExceptionClass, pStrMessage);
}



void
checkArrayLength(JNIEnv *env, jarray array, int nRequiredLength)
{
	int	nLength;

	nLength = (*env)->GetArrayLength(env, array);
	if (nLength < nRequiredLength)
	{
		throwRuntimeException(env, "array does not have enough elements");
	}
}



void
setStringArrayElement(JNIEnv *env, jobjectArray array, int nIndex, const char* string1)
{
	jstring		string2;

	string2 = (*env)->NewStringUTF(env, string1);
	if (string1 == NULL)
	{
		throwRuntimeException(env, "NewStringUTF() failed");
	}
	(*env)->SetObjectArrayElement(env, array, nIndex, string2);
}



/*** common.c ***/
