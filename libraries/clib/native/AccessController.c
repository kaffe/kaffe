/* AccessController.c -- native VMAccessController methods.
   Copyright (C) 2004  Casey Marshall <csm@gnu.org>
   Original Copyright (C) 1996, 1997 Transvirtual Technologies, Inc.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to the
Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301 USA.  */


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
#include "java_security_VMAccessController.h"
#include "stackTrace.h"
#include "support.h"
#include "stringSupport.h"

/*
 * Returns the call stack of the current thread as a pair of arrays: the
 * class in the stack, and the name of the method on the stack.
 *
 * This method is based on kaffe_lang_ThreadStack, but returns the
 * method names along with the classes.
 */
HArrayOfArray *
java_security_VMAccessController_getStack (void)
{
  stackTraceInfo *info;
  errorInfo einfo;
  int cnt;
  int i;
  HArrayOfObject *classes;
  HArrayOfObject *meths;
  HArrayOfArray *array;

  info = (stackTraceInfo *) buildStackTrace(NULL);
  if (info == NULL)
    {
      postOutOfMemory (&einfo);
      throwError (&einfo);
    }

  cnt = 0;
  for (i = 0; info[i].meth != ENDOFSTACK; i++)
    {
      if (info[i].meth != NULL && info[i].meth->class != NULL)
	cnt++;
    }

  array = (HArrayOfArray *) AllocObjectArray (2, "[Ljava/lang/Object;", NULL);
  classes = (HArrayOfObject *) AllocObjectArray (cnt, "Ljava/lang/Class;", NULL);
  meths = (HArrayOfObject *) AllocObjectArray (cnt, "Ljava/lang/String;", NULL);

  cnt = 0;
  for (i = 0; info[i].meth != ENDOFSTACK; i++)
    {
      if (info[i].meth != NULL && info[i].meth->class != NULL)
	{
	  unhand_array(classes)->body[cnt] = (Hjava_lang_Object *) info[i].meth->class;
	  unhand_array(meths)->body[cnt] = (Hjava_lang_Object *) utf8Const2Java (info[i].meth->name);
	  cnt++;
	}      
    }

  unhand_array(array)->body[0] = (Hjava_lang_Object *) classes;
  unhand_array(array)->body[1] = (Hjava_lang_Object *) meths;
  return array;
}
