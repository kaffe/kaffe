/*
 * memorySamples.c
 * Routines for tracking locations in memory when profiling
 *
 * Copyright (c) 2000 University of Utah and the Flux Group.
 * All rights reserved.
 *
 * This file is licensed under the terms of the GNU Public License.
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * Contributed by the Flux Research Group, Department of Computer Science,
 * University of Utah, http://www.cs.utah.edu/flux/
 */

#include "config.h"

#if defined(KAFFE_XPROFILER)

#include <stdlib.h>
#include <string.h>
#include <gtypes.h>

#include "kaffe/jmalloc.h"

#include "memorySamples.h"

/* Bits used to determine the branch */
#define SAMPLE_BIT_COUNT 8
#define SAMPLE_BIT_MASK 0xff

/* Number of branches at a level in the tree */
#define SAMPLE_BRANCH_COUNT 256

/* Number of leaves to store sample counts */
#define SAMPLE_LEAF_COUNT (SAMPLE_BRANCH_COUNT / 2)

/* Address size */
#define SAMPLE_ADDRESS_BYTES SIZEOF_VOID_P
#define SAMPLE_ADDRESS_BITS (SAMPLE_ADDRESS_BYTES * 8)

#define SAMPLE_BRANCH_LEVELS (SAMPLE_ADDRESS_BYTES - 1)

/* Macros for computing branches/leaf indexes */
#define SAMPLE_BRANCH(addr, level) \
	((((jword)addr) >> (SAMPLE_ADDRESS_BITS - (SAMPLE_BIT_COUNT * \
						 ((level) + 1)))) \
	 & SAMPLE_BIT_MASK)
#define SAMPLE_BIN(addr) ((((jword)addr) & 0xfe) >> 1)
/* Set the index for some level in a pointer value */
#define SET_ADDR_LEVEL(addr, level, value) \
	((void *)((((jword)(addr)) & \
		   ~(SAMPLE_BIT_MASK << \
		     (((SAMPLE_BRANCH_LEVELS) - (level)) * \
		      SAMPLE_BIT_COUNT))) | \
		  (((jword)(value)) << \
		   (((SAMPLE_BRANCH_LEVELS) - (level)) * \
		      SAMPLE_BIT_COUNT))))
/* Align the address to something that's sample-able */
#define ALIGN_ADDR(addr) ((char *)((jword)((addr) + 2) & ~1))

#define min(x,y) ((x < y) ? x : y)
#define max(x,y) ((x > y) ? x : y)

/* Allocates and zeroes out memory for the sample bins */
static short *createSampleLeaves(void)
{
	short *retval;

	retval = (short *)KMALLOC(sizeof(short) * SAMPLE_LEAF_COUNT);
	return( retval );
}

/* Free memory allocated for the leaves */
static void deleteSampleLeaves(short *leaves)
{
	KFREE(leaves);
}

/* Allocates and zeroes out memory for the sample tree pointers */
static void **createSampleBranches(void)
{
	void **retval = 0;

	retval = (void **)KMALLOC(sizeof(void *) * SAMPLE_BRANCH_COUNT);
	return( retval );
}

/*
 * Delete a subtree of the sample branches, level indicates what level of the
 * tree we are on so we know when we've hit the leaves.
 */
static void deleteSampleBranches(void **ptr, int level)
{
	int lpc;

	for( lpc = 0; lpc < SAMPLE_BRANCH_COUNT; lpc++ )
	{
		if( ptr[lpc] )
		{
			if( (level + 1) == SAMPLE_BRANCH_LEVELS )
				deleteSampleLeaves((short *)ptr[lpc]);
			else
				deleteSampleBranches(ptr[lpc], level + 1);
		}
	}
}

struct memory_samples *createMemorySamples(void)
{
	struct memory_samples *retval;

	/* Allocate the root structure and the root branch pointers */
	if( (retval = KMALLOC(sizeof(struct memory_samples))) &&
	    (retval->ms_samples = createSampleBranches()) )
	{
		retval->ms_flags = 0;
		retval->ms_low = ((char *)0) - 1;
		retval->ms_high = 0;
		retval->ms_misses = 0;
	}
	else
	{
		KFREE(retval);
	}
	return( retval );
}

void deleteMemorySamples(struct memory_samples *ms)
{
	if( ms )
	{
		/* Free the tree */
		deleteSampleBranches(ms->ms_samples, 0);
		KFREE(ms);
	}
}

int observeMemory(struct memory_samples *ms, char *addr, int size)
{
	int retval = 1;

	if( ms->ms_flags & MSF_CONTIGUOUS )
	{
		/* Its a contiguous block of memory */
		if( addr >= ms->ms_low )
		{
			if( (addr + size) < ms->ms_high )
			{
			}
			else
			{
				void *samples;

				if( (samples = KREALLOC(ms->ms_samples,
							((addr + size) -
							 ms->ms_low) /
							sizeof(short))) )
				{
					ms->ms_high = addr + size;
					ms->ms_samples = samples;
				}
				else
				{
					retval = 0;
				}
			}
		}
		else
		{
			void *samples;

			if( (samples = KREALLOC(ms->ms_samples,
						(ms->ms_high - addr) /
						sizeof(short))) )
			{
				ms->ms_low = addr;
				ms->ms_samples = samples;
			}
		}
	}
	else
	{
		/* else... The bins are organized in a tree */
		void **level[SAMPLE_BRANCH_LEVELS];
		int slot[SAMPLE_BRANCH_LEVELS];

		ms->ms_low = min(ms->ms_low, addr);
		ms->ms_high = max(ms->ms_high, ALIGN_ADDR(addr + size));

		/*
		 * If the starting address doesn't fall directly on the
		 * boundary between leaves then we need to adjust `size' to
		 * incorporate the extra space.
		 */
		size += SAMPLE_BRANCH(addr, SAMPLE_BRANCH_LEVELS);

		/*
		 * Starting at the root we walk over the tree allocating
		 * necessary branches and leaves
		 */
		level[0] = ms->ms_samples;
		/* Loop until we've allocated everything */
		while( (size > 0) && retval )
		{
			int lpc;

			/* Process the address to get the branch indexes */
			for( lpc = 0; lpc < SAMPLE_BRANCH_LEVELS; lpc++ )
			{
				slot[lpc] = SAMPLE_BRANCH(addr, lpc);
			}
			/*
			 * Walk through the upper levels and allocate any
			 * needed branches.
			 */
			for( lpc = 0;
			     (lpc < (SAMPLE_BRANCH_LEVELS - 1)) && retval;
			     lpc++ )
			{
				if( !level[lpc][slot[lpc]] )
				{
					/* No branch has been allocated yet */
					level[lpc][slot[lpc]] =
						createSampleBranches();
					level[lpc + 1] = level[lpc][slot[lpc]];
					if( !level[lpc] )
						retval = 0;
				}
				else
				{
					level[lpc + 1] = level[lpc][slot[lpc]];
				}
			}
			/* Check if we need to allocate the leaf */
			if( retval && !level[lpc][slot[lpc]] )
			{
				if( (level[lpc][slot[lpc]] =
				     createSampleLeaves()) )
				{
				}
				else
				{
					retval = 0;
				}
			}
			/* Move on the next subrange */
			size -= SAMPLE_BRANCH_COUNT;
			addr += SAMPLE_BRANCH_COUNT;
		}
	}
	return( retval );
}

void memoryHit(struct memory_samples *ms, char *addr)
{
	void **level;
	short *sptr;
	int lpc;

	if( (addr >= ms->ms_low) && (addr <= ms->ms_high) )
	{
		/* Walk the tree */
		level = ms->ms_samples;
		for( lpc = 0; (lpc < SAMPLE_BRANCH_LEVELS) && level; lpc++ )
		{
			level = level[SAMPLE_BRANCH(addr, lpc)];
		}
		if( (sptr = (short *)level) )
		{
			/* Theres a leaf, increment the counter */
			sptr[SAMPLE_BIN(addr)]++;
		}
		else
		{
			/* No leaf, record it is a miss */
			ms->ms_misses++;
		}
	}
}

void memoryHitCount(struct memory_samples *ms, char *addr, int count)
{
	void **level;
	short *sptr;
	int lpc;

	if( (addr >= ms->ms_low) && (addr <= ms->ms_high) )
	{
		/* Walk the tree */
		level = ms->ms_samples;
		for( lpc = 0; (lpc < SAMPLE_BRANCH_LEVELS) && level; lpc++ )
		{
			level = level[SAMPLE_BRANCH(addr, lpc)];
		}
		if( (sptr = (short *)level) )
		{
			/* Theres a leaf, increment the counter */
			sptr[SAMPLE_BIN(addr)] += count;
		}
		else
		{
			/* No leaf, record it is a miss */
			ms->ms_misses += count;
		}
	}
}

/*
 * Recurses over the tree and calls the walker function on the leaf nodes.
 */
static int walkHelper(struct memory_samples *ms, char **addr, char *high_addr,
		      void *handle,
		      sample_walker_t walker, void **branches, int level)
{
	int retval = 0, lpc;

	if( level == SAMPLE_BRANCH_LEVELS )
	{
		/* We're at a leaf, call the walker function */
		retval = walker(handle,
				*addr,
				(short *)branches,
				min(high_addr - (*addr),
				    SAMPLE_BRANCH_COUNT -
				    SAMPLE_BRANCH(*addr,
						  SAMPLE_BRANCH_LEVELS)) / 2);
	}
	else
	{
		/*
		 * We're somewhere in the tree, start at whatever address is
		 * there initially and move from there.
		 */
		for( lpc = SAMPLE_BRANCH(*addr, level);
		     (lpc < SAMPLE_BRANCH_COUNT) && !retval;
		     lpc++ )
		{
			if( branches[lpc] )
			{
				*addr = SET_ADDR_LEVEL(*addr,
						       level,
						       lpc);
				retval = walkHelper(ms,
						    addr,
						    high_addr,
						    handle,
						    walker,
						    branches[lpc],
						    level + 1);
			}
			/* Clear out the initial value */
			*addr = SET_ADDR_LEVEL(*addr, level + 1, 0);
		}
	}
	return( retval );
}

void walkMemorySamples(struct memory_samples *ms,
		       char *low_addr,
		       char *high_addr,
		       void *handle,
		       sample_walker_t walker)
{
	walkHelper(ms, &low_addr, high_addr,
		   handle, walker, ms->ms_samples, 0);
}

/* A simple walker function that zeroes out the sample values */
static int resetBinsWalker(void *handle UNUSED, char *addr UNUSED, short *bins, size_t size)
{
	memset(bins, 0, size * sizeof(short));
	return( 0 );
}

void resetMemorySamples(struct memory_samples *ms)
{
	walkMemorySamples(ms, 0, ms->ms_high, 0, resetBinsWalker);
	ms->ms_misses = 0;
}

/* A simple walker function that prints out the bin values */
static int printBinsWalker(void *handle, char *addr, short *bins, size_t size)
{
	FILE *file = handle;
	int printed_header = 0;
	size_t lpc;

	for( lpc = 0; lpc < size; lpc++ )
	{
		if( bins[lpc] )
		{
			if( !printed_header )
			{
				/* Print out a header message */
				fprintf(file, "bins from %p to %p:\n",
					addr,
					addr + SAMPLE_BRANCH_COUNT);
				printed_header = 1;
			}
 			fprintf(file, "  %p: %d\n",
				addr + (lpc * 2),
				bins[lpc]);
		}
	}
	return( 0 );
}

void printMemorySamples(FILE *file, struct memory_samples *ms)
{
	walkMemorySamples(ms, 0, ms->ms_high, file, printBinsWalker);
}

#endif /* KAFFE_XPROFILER */
