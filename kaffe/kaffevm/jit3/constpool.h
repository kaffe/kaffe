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

/*
 * The per-method constant pool is used to store large constants used by a
 * method that cannot be inlined in the instruction stream.  For example, on a
 * RISC CPU, like the PowerPC, only 16 bit immediates can be stored in the
 * instruction stream.  Therefore, we prepend a block of constants to the
 * generated code so that the method can reference them using pc-relative
 * addressing.
 */


/*
 * Constant pool value types:
 *   CPint - A jint
 *   CPlong - A jlong
 *   CPref - A void *
 *   CPfloat - A float
 *   CPdouble - A double
 *   CPstring - A char * XXX ???
 *   CPlabel - A void * XXX ???
 */
enum {
	CP_min,
	
	CPint,
	CPlong,
	CPref,
	CPfloat,
	CPdouble,
	CPstring,
	CPlabel,
	
	CP_max,
};

/*
 * The constpool structure is a linked list node that is used to track the
 * constants that will be placed in the block.
 *
 * next - Link to the next constpool in the list.
 * at - The location of the constant in the constpool.  This value is not
 *   generated until establishConstants() is called, which is after the
 *   instructions have been generated, but before labels have been linked.
 * type - The type of constant, needs to be one of the above CP* values.
 * val - The constant's value.
 */
typedef struct _constpool {
	struct _constpool* next;
	uintp		   at;
	int		   type;
	union _constpoolval {
		jint	   i;
		jlong	   l;
		void*	   r;
		float	   f;
		double	   d;
	} val;
} constpool;

#define ALLOCCONSTNR	32

/*
 * These variables track the pool of constpool objects allocated by the jitter.
 * The set of valid constpool objects is the sublist between firstConst and
 * currConst, exclusive.
 */
extern constpool* firstConst;
extern constpool* lastConst;
extern constpool* currConst;

/* The number of active constpool objects. */
extern uint32 nConst;

/*
 * Allocate a new constpool object and link it in to the global list.
 *
 * type - Type of value to store in the constpool.
 * ... - The value to store.
 *
 * Example:
 *   newConstant(CPint, 256);
 */
constpool* newConstant(int type, ...);

/*
 * Copy the list of constant values to the given location and update the "at"
 * values in the constpool objects.
 *
 * at - The location of the constant pool for the current method.
 */
void establishConstants(void *at);

/*
 * Reset the constant pool global data structures for a new method.
 */
void resetConstants(void);

#endif
