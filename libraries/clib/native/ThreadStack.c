/*
 * kaffe.lang.ThreadStack.c
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
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
#include "kaffe_lang_ThreadStack.h"
#include "stackTrace.h"
#include "support.h"

HArrayOfObject* /* HArrayOfClass */
kaffe_lang_ThreadStack_getClassStack(void)
{
	stackTraceInfo* info;
	errorInfo einfo;
	int cnt;
	int i;
	HArrayOfObject* array;

	info = (stackTraceInfo*)buildStackTrace(0);
	if (info == NULL) {
		postOutOfMemory(&einfo);
		throwError(&einfo);
	}

	cnt = 0;
	for (i = 0; info[i].meth != ENDOFSTACK; i++) {
		if (info[i].meth != 0 && info[i].meth->class != 0) {
			cnt++;
		}
	}

	array = (HArrayOfObject*)AllocObjectArray(cnt, "Ljava/lang/Class;", 0);

	cnt = 0;
	for (i = 0; info[i].meth != ENDOFSTACK; i++) {
		if (info[i].meth != 0 && info[i].meth->class != 0) {
			unhand_array(array)->body[cnt] = (Hjava_lang_Object*)info[i].meth->class;
			cnt++;
		}
	}

	return (array);
}
