/* gc-block.c
 * Store descriptions of gc-blocks in blocks themselves.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "debug.h"
#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "gtypes.h"
#include "mem/gc-mem.h"
#include "mem/gc-block.h"

/* Why isn't this in exception.h? */
extern void throwOutOfMemory(void);

gc_block* gc_objecthash[GC_OBJECT_HASHSIZE];

/* Mark this block as in-use */
void gc_block_add(gc_block *ptr)
{
	int hidx = GC_OBJECT_HASHIDX(ptr);
	ptr->next = gc_objecthash[hidx];
	gc_objecthash[hidx] = ptr;
}

/* Mark this block as free */
void gc_block_rm(gc_block *mem)
{
	/* Remove from object hash */
	gc_block *lptr;
	int hidx = GC_OBJECT_HASHIDX(mem);

	if (gc_objecthash[hidx] == mem) {
		gc_objecthash[hidx] = mem->next;
	}
	else {
		for (lptr = gc_objecthash[hidx]; lptr->next != 0;
		     lptr = lptr->next) {
			if (lptr->next == mem) {
				lptr->next = mem->next;
				return;
			}
		}
		assert("Failed to find freeing block in object hash" == 0);
	}
}

void gc_block_init() { }	/* there is nothing to see here */

/* Allocate size bytes of heap memory, and return the corresponding
   gc_block *. */
void*
gc_block_alloc(size_t size)
{
	void* ptr;

#define	CHECK_OUT_OF_MEMORY(P)	if ((P) == 0) throwOutOfMemory();

#if defined(HAVE_SBRK)

	/* Our primary choice for basic memory allocation is sbrk() which
	 * should avoid any unsee space overheads.
	 */
	for (;;) {
		int missed;
		ptr = sbrk(size);
		if (ptr == (void*)-1) {
			ptr = 0;
			break;
		}
		if ((uintp)ptr % gc_pgsize == 0) {
			break;
		}
		missed = gc_pgsize - ((uintp)ptr % gc_pgsize);
		sbrk(-size + missed);
	}
	CHECK_OUT_OF_MEMORY(ptr);

#elif defined(HAVE_MEMALIGN)

        ptr = memalign(gc_pgsize, size);
	CHECK_OUT_OF_MEMORY(ptr);

#elif defined(HAVE_VALLOC)

        ptr = valloc(size);
	CHECK_OUT_OF_MEMORY(ptr);

#else

	/* Fallback ...
	 * Allocate memory using malloc and align by hand.
	 */
	size += gc_pgsize;
        ptr = malloc(size);
	CHECK_OUT_OF_MEMORY(ptr);
	ptr = (void*)((((uintp)ptr) + gc_pgsize - 1) & -gc_pgsize);

#endif

	return (ptr);
}

