/* constpool.h
 * Manage the constant pool.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __jit_constpool_h
#define __jit_constpool_h

#define CPint		1
#define CPlong		2
#define CPref		3
#define CPfloat		4
#define CPdouble	5
#define	CPstring	6

typedef struct _constpool {
	struct _constpool* next;
	uintp		   at;
	union _constpoolval {
		jint	   i;
		jlong	   l;
		void*	   r;
		float	   f;
		double	   d;
	} val;
} constpool;

#define ALLOCCONSTNR	32

constpool* KaffeJIT_newConstant(int type, ...);
void KaffeJIT_establishConstants(void *at);
void KaffeJIT_resetConstants(void);
uint32 KaffeJIT_getNumberOfConstants(void);

#endif
