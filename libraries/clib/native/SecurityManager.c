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

extern struct Hjava_lang_Class* getClassWithLoader(int*);

HArrayOfObject* /* HArrayOfClass */
java_lang_SecurityManager_getClassContext(struct Hjava_lang_SecurityManager* this)
{
	return ((HArrayOfObject*)getClassContext(NULL));
}

struct Hjava_lang_ClassLoader*
java_lang_SecurityManager_currentClassLoader(struct Hjava_lang_SecurityManager* this)
{
	int depth;
	struct Hjava_lang_Class* class;

	class = getClassWithLoader(&depth);
	if (class != NULL) {
		return ((struct Hjava_lang_ClassLoader*)(class->loader));
	}
	else {
		return NULL;
	}
}

jint
java_lang_SecurityManager_classDepth(struct Hjava_lang_SecurityManager* this, struct Hjava_lang_String* str)
{
	char buf[MAXNAMELEN];

	javaString2CString(str, buf, sizeof(buf));
	classname2pathname(buf, buf);

	return (classDepth(buf));
}

jint
java_lang_SecurityManager_classLoaderDepth(struct Hjava_lang_SecurityManager* this)
{
	int depth;

	(void)getClassWithLoader(&depth);

	return (depth);
}

struct Hjava_lang_Class*
java_lang_SecurityManager_currentLoadedClass0(struct Hjava_lang_SecurityManager* this)
{
	unimp("java.lang.SecurityManager:currentLoadedClass0 unimplemented");
}
