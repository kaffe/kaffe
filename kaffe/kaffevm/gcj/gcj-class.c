/*
 * gcj-class.c
 * ...
 *
 * Copyright (c) 1996, 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#include "config.h"
#include "config-std.h"
#include "gtypes.h"
#include "classMethod.h"
#include "baseClasses.h"
#include "itypes.h"
#include "access.h"
#include "external.h"
#include "constants.h"
#include "md.h"
#include "jar.h"
#include "classpath.h"
#include "stringSupport.h"
#include "gcj.h"

#if defined(HAVE_GCJ_SUPPORT)

static Hjava_lang_Class* preCList;

/*
 * The only external reference made by GCJ compiled classes it to
 * the virtual dispatch table for java.lang.Class.  We must patch
 * this up by hand.
 */
int __vt_Q34java4lang5Class[1];

void gcjProcessClass(Hjava_lang_Class*);

/*
 * Load any shared objects in the class path.
 */
void
gcjInit(void)
{
	classpathEntry* entry;

        /* Load any shared objects into VM now */
        for (entry = classpath; entry != 0; entry = entry->next) {
                if (entry->type == CP_SOFILE) {
                        loadNativeLibrary(entry->path, NULL, 0);
                }
        }
}

/*
 * Register a pre-built class.
 */
void
_Jv_RegisterClass(Hjava_lang_Class* clazz)
{
	clazz->centry = (classEntry*)preCList;
	preCList = clazz;
}

bool
gcjFindClass(classEntry* centry)
{
	Hjava_lang_Class** ptr;

	for (ptr = &preCList; *ptr != 0; ptr = (Hjava_lang_Class**)&(*ptr)->centry) {
		if (utf8ConstEqual(centry->name, (*ptr)->name)) {
			centry->class = *ptr;
			*ptr = (Hjava_lang_Class*)(*ptr)->centry;
			centry->class->centry = centry;
			centry->class->state = CSTATE_PRELOADED;
			SET_CLASS_GCJ(centry->class);
			return (true);
		}
	}
	return (false);
}

/*
 * Process a prebuild method.
 */
void
gcjProcessClass(Hjava_lang_Class* clazz)
{
	if (CLASS_IS_INTERFACE(clazz)) {
		clazz->superclass = ObjectClass;
	}
	clazz->head.dtable = ClassClass->dtable;
}

#endif
