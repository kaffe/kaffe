/* gc-block.c
 * Store descriptions of gc-blocks in a seperate array.  Use mmap to
 * grow array of heap blocks and array of struct gc_blocks in lock step.
 *
 * Copyright (c) 1998 The University of Utah.
 *
 * Copyright (c) 1998
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file.
 *
 * Contributed by the Flux Research Group at the University of Utah.
 */

#include "debug.h"
#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "gtypes.h"
#include "baseClasses.h"
#include "support.h"
#include "mem/gc-mem.h"
#include "mem/gc-block.h"
#include "locks.h"
#include <sys/mman.h>
#include <limits.h>
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef malloc
#undef malloc
#endif

#if !defined(HAVE_MPROTECT) || !defined(DEBUG)
#define mprotect(A,L,P)
#define ALL_PROT
#define NO_PROT
#else
/* In a sense, this is backwards. */
#define ALL_PROT PROT_READ|PROT_WRITE|PROT_EXEC
#define NO_PROT  PROT_NONE
#endif

/* Get some page-aligned memory from the system. */
static uintp
pagealloc(size_t size)
{
	void* ptr;

#define	CHECK_OUT_OF_MEMORY(P)	if ((P) == 0) return 0;

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
		DBG(GCSYSALLOC,
		    dprintf("unaligned sbrk %p, missed %d bytes\n",
			    ptr, missed));
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
	return ((uintp) ptr);
}

/* Free memory allocated with pagealloc */
static void pagefree(uintp base, size_t size)
{
#ifdef HAVE_SBRK
	sbrk(-size);
#else
	/* it must have been allocated with memalign, valloc or malloc */
	free((void *)base);
#endif
}

size_t gc_heap_base;
size_t gc_block_base;
uintp gc_heap_range;

/* Mark this block as in-use */
void gc_block_add(gc_block *b)
{
	b->next = GCBLOCK_LIVE;
	mprotect(GCBLOCK2BASE(b), b->size, ALL_PROT);
}

/* Mark this block as free */
void gc_block_rm(gc_block *b)
{
	b->next = 0;
	mprotect(GCBLOCK2BASE(b), b->size, NO_PROT);
}

/* Initialize the gc_block interface.  This function must be called
   after all the gc-mem variables are set up. */
void gc_block_init()
{ }

/* Allocate size bytes of heap memory, and return the corresponding
   gc_block *. */
void *
gc_block_alloc(size_t size)
{
	int size_pg = (size>>gc_pgbits);
	static int n_live = 0;	/* number of pages in java heap */
	static int nblocks;	/* number of gc_blocks in array */
	uintp heap_addr;
	static uintp last_addr;

	if (!gc_block_base) {
		nblocks = (gc_heap_limit>>gc_pgbits);

		nblocks += nblocks/4;
		gc_block_base = (uintp) malloc(nblocks * sizeof(gc_block));
		if (!gc_block_base) return 0;
		memset((void *)gc_block_base, 0, nblocks * sizeof(gc_block));
	}

	heap_addr = pagealloc(size);

	if (!heap_addr) return 0;
	
	if (!gc_heap_base) {
		gc_heap_base = heap_addr;
	}

	if (GCMEM2BLOCK(heap_addr + size)
	    > ((gc_block *)gc_block_base) + nblocks
	    || heap_addr < gc_heap_base) {
		uintp old_blocks = gc_block_base;
		int onb = nblocks;
		int min_nb;	/* minimum size of array to hold heap_addr */
		static timespent growtime;

		startTiming(&growtime, "gc block realloc");
		/* Pick a new size for the gc_block array.  Remeber,
		   malloc does not simply grow a memory segment.

		   We can extrapolate how many gc_blocks we need for
		   the entire heap based on how many heap pages
		   currently fit in the gc_block array.  But, we must
		   also make sure to allocate enough blocks to cover
		   the current allocation */
		nblocks = (nblocks * (gc_heap_limit >> gc_pgbits))
			/ n_live;
		if (heap_addr < gc_heap_base) 
			min_nb = nblocks
			  + ((gc_heap_base - heap_addr) >> gc_pgbits);
		else
			min_nb = ((heap_addr + size) - gc_heap_base) >>
			  gc_pgbits;
		nblocks = MAX(nblocks, min_nb);
		DBG(GCSYSALLOC,
		    dprintf("growing block array from %d to %d elements\n",
			    onb, nblocks));

		LOCK();
		gc_block_base = (uintp) realloc((void *) old_blocks,
						nblocks * sizeof(gc_block));
		if (!gc_block_base) {
			/* roll back this call */
			pagefree(heap_addr, size);
			gc_block_base = old_blocks;
			nblocks = onb;
			UNLOCK();
			return 0;
		}

		/* If the array's address has changed, we have to fix
		   up the pointers in the gc_blocks, as well as all
		   external pointers to the gc_blocks.  We can only
		   fix gc_prim_freelist and the size-freelist array.
		   There should be no gc_block *'s on any stack
		   now. */ 
		if (gc_block_base != old_blocks) {
			extern gc_block *gc_prim_freelist;
			int i;
			gc_block *b = (void *) gc_block_base;
			uintp delta = gc_block_base - old_blocks;
#define R(X) if (X) ((uintp) (X)) += delta

			DBG(GCSYSALLOC,
			    dprintf("relocating gc_block array\n"));
			for (i = 0; i < onb; i++) {
				R(b[i].nfree);
				if (b[i].next != GCBLOCK_LIVE) R(b[i].next);
			}
			memset(b + onb, 0,
			       (nblocks - onb) * sizeof(gc_block));

			R(gc_prim_freelist);

			for (i = 0; freelist[i].list != (void *) -1; i++) 
				R(freelist[i].list);
#undef R
		}
		UNLOCK();
		stopTiming(&growtime);
	}
	n_live += size_pg;
	last_addr = MAX(last_addr, heap_addr + size);
	gc_heap_range = last_addr - gc_heap_base;
	DBG(GCSYSALLOC, dprintf("%d unused bytes in heap addr range\n",
				gc_heap_range - gc_heap_total));
	mprotect((void *) heap_addr, size, NO_PROT);
	return GCMEM2BLOCK(heap_addr);
}
