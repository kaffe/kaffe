/*
 * gnu_classpath_VMStackWalker.c
 *
 * Copyright (c) 2005
 *      Kaffe.org contributors.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "gtypes.h"
#include "access.h"
#include "constants.h"
#include "object.h"
#include "classMethod.h"
#include "itypes.h"
#include <native.h>
#include "defs.h"
#include <stdlib.h>
#include "stackTrace.h"
#include "support.h"
#include "gnu_classpath_VMStackWalker.h"

static int findCallingMethod (stackTraceInfo *info);

HArrayOfObject* /* HArrayOfClass */
gnu_classpath_VMStackWalker_getClassContext(void)
{
	stackTraceInfo* info;
	errorInfo einfo;
	int cnt;
	int i;
	int start;
	HArrayOfObject* array;

	info = (stackTraceInfo*)buildStackTrace(NULL);
	if (info == NULL) {
		postOutOfMemory(&einfo);
		throwError(&einfo);
	}

	start = findCallingMethod (info);

	cnt = 0;
	for (i = start; info[i].meth != ENDOFSTACK; i++) {
		if (info[i].meth != NULL && info[i].meth->class != NULL) {
			cnt++;
		}
	}

	array = (HArrayOfObject*)AllocObjectArray(cnt, "Ljava/lang/Class;", NULL);

	cnt = 0;
	for (i = start; info[i].meth != ENDOFSTACK; i++) {
		if (info[i].meth != NULL && info[i].meth->class != NULL) {
			unhand_array(array)->body[cnt] = (Hjava_lang_Object*)info[i].meth->class;
			cnt++;
		}
	}

	return (array);
}


static int
findCallingMethod (stackTraceInfo* info)
{
	Hjava_lang_Class*	callerOfVMStackWalker;
	int 			ret = 0;

	/* skip VMStackWalker; since we're only called from java code,
	 * we cannot reach the end of the stack here.
	 */
	while ( (info[ret].meth == NULL) ||
		(strcmp(CLASS_CNAME(info[ret].meth->class), "gnu/classpath/VMStackWalker") == 0))
	{
		ret++;
	}

	/* since the VMStackWalker methods are not called from native code,
	 * we can assume that info[ret] is the frame of a java method.
	 */
	callerOfVMStackWalker = info[ret].meth->class;

	/* advance to possible caller of caller of VMStackWalker */
	ret++;

	/* skip recursions inside the class calling VMStackWalker */
	while ( (info[ret].meth != ENDOFSTACK) &&
		((info[ret].meth == NULL) ||
		 (info[ret].meth->class == callerOfVMStackWalker)) )
	{
		ret++;
	}

	/* if the caller of VMStackWalker was called via reflection, skip that too */
	while ( (info[ret].meth != ENDOFSTACK) &&
		((info[ret].meth == NULL) ||
		 (strncmp (CLASS_CNAME(info[ret].meth->class), "java/lang/reflect/", 18) == 0)) )
	{
		ret++;
	}

	return ret;
}

Hjava_lang_Class*
gnu_classpath_VMStackWalker_getCallingClass(void)
{
	stackTraceInfo* info;
	errorInfo einfo;
	int start;

	info = (stackTraceInfo*)buildStackTrace(NULL);
	if (info == NULL) {
		postOutOfMemory(&einfo);
		throwError(&einfo);
	}

	start = findCallingMethod (info);

	return (info[start].meth == ENDOFSTACK) ? NULL : info[start].meth->class;
}

Hjava_lang_ClassLoader*
gnu_classpath_VMStackWalker_getCallingClassLoader(void)
{
	stackTraceInfo* info;
	errorInfo einfo;
	int start;

	info = (stackTraceInfo*)buildStackTrace(NULL);
	if (info == NULL) {
		postOutOfMemory(&einfo);
		throwError(&einfo);
	}

	start = findCallingMethod (info);

	return (info[start].meth == ENDOFSTACK) ? NULL : info[start].meth->class->loader;
}
