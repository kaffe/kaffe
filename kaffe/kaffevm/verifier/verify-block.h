/*
 * verify-block.h
 *
 * Copyright 2004
 *   Kaffe.org contributors. See ChangeLog for details. All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Interface to code for handing of blocks in the verifier.
 */

#if !defined(VERIFY_BLOCK_H)
#define VERIFY_BLOCK_H

#include "gtypes.h"
#include "verify-type.h"

/*
 * basic block header information
 */
typedef struct BlockInfo
{
        /* address of start of block */
	uint32 startAddr;
        uint32 lastAddr;  /* whether it be the address of a GOTO, etc. */
	
        /* status of block...
	 * changed (needs to be re-evaluated), visited, etc. 
	 */
	uint32 status;
	
        /* array of local variables */
	Type*  locals;
	
        /* simulated operand stack */
	uint32 stacksz;
	Type*  opstack;
} BlockInfo;

/* status flags for a basic block.
 * these also pertain to the status[] array for the entire instruction array
 */
#define CHANGED            1
#define VISITED            2
#define IS_INSTRUCTION     4

/* if the instruction is preceeded by WIDE */
#define WIDE_MODDED        8

/* used at the instruction status level to find basic blocks */
#define START_BLOCK       16
#define END_BLOCK         32

#define EXCEPTION_HANDLER 64

/*
 * allocate memory for a block info and fill in with default values
 */
extern BlockInfo* createBlock(const Method* method);

/*
 * frees the memory of a basic block
 */
extern void freeBlock(BlockInfo* binfo);

/*
 * copies information from one stack of basic blocks to another
 */
extern void copyBlockData(const Method* method, BlockInfo* fromBlock, BlockInfo* toBlock);

/*
 * copies the local variables, operand stack, status, and context
 * from one block to another.
 */
extern void copyBlockState(const Method* method, BlockInfo* fromBlock, BlockInfo* toBlock);

/*
 * returns which block the given pc is in
 */
extern BlockInfo* inWhichBlock(uint32 pc, BlockInfo** blocks, uint32 numBlocks);

#endif /* !defined(VERIFY_BLOCK_H) */

