/*
 * verify-block.c
 *
 * Copyright 2004
 *   Kaffe.org contributors. See ChangeLog for details. All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Code for handing of blocks in the verifier.
 */

#include "debug.h"
#include "verify-block.h"

/*
 * allocate memory for a block info and fill in with default values
 */
BlockInfo*
createBlock(const Method* method)
{
	int i;
	
	BlockInfo* binfo = checkPtr((BlockInfo*)gc_malloc(sizeof(BlockInfo), GC_ALLOC_VERIFIER));
	
	binfo->startAddr   = 0;
	binfo->status      = IS_INSTRUCTION | START_BLOCK;  /* not VISITED or CHANGED */
	
	/* allocate memory for locals */
	if (method->localsz > 0) {
		binfo->locals = checkPtr(gc_malloc(method->localsz * sizeof(Type), GC_ALLOC_VERIFIER));
		
		for (i = 0; i < method->localsz; i++) {
			binfo->locals[i] = *TUNSTABLE;
		}
	} else {
		binfo->locals = NULL;
	}
	
	
	/* allocate memory for operand stack */
	binfo->stacksz = 0;
	if (method->stacksz > 0) {
		binfo->opstack = checkPtr(gc_malloc(method->stacksz * sizeof(Type), GC_ALLOC_VERIFIER));
		
		for (i = 0; i < method->stacksz; i++) {
			binfo->opstack[i] = *TUNSTABLE;
		}
	} else {
		binfo->opstack = NULL;
	}
	
	return binfo;
}

/*
 * frees the memory of a basic block
 */
void
freeBlock(BlockInfo* binfo)
{
	if (binfo == NULL) return;
	
	if (binfo->locals != NULL)
		gc_free(binfo->locals);
	if (binfo->opstack != NULL)
		gc_free(binfo->opstack);
	
	gc_free(binfo);
}

/*
 * copies information from one stack of basic blocks to another
 */
void
copyBlockData(const Method* method, BlockInfo* fromBlock, BlockInfo* toBlock)
{
	toBlock->startAddr = fromBlock->startAddr;
	toBlock->lastAddr  = fromBlock->lastAddr;
	
	copyBlockState(method, fromBlock, toBlock);
}

/*
 * copies the local variables, operand stack, status, and context
 * from one block to another.
 */
void
copyBlockState(const Method* method, BlockInfo* fromBlock, BlockInfo* toBlock)
{
	uint32 n;
	
	toBlock->status  = fromBlock->status;
	
	for (n = 0; n < method->localsz; n++) {
		toBlock->locals[n] = fromBlock->locals[n];
	}
	
	toBlock->stacksz = fromBlock->stacksz;
	for (n = 0; n < method->stacksz; n++) {
		toBlock->opstack[n] = fromBlock->opstack[n];
	}
}

/*
 * returns which block the given pc is in
 */
BlockInfo*
inWhichBlock(uint32 pc, BlockInfo** blocks, uint32 numBlocks)
{
	uint32 i;
	for (i = 0; i < numBlocks; i++) {
		if (pc < blocks[i]->startAddr) continue;
		if (pc <= blocks[i]->lastAddr) return blocks[i];
	}
	
	/* shouldn't ever get here unless the specified PC is messed up */
	DBG(VERIFY3, dprintf("inWhichBlock(...): pc = %d out of range...weird.\n", pc); );
	
	return NULL;
}
