/*
 * java.lang.SecurityManager.c
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "lib-license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "../../../kaffe/kaffevm/gtypes.h"
#include "../../../kaffe/kaffevm/access.h"
#include "../../../kaffe/kaffevm/constants.h"
#include "../../../kaffe/kaffevm/object.h"
#include "../../../kaffe/kaffevm/classMethod.h"
#include "../../../kaffe/kaffevm/itypes.h"
#include <native.h>
#include "defs.h"
#include <stdlib.h>
#include "SecurityManager.h"
#include "../../../kaffe/kaffevm/stackTrace.h"
#include "../../../kaffe/kaffevm/support.h"

extern Hjava_lang_Object* buildStackTrace(struct _exceptionFrame*);

HArrayOfObject* /* HArrayOfClass */
java_lang_SecurityManager_getClassContext0(void)
{
	stackTraceInfo* info;
	int cnt;
	int i;
	HArrayOfObject* array;

	info = (stackTraceInfo*)buildStackTrace(0);

	cnt = 0;
	for (i = 0; info[i].meth != ENDOFSTACK; i++) {
		if (info[i].meth != 0 && info[i].meth->class != 0) {
			cnt++;
		}
	}

	array = (HArrayOfObject*)AllocObjectArray(cnt, "Ljava/lang/Class;");

	cnt = 0;
	for (i = 0; info[i].meth != ENDOFSTACK; i++) {
		if (info[i].meth != 0 && info[i].meth->class != 0) {
			unhand(array)->body[cnt] = (Hjava_lang_Object*)info[i].meth->class;
			cnt++;
		}
	}

	return (array);
}
