/*
 * memorySamples.h
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

#ifndef __memorysamples_h
#define __memorysamples_h

#include <stdio.h>

/* Bit numbers for various flags */
enum {
	MSB_CONTIGUOUS,	/* The sample memory is contiguous */
};

/* Flags for the memory_samples structure */
enum {
	MSF_CONTIGUOUS = 1L << MSB_CONTIGUOUS,
};

/* Root structure that holds references to the sample bins */
struct memory_samples {
	unsigned int ms_flags;	/* Flags */
	char *ms_low;		/* Low water mark for samples */
	char *ms_high;		/* High water mark for samples */
	unsigned int ms_misses;	/* Hits that fell outside the observed range */
	void *ms_samples;	/* The root of the samples tree */
};

/*
 * Typedef for a function that is called while walking over the samples tree.
 * The function will be called for each sample bin reached, `addr' refers
 * to the beginning address that the `bins' cover.  Size is the number of
 * actual bins.
 */
typedef int (*sample_walker_t)(void *handle, char *addr,
			       short *bins, size_t size);

/*
 * Create a memory_samples structure.
 */
struct memory_samples *createMemorySamples(void);
/*
 * Delete a memory_samples structure.
 */
void deleteMemorySamples(struct memory_samples *ms);
/*
 * Allocate space in the sample tree to hold counters for the specified memory
 * range
 */
int observeMemory(struct memory_samples *ms, char *addr, int size);
/*
 * Increment the counter in the sample tree for the given address, if the
 * address isn't being observed then the misses counter is increased.
 */
void memoryHit(struct memory_samples *ms, char *addr);
void memoryHitCount(struct memory_samples *ms, char *addr, int count);
/*
 * Walk over the sample tree calling the walker function for each bin reached
 */
void walkMemorySamples(struct memory_samples *ms,
		       char *low_addr,
		       char *high_addr,
		       void *handle,
		       sample_walker_t walker);
/*
 * Reset memory sample values
 */
void resetMemorySamples(struct memory_samples *ms);
/*
 * Print out the memory sample tree.
 */
void printMemorySamples(FILE *file, struct memory_samples *ms);

#endif /* __memorysamples_h */
