/* constpool.c
 * Manage the constant pool.
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
#include "constpool.h"
#include "gc.h"
#include "thread.h"
#include "jthread.h"
#include "debug.h"

#include <stdarg.h>

static constpoolchunk* poolchunks;
constpool* firstConst;
constpool* lastConst;
constpool* currConst;
uint32 nConst;

constpool*
newConstant(int type, ...)
{
	union _constpoolval val;
	constpool *c;
	va_list arg;

	assert(type > CP_min);
	assert(type < CP_max);

	bzero(&val, sizeof(val));
	va_start(arg, type);
	switch (type) {
	case CPint:
		val.i = va_arg(arg, jint);
		break;
	case CPlong:
		val.l = va_arg(arg, jlong);
		break;
	case CPref:
		val.r = va_arg(arg, void*);
		break;
	case CPfloat:
		/* Remember ISO rules about argument widening.  */
		val.f = (float)va_arg(arg, double);
		break;
	case CPdouble:
		val.d = va_arg(arg, double);
		break;
	case CPstring:
		val.r = va_arg(arg, void*);
		break;
	}
	va_end(arg);

	if (type != CPlabel) {
		/* Find out if we've allocated this constant (or its bit
		   equivalent) before, and if so, reuse it.  */
		for (c = firstConst; c != currConst; c = c->next) {
			if (memcmp(&c->val, &val, sizeof(val)) == 0) {
				return (c);
			}
		}
	}
	else {
		c = currConst;
	}

	if (c == NULL) {
		constpoolchunk *cpc;
		int i;

		/* Allocate chunk of constpool elements */
		cpc = gc_malloc(sizeof(constpoolchunk), KGC_ALLOC_JIT_CONST);
		/* XXX Ack! */
		assert(cpc != 0);

		cpc->next = poolchunks;
		poolchunks = cpc;

		c = &cpc->data[0];
		
		/* Attach to current chain */
		if (lastConst == 0) {
			firstConst = c;
		}
		else {
			lastConst->next = c;
		}
		lastConst = &cpc->data[ALLOCCONSTNR-1];

		/* Link elements into list */
		for (i = 0; i < ALLOCCONSTNR - 1; i++) {
			cpc->data[i].next = &cpc->data[i+1];
		}
	}

	c->type = type;
	c->val = val;
	currConst = c->next;
	nConst++;

	return (c);
}

const char *constpoolTypeNames[] = {
	"<invalid>",
	"int",
	"long",
	"ref",
	"float",
	"double",
	"string",
	"label",
};

#if defined(NDEBUG) || !defined(KAFFE_VMDEBUG)
static void
printConstant(FILE *file, constpool *cp)
{
	fprintf(file, "%08x: (%s) ", cp->at, constpoolTypeNames[cp->type]);
	switch( cp->type )
	{
	case CPint:
		fprintf(file, "%d\t0x%x\n", cp->val.i, cp->val.i);
		break;
	case CPlong:
		fprintf(file, "%qd\t0x%qx\n", cp->val.l, cp->val.l);
		break;
	case CPref:
		fprintf(file, "%p\t%p\n", cp->val.r, cp->val.r);
		break;
	case CPfloat:
		fprintf(file, "%f\t0x%x\n", cp->val.f, (int)cp->val.f);
		break;
	case CPdouble:
		fprintf(file, "%f\t0x%qx\n", cp->val.d, (long long)cp->val.d);
		break;
	case CPstring:
		fprintf(file, "%s\t%p\n",
			(char *)cp->val.r, cp->val.r);
		break;
	case CPlabel:
		fprintf(file, "%p\t%p\n", cp->val.r, cp->val.r);
		break;
	default:
		assert(0);
		break;
	}
}
#endif /* defined(NDEBUG) || !defined(KAFFE_VMDEBUG) */

void
establishConstants(void *at)
{
	constpool *c;

	assert(at != 0);

DBG(MOREJIT, fprintf(stderr, "Method Constant Pool:\n"));
	for (c = firstConst; c != currConst; c = c->next) {
		c->at = (uintp)at;
		*(union _constpoolval*)at = c->val;
DBG(MOREJIT,	printConstant(stderr, c));
		at = (void*)(((uintp)at) + sizeof(c->val));
	}
}

void
resetConstants(void)
{
	currConst = firstConst;
	nConst = 0;
	while( (poolchunks != NULL) && (poolchunks->next != NULL) )
	{
		constpoolchunk *cpc = poolchunks;

		poolchunks = cpc->next;
		gc_free(cpc);
	}
	if( poolchunks != NULL )
	{
		poolchunks->data[ALLOCCONSTNR - 1].next = NULL;
		lastConst = &poolchunks->data[ALLOCCONSTNR - 1];
	}
}
