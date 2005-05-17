/*
 * java.lang.System.c
 *
 * Copyright (c) 1996, 1997, 1998, 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 * Copyright (c) 2003
 *	Mark J. Wielaard <mark@klomp.org>
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "config-io.h"
#include "config-mem.h"
#if defined(HAVE_SYS_UTSNAME_H)
#include <sys/utsname.h>
#endif
#if defined(HAVE_PWD_H)
#include <pwd.h>
#endif
#include <sys/time.h>
#include <time.h>
#include <ctype.h>
#include "classMethod.h"
#include "gtypes.h"
#include "object.h"
#include "constants.h"
#include "access.h"
#include "baseClasses.h"
#include "stringSupport.h"
#include "support.h"
#include "external.h"
#include "soft.h"
#include "debug.h"
#include "exception.h"
#include "system.h"
#include "defs.h"
#include "java_io_InputStream.h"
#include "java_io_PrintStream.h"
#include "java_lang_VMSystem.h"
#include "java_lang_Throwable.h"
#include <native.h>
#include <jni.h>

#include <locale.h>

extern void printStackTrace(struct Hjava_lang_Throwable*,
	struct Hjava_lang_Object*, int);

jint
java_lang_VMSystem_identityHashCode(struct Hjava_lang_Object* o)
{
  /* Hash code is object's address */
  return ((jint)(jword)o);
}

/*
 * See java/lang/System.java for info on these two routines.
 */
void
java_lang_System_debug(struct Hjava_lang_String *str)
{
	char *s;

	s = checkPtr(stringJava2C(str));
	dprintf("%s\n", s);
	gc_free(s);
}

void
java_lang_System_debugE(struct Hjava_lang_Throwable *t)
{
	Hjava_lang_String *msg;
	const char *cname;
	char *s;

	cname = CLASS_CNAME(OBJECT_CLASS(&t->base));
	msg = unhand(t)->detailMessage;

	if (msg) {
		s = checkPtr(stringJava2C(msg));
		dprintf("%s: %s\n", cname, s);
		gc_free(s);
	} else {
		dprintf("%s\n", cname);
	}
	printStackTrace(t, NULL, 1);
}

void
java_lang_VMSystem_arraycopy0(struct Hjava_lang_Object* src,
			      jint srcpos,
			      struct Hjava_lang_Object* dst,
			      jint dstpos,
			      jint len)
{
	char* in; 	 
	char* out; 	 
	int elemsz; 	 
	Hjava_lang_Class* sclass; 	 
	Hjava_lang_Class* dclass;

	sclass = OBJECT_CLASS(src); 	 
	dclass = OBJECT_CLASS(dst);

	sclass = CLASS_ELEMENT_TYPE(sclass); 	 
	dclass = CLASS_ELEMENT_TYPE(dclass); 	 
	elemsz = TYPE_SIZE(sclass); 	 

	len *= elemsz; 	 
	srcpos *= elemsz; 	 
	dstpos *= elemsz; 	 

	in = &((char*)ARRAY_DATA(src))[srcpos]; 	 
	out = &((char*)ARRAY_DATA(dst))[dstpos];

	if (sclass == dclass) {
#if defined(HAVE_MEMMOVE) 	 
		memmove((void*)out, (void*)in, (size_t)len); 	 
#else 	 
		/* Do it ourself */ 	 
#if defined(HAVE_MEMCPY) 	 
		if (src != dst) { 	 
			memcpy((void*)out, (void*)in, (size_t)len); 	 
		} else 	 
#endif 	 
		if (out < in) { 	 
			/* Copy forwards */ 	 
			for (; len > 0; len--) { 	 
				*out++ = *in++; 	 
			} 	 
		} else { 	 
			/* Copy backwards */ 	 
			out += len; 	 
			in += len; 	 
			for (; len > 0; len--) { 	 
				*--out = *--in; 	 
			} 	 
		} 	 
#endif 	 
	} else {
		if (CLASS_IS_PRIMITIVE(sclass) || CLASS_IS_PRIMITIVE(dclass)) {
		  Hjava_lang_Throwable* asexc;
		  const char *stype = CLASS_CNAME(sclass);
		  const char *dtype = CLASS_CNAME(dclass);
		  char *b;
#define _FORMAT "incompatible array types `%s' and `%s'"
		  b = checkPtr(KMALLOC(strlen(stype)+strlen(dtype)+strlen(_FORMAT)));
		  sprintf(b, _FORMAT, stype, dtype);
#undef _FORMAT
		  asexc = ArrayStoreException(b);
		  KFREE(b);
		  throwException(asexc);
		}

		for (; len > 0; len -= sizeof(Hjava_lang_Object*)) { 	 
			Hjava_lang_Object* val = *(Hjava_lang_Object**)in; 	 
			if (val != 0 && !instanceof(dclass, OBJECT_CLASS(val))) { 	 
			  Hjava_lang_Throwable* asexc;
			  const char *vtype = CLASS_CNAME(OBJECT_CLASS(val));
			  const char *atype = CLASS_CNAME(dclass);
			  char *b;
#define _FORMAT "can't store `%s' in array of type `%s'"
			  b = checkPtr(KMALLOC(strlen(vtype)+strlen(atype)+strlen(_FORMAT)));
			  sprintf(b, _FORMAT, vtype, atype);
#undef _FORMAT
			  asexc = ArrayStoreException(b);
			  KFREE(b);
			  throwException(asexc);
			}
			*(Hjava_lang_Object**)out = val; 	 
			in += sizeof(Hjava_lang_Object*); 	 
			out += sizeof(Hjava_lang_Object*); 	 
		}
	}
}
