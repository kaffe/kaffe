/*
 * callGraph.h
 * Routines for tracking the call graph of jitted code
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

#ifndef __callgraph_h
#define __callgraph_h

#include "gmonFile.h"

/* A call arc as stored in a hash table */
struct call_arc {
	struct call_arc *ca_next;	/* Hash table link */
	char *ca_from;			/* Caller */
	char *ca_to;			/* Callee */
	int ca_count;			/* Traversal count */
};

/* Size of the hash table */
#define CALL_GRAPH_TABLE_SIZE 2047

/* The root data structure for a collection of call graphs */
struct call_graph {
	int cg_misses;			/* Arcs we couldn't record */
	int cg_free;			/* Index of the next free call_arc */
	int cg_count;			/* Number of call_arc's allocated */
	struct call_arc *cg_table[CALL_GRAPH_TABLE_SIZE];	/* Hashtable */
	struct call_arc *cg_arcs;	/* Array of call_arc structures */
};

/*
 * Create a call graph and allocate `count' arcs for it
 */
struct call_graph *createCallGraph(int count);
/*
 * Delete the call graph object
 */
void deleteCallGraph(struct call_graph *cg);
/*
 * Increment the counter for this call arc
 */
void arcHit(struct call_graph *cg, char *frompc, char *selfpc);
/*
 * Reset the values in the call graph
 */
void resetCallGraph(struct call_graph *cg);
/*
 * Write the call graph to the given gmon file
 */
int writeCallGraph(struct call_graph *cg, struct gmon_file *gf);

#endif /* __callgraph_h */
