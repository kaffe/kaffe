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
#include "../../../kaffe/kaffevm/gtypes.h"
#include "../../../kaffe/kaffevm/file.h"
#include "../../../kaffe/kaffevm/readClass.h"
#include "../../../kaffe/kaffevm/constants.h"
#include "../../../kaffe/kaffevm/access.h"
#include "../../../kaffe/kaffevm/classMethod.h"
#include "../../../kaffe/kaffevm/object.h"
#include "../../../kaffe/kaffevm/locks.h"
#include "../../../kaffe/kaffevm/itypes.h"
#include "../../../kaffe/kaffevm/support.h"
#include "../../../kaffe/kaffevm/stringSupport.h"
#include "../../../kaffe/kaffevm/baseClasses.h"
#include "../../../kaffe/kaffevm/exception.h"
#include "kaffe_lang_PrimordialClassLoader.h"
#include <native.h>
#include "defs.h"

/*
 * Load a system class.
 */
struct Hjava_lang_Class*
kaffe_lang_PrimordialClassLoader_findClass0(Hkaffe_lang_PrimordialClassLoader* this, Hjava_lang_String* jStr)
{
	Hjava_lang_Class *clazz = 0;
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
			clazz = loadArray(c, 0, &info);
		}
		else
		{
			clazz = loadClass(c, 0, &info);
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

