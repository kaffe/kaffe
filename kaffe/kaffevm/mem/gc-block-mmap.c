/* gc-block.c
 * Store descriptions of gc-blocks in a seperate array.  Use mmap to
 * grow array of heap blocks and array of struct gc_blocks in lock step.
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
#include "slib.h"
#include <sys/mman.h>
#include <limits.h>
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#if defined(MAP_ANONYMOUS) && !defined(MAP_ANON)
#define MAP_ANON MAP_ANONYMOUS
#endif

#ifdef MAP_ANON
#define MAP_FLAGS MAP_PRIVATE|MAP_ANON
#else
#define MAP_FLAGS MAP_PRIVATE
#endif


/* Why isn't this in exception.h? */
extern void throwOutOfMemory(void);

#ifndef gc_heap_base
size_t gc_heap_base;
size_t gc_block_base;
#endif

static int dev_zero;

/* Mark this block as in-use */
void gc_block_add(gc_block *b)
{
	b->next = GCBLOCK_LIVE;
}

/* Mark this block as free */
void gc_block_rm(gc_block *b)
{
	b->next = 0;
}

/* Initialize the gc_block interface.  This function must be called
   after all the gc-mem variables are set up. */
void gc_block_init()
{
	extern char *optarg;
	int incr = gc_heap_allocation_size;
	int heap_cnt = gc_heap_limit / incr;
	int hdr_cnt = ((gc_heap_limit>>gc_pgbits)*sizeof(gc_block) + incr - 1)
		/ incr;
	char *this_range;
	char *shlib_addr;
	int this_range_cnt = 0;

#ifndef MAP_ANON
	dev_zero = open("/dev/zero", O_RDWR);
	assert(dev_zero >= 0);
#else
	dev_zero = -1;
#endif

#ifndef gc_heap_base
	/* If this port uses fixed address ranges for heap and page
	 * headers, our work here is done.  Otherwise, find address
	 * ranges we can expect the system will not use.
	 */
	/* Leave at least incr-many bytes for C heap growth, and leave
	   gc_heap_limit many bytes for later shlib loading. (We
	   assume that shared libraries will end up more or less
	   contigous in memory. */
	this_range = (char *) ((((uintp) sbrk(0)) + 2*incr - 1) & -incr);
	LIBRARYADDR(shlib_addr);

	DBG(GCSYSALLOC,
	    dprintf("Finding address ranges using %d blocks "
		    "(need %d for heap, %d for headers)\n"
		    "starting scan at %p (heap end = %p)\n"
		    "pointers to skip: shared-text=%p, stack=%p\n"
		    "Used block map follows:\n",
		    incr, heap_cnt, hdr_cnt, this_range, sbrk(0),
		    shlib_addr, &incr));

	/* Keep scanning until the pointer wraps, or we allocate both
	   address ranges. */
	for ( ; this_range + incr > this_range
		      && !(gc_heap_base && gc_block_base); 
	      this_range += incr) {
		char *ret = mmap(this_range, incr,
				 PROT_READ|PROT_WRITE,
				 MAP_FLAGS, dev_zero, 0);
		if (ret != (void *) -1)	munmap(ret, incr);

		if (this_range_cnt >= heap_cnt) {
			gc_heap_base = (uintp)(this_range
					       -  incr*this_range_cnt);
			this_range_cnt -= heap_cnt;
			heap_cnt = INT_MAX;
		}
		/* Only grab this address range for headers if it
		   cannot be used for the heap. */
		if ((heap_cnt == INT_MAX || ret != this_range)
		    && this_range_cnt >= hdr_cnt) {
			gc_block_base = (uintp)(this_range
						- incr*this_range_cnt);
			this_range_cnt -= hdr_cnt;
			hdr_cnt = INT_MAX;
		}

		/* ignore at least gc_heap_limit many bytes after the
		   start of shared library text, the after start of
		   shared library data and before the main stack. */
		if (ret != this_range
		    || (shlib_addr && ret > shlib_addr
			&& ret < shlib_addr + gc_heap_limit)
		    || ((ret > ((char *) &incr) - gc_heap_limit)
			&& ret < ((char *) &incr))) {
			DBG(GCSYSALLOC, dprintf("X"));
			this_range_cnt = 0;
		} else {
			DBG(GCSYSALLOC, dprintf(" "));
			this_range_cnt++;
		}
	}
	if (!(gc_heap_base && gc_block_base)) {
		fprintf(stderr, "cannot find free address space\n");
		exit(-1);
	}
	DBG(GCSYSALLOC,
	    dprintf("\n%#x bytes of heap starting at %p\n",
		    gc_heap_limit, gc_heap_base);
	    dprintf("%d gc_blocks starting at %p\n",
		    gc_heap_limit/gc_pgsize, gc_block_base));
#endif
}

/* Allocate size bytes of heap memory, and return the corresponding
   gc_block *. */
void *
gc_block_alloc(size_t size)
{
	static void *next_hdr;
	static void *next_base;
	void *ret;
	uintp n_next_hdr;
	void *got_addr;

	if (!next_base) {
		next_base = (void *) gc_heap_base;
		next_hdr = (void *) gc_block_base;
	}

	ret = GCMEM2BLOCK(next_base);
	got_addr = mmap(next_base, size, PROT_READ|PROT_WRITE|PROT_EXEC,
		   MAP_FLAGS, dev_zero, 0);
	if (got_addr == (void *) -1 || !got_addr) return 0;
	assert(got_addr == next_base);
	next_base += size;

	/* Return the gc_block for the newly allocated memory, and
	   allocate gc_blocks to cover the range:  Round the first
	   unneeded gc_block addr up to a page boundary. */

	/* compute end of header allocation */
	n_next_hdr = (uintp) GCMEM2BLOCK(next_base);
	n_next_hdr = (n_next_hdr + gc_pgsize - 1) & -gc_pgsize;

	got_addr = mmap(next_hdr, n_next_hdr - (uintp) next_hdr,
		    PROT_READ|PROT_WRITE, MAP_FLAGS, dev_zero, 0);
	if (got_addr == (void *) -1 || !got_addr) {
		next_base -= size;
		munmap(next_base, size);
		return 0;
	}
	assert(got_addr == next_hdr);
	next_hdr = (void *) n_next_hdr;
	return ret;
}
