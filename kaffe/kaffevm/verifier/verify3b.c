/**
 * verify3b.c
 *
 * Copyright 2004
 *   Kaffe.org contributors. See ChangeLog for details. All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#include "baseClasses.h"
#include "bytecode.h"
#include "classMethod.h"
#include "code.h"
#include "constants.h"
#include "exception.h"
#include "errors.h"
#include "debug.h"
#include "gc.h"

#include "verify.h"
#include "verify-block.h"
#include "verify-debug.h"
#include "verify-errors.h"
#include "verify-type.h"


static inline bool verifyErrorInVerifyMethod3b(Verifier* v,
					       BlockInfo* curBlock,
					       const char * msg);
static        bool mergeBasicBlocks(Verifier* v,
				    BlockInfo* fromBlock,
				    BlockInfo* toBlock);


/**
 * Helper function for error reporting in verifyMethod3b
 */
static inline 
bool
verifyErrorInVerifyMethod3b(Verifier* v, BlockInfo* curBlock, const char * msg)
{
        gc_free(curBlock);
	return verifyError(v, msg);
}

/*
 * verifyMethod3b()
 *    The Data-flow Analyzer
 *
 * The data-flow algorithm is taken from the JVM 2 spec, which describes it more or less as follows:
 *
 *  0  data-flow analyzer is initialised
 *       - for the first instruction of the method, the local variables that represent parameters
 *         initially contain values of the types indicated by the method's type descriptor.
 *       - the operand stack is empty.
 *       - all local variables contain an illegal value.
 *       - for the other instructions, which have not been examined yet, no information is available
 *         regarding the operand stack or local variables.
 *       - the "changed" bit is only set for the first instruction.
 *
 *  1  select a VM instruction whose "changed" bit is set
 *
 *       - if no such instruction remains, the method has successfully been verified.
 *       - otherwise, turn off the "changed" bit of the selected instruction.
 *
 *  2  model the effect of the instruction on the operand stack and local variable array by:
 *
 *       - if the instruction uses values from the operand stack, ensure that there are a
 *         sufficient number of values on the stack and that the top values on the stack are
 *         of an appropriate type.
 *       - if the instruction uses a local variable, ensure that the specified local variable
 *         contains a value of the appropriate type.
 *       - if the instruction pushes values onto the operand stack, ensure that there is sufficient
 *         room on the operand stack for the new values.  add the indicated types to the type of the
 *         modeled operand stack.
 *       - if the instruction modifies a local variable, record that the local variable now contains
 *         a type.
 *
 *  3  determine the instructions that can follow the current instruction.  successor instructions
 *     can be one of the following:
 *
 *       - the next instruction, if the current instruction is not an unconditional control tranfer
 *         instruction (ie - goto, return, or athrow).  basically check to make sure you don't
 *         "fall off" the last instruction of the method.
 *       - the target of a conditional or unconditional branch or switch.
 *       - any exception handlers for this instruction.
 *
 *  4  merge the state of the operand stack and local variable array at the end of the execution of the
 *     current instruction into each of the successor instructions.
 *
 *     (see merge function below)
 *
 *  5  continue at step 1.
 */
bool
verifyMethod3b(Verifier* v)
{
	const uint32 codelen      = METHOD_BYTECODE_LEN(v->method);
	const unsigned char* code = METHOD_BYTECODE_CODE(v->method);
	
	
	BlockInfo** blocks = v->blocks; /* aliased for convenience */
	uint32 curIndex;
	BlockInfo* curBlock;
	BlockInfo* nextBlock;

	uint32 pc = 0, newpc = 0, n = 0;
	int32 high = 0, low = 0;  /* for the switching instructions */
	
	
	
	DBG(VERIFY3, dprintf("    Verifier Pass 3b: Data Flow Analysis and Type Checking...\n"); );
	DBG(VERIFY3, dprintf("        memory allocation...\n"); );
	curBlock = createBlock(v->method);
	
	
	DBG(VERIFY3, dprintf("        doing the dirty data flow analysis...\n"); );
	blocks[0]->status |= CHANGED;
	curIndex = 0;
	while(curIndex < v->numBlocks) {
		DBG(VERIFY3,
		    dprintf("      blockNum/first pc/changed/stksz = %d / %d / %d / %d\n",
			    curIndex,
			    blocks[curIndex]->startAddr,
			    blocks[curIndex]->status & CHANGED,
			    blocks[curIndex]->stacksz);
		    dprintf("          before:\n");
		    printBlock(v->method, blocks[curIndex], "                 ");
		    );
		
		if (!(blocks[curIndex]->status & CHANGED)) {
			DBG(VERIFY3, dprintf("        not changed...skipping\n"); );
			curIndex++;
			continue;
		}
		
		blocks[curIndex]->status ^= CHANGED; /* unset CHANGED bit */
		blocks[curIndex]->status |= VISITED; /* make sure we've visited it...important for merging */
		copyBlockData(v->method, blocks[curIndex], curBlock);
		
		if (curBlock->status & EXCEPTION_HANDLER && curBlock->stacksz > 0) {
			return verifyErrorInVerifyMethod3b(v, curBlock, "it's possible to reach an exception handler with a nonempty stack");
		}
		
		
		if (!verifyBasicBlock(v, curBlock)) {
			return verifyErrorInVerifyMethod3b(v, curBlock, "failure to verify basic block");
		}
		
		
		DBG(VERIFY3, dprintf("          after:\n"); printBlock(v->method, curBlock, "                 "); );
		
		
		/*
		 * merge this block's information into the next block
		 */
		pc = curBlock->lastAddr;
		if (code[pc] == WIDE && code[getNextPC(code, pc)] == RET)
			pc = getNextPC(code, pc);
		switch(code[pc])
			{
			case GOTO:
				newpc = pc + 1;
				newpc = pc + getWord(code, newpc);
				nextBlock = inWhichBlock(newpc, blocks, v->numBlocks);
				
				if (!mergeBasicBlocks(v, curBlock, nextBlock)) {
					return verifyErrorInVerifyMethod3b(v, curBlock, "error merging operand stacks");
				}
				break;
				
			case GOTO_W:
				newpc = pc + 1;
				newpc = pc + getDWord(code, newpc);
				nextBlock = inWhichBlock(newpc, blocks, v->numBlocks);
				
				if (!mergeBasicBlocks(v, curBlock, nextBlock)) {
					return verifyErrorInVerifyMethod3b(v, curBlock, "error merging operand stacks");
				}
				break;
					
			case JSR:
				newpc = pc + 1;
				newpc = pc + getWord(code, newpc);
				goto JSR_common;
			case JSR_W:
				newpc = pc + 1;
				newpc = pc + getDWord(code, newpc);
			JSR_common:
				nextBlock = inWhichBlock(newpc, blocks, v->numBlocks);
				
				if (!mergeBasicBlocks(v, curBlock, nextBlock)) {
					return verifyErrorInVerifyMethod3b(v, curBlock, "jsr: error merging operand stacks");
				}
	
				/* TODO:
				 * args, we need to verify the RET block first ...
				 */
				for (;curIndex < v->numBlocks && blocks[curIndex] != nextBlock; curIndex++);
				assert (curIndex < v->numBlocks);
				continue;
				
			case RET:
				if (v->status[pc] & WIDE_MODDED) {
					n = pc + 1;
					n = getWord(code, n);
				} else {
					n = code[pc + 1];
				}
				
				if (!IS_ADDRESS(&curBlock->locals[n])) {
					return verifyErrorInVerifyMethod3b(v, curBlock, "ret instruction does not refer to a variable with type returnAddress");
				}
				
				newpc = curBlock->locals[n].tinfo;
				
				/* each instance of return address can only be used once */
				curBlock->locals[n] = *getTUNSTABLE();
				
				nextBlock = inWhichBlock(newpc, blocks, v->numBlocks);
				if (!mergeBasicBlocks(v, curBlock, nextBlock)) {
					return verifyErrorInVerifyMethod3b(v, curBlock, "error merging opstacks when returning from a subroutine");
				}

				/* 
				 * unmark this block as visited, so that the next
				 * entry is treated as a first time merge.
				 */
				blocks[curIndex]->status ^= VISITED;
				break;	
				
			case IF_ACMPEQ:  case IFNONNULL:
			case IF_ACMPNE:  case IFNULL:
			case IF_ICMPEQ:  case IFEQ:
			case IF_ICMPNE:	 case IFNE:
			case IF_ICMPGT:	 case IFGT:
			case IF_ICMPGE:	 case IFGE:
			case IF_ICMPLT:	 case IFLT:
			case IF_ICMPLE:	 case IFLE:
				newpc     = pc + 1;
				newpc     = pc + getWord(code, newpc);
				nextBlock = inWhichBlock(newpc, blocks, v->numBlocks);
				
				if (!mergeBasicBlocks(v, curBlock, nextBlock)) {
					return verifyErrorInVerifyMethod3b(v, curBlock, "error merging operand stacks");
				}
				
				/* if the condition is false, then the next block is the one that will be executed */
				curIndex++;
				if (curIndex >= v->numBlocks) {
					return verifyErrorInVerifyMethod3b(v, curBlock, "execution falls off the end of a basic block");
				}
				else if (!mergeBasicBlocks(v, curBlock, blocks[curIndex])) {
					return verifyErrorInVerifyMethod3b(v, curBlock, "error merging operand stacks");
				}
				break;
				
				
			case LOOKUPSWITCH:
			        /* default branch...between 0 and 3 bytes of padding are added so that the
				 * default branch is at an address that is divisible by 4
				 */
				n = (pc + 1) % 4;
				if (n) n = pc + 5 - n;
				else   n = pc + 1;
				newpc = pc + getDWord(code, n);
				nextBlock = inWhichBlock(newpc, blocks, v->numBlocks);
				if (!mergeBasicBlocks(v, curBlock, nextBlock)) {
					return verifyErrorInVerifyMethod3b(v, curBlock, "error merging into the default branch of a lookupswitch instruction");
				}
				
				/* get number of key/target pairs */
				n += 4;
				low = getDWord(code, n);
				
				/* branch into all targets */
				/* NOTE: the cast is there only to keep gcc happy */
				for (n += 4, high = n + 8*low; n < (uint32)high; n += 8) {
					newpc = pc + getDWord(code, n+4);
					nextBlock = inWhichBlock(newpc, blocks, v->numBlocks);
					if (!mergeBasicBlocks(v, curBlock, nextBlock)) {
						return verifyErrorInVerifyMethod3b(v, curBlock, "error merging into a branch of a lookupswitch instruction");
					}
				}
				
				break;
				
			case TABLESWITCH:
			        /* default branch...between 0 and 3 bytes of padding are added so that the
				 * default branch is at an address that is divisible by 4
				 */
				n = (pc + 1) % 4;
				if (n) n = pc + 5 - n;
				else   n = pc + 1;
				newpc = pc + getDWord(code, n);
				
				/* get the high and low values of the table */
				low  = getDWord(code, n + 4);
				high = getDWord(code, n + 8);
				
				n += 12;
				
				/* high and low are used as temps in this loop that checks
				 * the validity of all the branches in the table
				 */
				/* NOTE: the cast is there only to keep gcc happy */
				for (high = n + 4*(high - low + 1); n < (uint32)high; n += 4) {
					newpc = pc + getDWord(code, n);
					nextBlock = inWhichBlock(newpc, blocks, v->numBlocks);
					if (!mergeBasicBlocks(v, curBlock, nextBlock)) {
						return verifyErrorInVerifyMethod3b(v, curBlock, "error merging into a branch of a tableswitch instruction");
					}
				}
				break;
				
				
				/* the rest of the ways to end a block */
			case RETURN:
			case ARETURN:
			case IRETURN:
			case FRETURN:
			case LRETURN:
			case DRETURN:
			case ATHROW:
				curIndex++;
				continue;
				
			default:
				for (n = pc + 1; n < codelen; n++) {
					if (v->status[n] & IS_INSTRUCTION) break;
				}
				if (n == codelen) {
					return verifyErrorInVerifyMethod3b(v, curBlock, "execution falls off the end of a code block");
				}
				else if (!mergeBasicBlocks(v, curBlock, blocks[curIndex+1])) {
					return verifyErrorInVerifyMethod3b(v, curBlock, "error merging operand stacks");
				}
			}
		
		
		for (curIndex = 0; curIndex < v->numBlocks; curIndex++) {
			if (blocks[curIndex]->status & CHANGED)
				break;
		}
	}
	
	
	DBG(VERIFY3, dprintf("    Verifier Pass 3b: Complete\n"); );
	gc_free(curBlock);
	return(true);
}

/*
 * merges two operand stacks.  just to repeat what the JVML 2 spec says about this:
 *   Merge the state of the operand stack and local variable array at the end of the
 *   execution of the current instruction into each of the successor instructions.  In
 *   the special case of control transfer to an exception handler, the operand stack is
 *   set to contain a single object of the exception type indicated by the exception
 *   handler information.
 *     - if this if the first time the successor instruction has been visited, record
 *       that the operand stack and local variable values calculated in steps 2 and 3
 *       are the state of the operand stack and local variable array prior to executing
 *       the successor instruction.  Set the "changed" bit for the successor instruction.
 *     - if the successor instruction has been seen before, merge the operand stack and
 *       local variable values calculated in steps 2 and 3 into the values already there.
 *       set the "changed" bit if there is any modification to the values.
 *
 *   to merge two operand stacks, the number of values on each stack must be identical.
 *   the types of values on the stacks must also be identical, except that differently
 *   typed reference values may appear at corresponding places on the two stacks.  in this
 *   case, the merged operand stack contains a reference to an instance of the first common
 *   superclass of the two types.  such a reference type always exists because the type Object
 *   is a superclass of all class and interface types.  if the operand stacks cannot be merged,
 *   verification of the method fails.
 *
 *   to merge two local variable array states, corresponding pairs of local variables are
 *   compared.  if the two types are not identical, then unless both contain reference values,
 *   the verification records that the local variable contains an unusable value.  if both of
 *   the pair of local variables contain reference values, the merged state contains a reference
 *   to an instance of the first common superclass of the two types.
 */
static
bool
mergeBasicBlocks(Verifier* v,
		 BlockInfo* fromBlock,
		 BlockInfo* toBlock)
{
	uint32 n;
	
	
	/* Ensure that no uninitiazed object instances are in the local variable array
	 * or on the operand stack during a backwards branch
	 */
	if (toBlock->startAddr < fromBlock->startAddr) {
		for (n = 0; n < v->method->localsz; n++) {
			if (fromBlock->locals[n].tinfo & TINFO_UNINIT) {
				return verifyError(v, "uninitialized object reference in a local variable during a backwards branch");
			}
		}
		for (n = 0; n < fromBlock->stacksz; n++) {
			if (fromBlock->opstack[n].tinfo & TINFO_UNINIT) {
				return verifyError(v, "uninitialized object reference on operand stack during a backwards branch");
			}
		}
	}
	
	if (!(toBlock->status & VISITED)) {
		DBG(VERIFY3, dprintf("          visiting block starting at %d for the first time\n",
				     toBlock->startAddr); );
		
		copyBlockState(v->method, fromBlock, toBlock);
		toBlock->status |= CHANGED;
		return(true);
	}
	
	DBG(VERIFY3,
	    dprintf("%snot a first time merge\n", indent);
	    dprintf("%s  from block (%d - %d):\n", indent, fromBlock->startAddr, fromBlock->lastAddr);
	    printBlock(v->method, fromBlock, indent2);
	    dprintf("%s  to block (%d - %d):\n", indent, toBlock->startAddr, toBlock->lastAddr);
	    printBlock(v->method, toBlock, indent2);
	    dprintf("\n");
	    );
	
	
	if (fromBlock->stacksz != toBlock->stacksz) {
		return verifyError(v, "merging two operand stacks of unequal size");
	}
	
	
	/* merge the local variable arrays */
	for (n = 0; n < v->method->localsz; n++) {
		if (mergeTypes(v, &fromBlock->locals[n], &toBlock->locals[n])) {
			toBlock->status |= CHANGED;
		}
	}
	
	/* merge the operand stacks */
	for (n = 0; n < fromBlock->stacksz; n++) {
	        /* if we get unstable here, not really a big deal until we try to use it.
		 * i mean, we could get an unstable value and then immediately pop it off the stack,
		 * for instance.
		 */
		if (mergeTypes(v, &fromBlock->opstack[n], &toBlock->opstack[n])) {
			toBlock->status |= CHANGED;
		}
	}
	
	
	DBG(VERIFY3,
	    dprintf("%s  result block:\n", indent);
	    printBlock(v->method, toBlock, indent2);
	    );
	
	
	return(true);
}
