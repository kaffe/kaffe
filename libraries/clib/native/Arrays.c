
/*
 * java.util.Arrays.c
 *
 * Copyright (c) 1999
 *	Archie L. Cobbs. All rights reserved.
 * Copyright (c) 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * Author: Archie L. Cobbs <archie@whistle.com>
 */

#include "config.h"
#include "config-std.h"
#include "native.h"
#include "gtypes.h"
#include "fp.h"
#include "support.h"
#include "java_util_Comparator.h"
#include "java_util_Arrays.h"

/*
 * Sorting functions
 *
 * We rely on libc routines to do the sorting because they are already
 * highly optimized and debugged.
 *
 * Note that these routines assume the bounds checking has already been
 * done in Java.
 */

static int
cmpByte(const void *p1, const void *p2)
{
	return (int)*((const jbyte *) p1) - (int)*((const jbyte *) p2);
}

static int
cmpChar(const void *p1, const void *p2)
{
	return (int)*((const jchar *) p1) - (int)*((const jchar *) p2);
}

static int
cmpDouble(const void *p1, const void *p2)
{
	const jlong bits1 = doubleToLong(*((const jdouble *) p1));
	const jlong bits2 = doubleToLong(*((const jdouble *) p2));

	return (bits1 == bits2) ? 0 : (bits1 < bits2) ? -1 : 1;
}

static int
cmpFloat(const void *p1, const void *p2)
{
	const jint bits1 = floatToInt(*((const jfloat *) p1));
	const jint bits2 = floatToInt(*((const jfloat *) p2));

	return (bits1 == bits2) ? 0 : (bits1 < bits2) ? -1 : 1;
}

static int
cmpInt(const void *p1, const void *p2)
{
	const jint int1 = *((const jint *) p1);
	const jint int2 = *((const jint *) p2);

	return (int1 == int2) ? 0 : (int1 < int2) ? -1 : 1;
}

static int
cmpShort(const void *p1, const void *p2)
{
	return (int)*((const jshort *) p1) - (int)*((const jshort *) p2);
}

static int
cmpLong(const void *p1, const void *p2)
{
	const jlong long1 = *((const jlong *) p1);
	const jlong long2 = *((const jlong *) p2);

	return (long1 == long2) ? 0 : (long1 < long2) ? -1 : 1;
}

void
java_util_Arrays_sortByte(HArrayOfByte *a, jint fromIndex, jint toIndex)
{
	qsort(&unhand_array(a)->body[fromIndex], (size_t)(toIndex - fromIndex),
		sizeof(*unhand_array(a)->body), cmpByte);
}

void
java_util_Arrays_sortChar(HArrayOfChar *a, jint fromIndex, jint toIndex)
{
	qsort(&unhand_array(a)->body[fromIndex], (size_t)(toIndex - fromIndex),
		sizeof(*unhand_array(a)->body), cmpChar);
}

void
java_util_Arrays_sortDouble(HArrayOfDouble *a, jint fromIndex, jint toIndex)
{
	qsort(&unhand_array(a)->body[fromIndex], (size_t)(toIndex - fromIndex),
		sizeof(*unhand_array(a)->body), cmpDouble);
}

void
java_util_Arrays_sortFloat(HArrayOfFloat *a, jint fromIndex, jint toIndex)
{
	qsort(&unhand_array(a)->body[fromIndex], (size_t)(toIndex - fromIndex),
		sizeof(*unhand_array(a)->body), cmpFloat);
}

void
java_util_Arrays_sortInt(HArrayOfInt *a, jint fromIndex, jint toIndex)
{
	qsort(&unhand_array(a)->body[fromIndex], (size_t)(toIndex - fromIndex),
		sizeof(*unhand_array(a)->body), cmpInt);
}

void
java_util_Arrays_sortShort(HArrayOfShort *a, jint fromIndex, jint toIndex)
{
	qsort(&unhand_array(a)->body[fromIndex], (size_t)(toIndex - fromIndex),
		sizeof(*unhand_array(a)->body), cmpShort);
}

void
java_util_Arrays_sortLong(HArrayOfLong *a, jint fromIndex, jint toIndex)
{
	qsort(&unhand_array(a)->body[fromIndex], (size_t)(toIndex - fromIndex),
		sizeof(*unhand_array(a)->body), cmpLong);
}

/*
 * Sorting object arrays with a Comparator
 *
 * This is less than elegant. We want to use the libc mergesort(), but we
 * get a race condition if we store the Comparator in a static variable.
 * So we allocate a "shadow" array and sort that instead, then copy the
 * object pointers back into the original array.
 */

/* Shadow array element */
struct objcmpinfo {
	struct Hjava_lang_Object *obj;		/* the Object */
	struct Hjava_util_Comparator *cmp;	/* the Comparator  */
};

static int
cmpObject(const void *p1, const void *p2)
{
	const struct objcmpinfo	*const o1 = (const struct objcmpinfo *) p1;
	const struct objcmpinfo	*const o2 = (const struct objcmpinfo *) p2;
	jvalue rtn;

	do_execute_java_method(&rtn, o1->cmp, "compare",
		"(Ljava/lang/Object;Ljava/lang/Object;)I",
		NULL, false, o1->obj, o2->obj);
	return rtn.i;
}

void
java_util_Arrays_sortObject(HArrayOfObject *a, jint fromIndex, jint toIndex, struct Hjava_util_Comparator *c)
{
	int slen = toIndex - fromIndex;
	unsigned int len;
	struct objcmpinfo *ilist;
	errorInfo info;
	unsigned int k;

	if (slen <= 1) {
		return;
	}
	len = slen;

	/* Prepare shadow array */
	ilist = KMALLOC(len * sizeof(*ilist));
	if (ilist == NULL) {
		goto nomem;
	}
	for (k = 0; k < len; k++) {
		ilist[k].obj = unhand_array(a)->body[fromIndex + k];
		ilist[k].cmp = c;
	}

	/* Do the sort */
#ifdef HAVE_MERGESORT
	if (mergesort(ilist, len, sizeof(*ilist), cmpObject) < 0) {
		KFREE(ilist);
		goto nomem;
	}
#else
	qsort(ilist, len, sizeof(*ilist), cmpObject);
#endif

	/* Copy back sorted results */
	for (k = 0; k < len; k++) {
		unhand_array(a)->body[fromIndex + k] = ilist[k].obj;
	}
	KFREE(ilist);
	return;

nomem:
	postOutOfMemory(&info);
	throwError(&info);
}

