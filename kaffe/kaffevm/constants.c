/*
 * constants.c
 * Constant management.
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
#include "config-hacks.h"
#include "gtypes.h"
#include "access.h"
#include "object.h"
#include "constants.h"
#include "object.h"
#include "errors.h"
#include "file.h"
#include "exception.h"
#include "classMethod.h"
#include "utf8const.h"
#include "debug.h"

/*
 * XXX move into readClass.c
 */
/**
 * Check if the words making up a jdouble need to be 
 * switched around. Necessary for some ARM systems.
 */
static
jboolean check_if_need_to_switch_words_in_jdouble(void) {
  jvalue val; 

  /* -0.0 as an IEEE754 double is 0x80000000000000LL, i.e. < 0LL.
   * If the words in the double are switched around, then the 
   * bit pattern will be 0x0000000080000000LL, i.e. > 0LL. 
   */
  val.d = -0.0;
  return val.j > 0;
}

/*
 * Read in constant pool from opened file.
 */
bool
readConstantPool(Hjava_lang_Class* this, classFile* fp, errorInfo *einfo)
{
	constants* info = CLASS_CONSTANTS (this);
	ConstSlot* pool;
	u1* tags;
	unsigned int i;
	unsigned int j;
	u1 type;
	u2 len;
	u2 d2, d2b;
	u4 d4, d4b;
	u2 poolsize;
	const char* className = "unknown";  /* CLASS_CNAME(this) won't work until after constant pool is read... */


	if (!checkBufSize(fp, 2, className, einfo))
		return false;

	readu2(&poolsize, fp);
	DBG(READCLASS,
	    dprintf("constant_pool_count=%d\n", poolsize);
		);

	/* Allocate space for tags and data */
	pool = gc_malloc((sizeof(ConstSlot) + sizeof(u1)) * poolsize,
			 KGC_ALLOC_CONSTANT);
	if (!pool) {
		postOutOfMemory(einfo);
		return false;
	}
	tags = (u1*)&pool[poolsize];
	info->data = pool;
	info->tags = tags;
	info->size = poolsize;

	pool[0] = 0;
	tags[0] = CONSTANT_Unknown;
	for (i = 1; i < info->size; i++) {
	        const char * str;

		if (! checkBufSize(fp, 1, className, einfo))
			goto fail;

		readu1(&type, fp);
		DBG(READCLASS,
		    dprintf("Constant[%d] type %d\n", i, type);
			);
		tags[i] = type;

		switch (type) {
		case CONSTANT_Utf8:
			if (! checkBufSize(fp, 2, className, einfo))
				goto fail;

			readu2(&len, fp);

			if (! checkBufSize(fp, len, className, einfo))
				goto fail;

			str = (const char *) fp->cur;

			if (!utf8ConstIsValidUtf8(str, len)) {
				postExceptionMessage(einfo,
					JAVA_LANG(ClassFormatError), 
					"Invalid UTF-8 constant");
				goto fail;
			}
			pool[i] = (ConstSlot) utf8ConstNew(str, len);
			if (!pool[i]) {
				postOutOfMemory(einfo);
				goto fail;
			}
			seekm(fp, len);
			break;
		case CONSTANT_Class:
		case CONSTANT_String:
			if (! checkBufSize(fp, 2, className, einfo))
				goto fail;
			readu2(&d2, fp);
			if ( (d2 == 0) || (d2 >= info->size) )
			{
				postExceptionMessage(
					einfo,
					JAVA_LANG(ClassFormatError),
					"%s (Invalid constant pool index: %d)",
					className,
					d2);
				goto fail;
			}
			pool[i] = d2;
			break;

		case CONSTANT_Fieldref:
		case CONSTANT_Methodref:
		case CONSTANT_InterfaceMethodref:
		case CONSTANT_NameAndType:
			if (! checkBufSize(fp, 4, className, einfo))
				goto fail;
			readu2(&d2, fp);
			readu2(&d2b, fp);
			pool[i] = (d2b << 16) | d2;
			break;

		case CONSTANT_Integer:
		case CONSTANT_Float:
			if (! checkBufSize(fp, 4, className, einfo))
				goto fail;
			readu4(&d4, fp);
			pool[i] = d4;
			break;

		case CONSTANT_Long:
			if (! checkBufSize(fp, 8, className, einfo))
				goto fail;
			readu4(&d4, fp);
			readu4(&d4b, fp);
#if SIZEOF_VOID_P == 8
			pool[i] = WORDS_TO_LONG(d4, d4b);
			i++;
			pool[i] = 0;
#else
			pool[i] = d4;
			i++;
			pool[i] = d4b;
#endif /* SIZEOF_VOID_P == 8 */
			tags[i] = CONSTANT_Unknown;
			break;

		case CONSTANT_Double:
			if (! checkBufSize(fp, 8, className, einfo))
				goto fail;
			readu4(&d4, fp);
			readu4(&d4b, fp);

#if SIZEOF_VOID_P == 8
			if(check_if_need_to_switch_words_in_jdouble())
			  pool[i] = WORDS_TO_LONG(d4b, d4);
			else
			  pool[i] = WORDS_TO_LONG(d4, d4b);
			i++;
			pool[i] = 0;
#else
			if (check_if_need_to_switch_words_in_jdouble()) {
			  pool[i] = d4b;
			  i++;
			  pool[i] = d4;
			}
			else {
			  pool[i] = d4;
			  i++;
			  pool[i] = d4b;
			}
#endif /* SIZEOF_VOID_P == 8 */
			tags[i] = CONSTANT_Unknown;
			break;

		default:
			postExceptionMessage(einfo, 
				JAVA_LANG(ClassFormatError), 
					     "Invalid constant type %d in class",
					     type);
fail:
			info->size = 0;
			for (j = 0 ; j < i; ++j) {
				if (tags[j] == CONSTANT_Utf8) {
					utf8ConstRelease((Utf8Const*)pool[j]);
				}
			}
			return false;
		}
	}

	/* Perform some constant pool optimisations to allow for the
	 * use of pre-compiled classes.
	 */
	for (i = 1; i < info->size; i++) {
		switch (info->tags[i]) {
		case CONSTANT_Class:
		case CONSTANT_String:
			j = CLASS_NAME(i, info);
			if (info->tags[j] == CONSTANT_Utf8) {
				/* Rewrite so points directly at string */
				info->data[i] = info->data[j];
				utf8ConstAddRef(WORD2UTF(info->data[j]));
			}
			else {
				/* Set this tag so it will generate an error
				 * during verification.
				 */
				info->tags[i] = CONSTANT_Error;
			}
			break;

		default:
			/* Ignore the rest */
			break;
		}
	}
	return true;
}
