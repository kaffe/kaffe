/*
 * kaffe.lang.PrimordialClassLoader.c
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
#include "file.h"
#include "readClass.h"
#include "constants.h"
#include "access.h"
#include "classMethod.h"
#include "object.h"
#include "locks.h"
#include "itypes.h"
#include "support.h"
#include "stringSupport.h"
#include "baseClasses.h"
#include "exception.h"
#include "kaffe_lang_PrimordialClassLoader.h"
#include <native.h>
#include "defs.h"

/*
 * Load a system class.
 */
struct Hjava_lang_Class*
kaffe_lang_PrimordialClassLoader_findClass0(Hkaffe_lang_PrimordialClassLoader* this UNUSED, Hjava_lang_String* jStr)
{
	Hjava_lang_Class *clazz = NULL;
	errorInfo info;
	int error = 0;
        Utf8Const *c;
        char *name;

	name = checkPtr(stringJava2C(jStr));
	classname2pathname(name, name);

	if( (c = utf8ConstNew(name, -1)) )
	{
		if( c->data[0] == '[' )
		{
			clazz = loadArray(c, NULL, &info);
		}
		else
		{
			clazz = loadClass(c, NULL, &info);
		}
		if( clazz )
		{
			/*
			 * do not process to COMPLETE, it will run static
			 * initializers prematurely!
			 */
			if( processClass(clazz, CSTATE_LINKED, &info) ==
			    false )
			{
				error = 1;
			}
		}
		else
		{
			error = 1;
		}
		utf8ConstRelease(c);
	}
	else
	{
		postOutOfMemory(&info);
		error = 1;
	}

	gc_free(name);
	
	if( error )
	{
		throwError(&info);
	}
	return( clazz );
}

