/**
 * verify3a.c
 *
 * Copyright 2004
 *   Kaffe.org contributors. See ChangeLog for details. All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 *
 * Performs pass 3a of bytecode verification:
 * verification of static constaints.
 */

#include "config.h"

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include "baseClasses.h"
#include "bytecode.h"
#include "classMethod.h"
#include "code.h"
#include "constants.h"
#include "exception.h"
#include "errors.h"
#include "debug.h"
#include "gc.h"
#include "lookup.h"
#include "soft.h"

#include "verify.h"
#include "verify-block.h"
#include "verify-debug.h"
#include "verify-errors.h"

/*
 * Helper function for error reporting in BRANCH_IN_BOUNDS macro in verifyMethod3a.
 */
static inline
bool
branchInBoundsErrorInVerifyMethod3a(Verifier* v,
				    uint32 codelen,
				    uint32 n)
{
	DBG(VERIFY3, dprintf("ERROR: branch to (%d) out of bound (%d) \n", n, codelen); );
	return verifyError(v, "branch out of method code");
}

/*
 * Helper function for error reporting in CHECK_LOCAL_INDEX macro in verifyMethod3a.
 */
static inline
bool
checkLocalIndexErrorInVerifyMethod3a(Verifier* v,
				     uint32 pc,
				     unsigned char* code,
				     uint32 n)
{
	DBG(VERIFY3,
	    dprintf("ERROR:  pc = %d, instruction = ", pc);
	    printInstruction(code[pc]);
	    dprintf(", localsz = %d, localindex = %d\n", v->method->localsz, n);
	    );
	return verifyError(v, "attempting to access a local variable beyond local array");
}

/*
 * verifyMethod3a()
 *     check static constraints.  section 4.8.1 of JVML Spec 2.
 *
 * NOTE: we don't check whether execution can fall off the end of method code here as
 *       that would require us to know whether the last statements are reachable.
 *       Sun's verifier, for instance, rejects code with an unreachable NOP at the end!
 *       Thus we check whether execution can fall off the end during the data flow analysis
 *       of pass 3b, structural constraint checking.
 *
 * TODO: turn some of these macros into inlined functions
 */
void
verifyMethod3a(Verifier* v)
{

#define ENSURE_NON_WIDE \
	if (wide) { \
		verifyError(v, "illegal instruction following wide instruction"); \
                return; \
	}

#define CHECK_POOL_IDX(_IDX) \
	if (_IDX > pool->size) { \
		verifyError(v, "attempt to access a constant pool index beyond constant pool range"); \
                return; \
	}

	
#define GET_IDX(_IDX, _PC) \
	(_IDX) = getIdx(code, _PC); \
	CHECK_POOL_IDX(_IDX)

#define GET_WIDX(_IDX, _PC) \
	_IDX = getWIdx(code, _PC); \
	CHECK_POOL_IDX(_IDX)

#define BRANCH_IN_BOUNDS(_N, _INST) \
	if (_N >= codelen) { \
		branchInBoundsErrorInVerifyMethod3a(v, codelen, _N); \
		return; \
	}

        /* makes sure the index given for a local variable is within the correct index */
#define CHECK_LOCAL_INDEX(_N) \
	if ((_N) >= v->method->localsz) { \
		checkLocalIndexErrorInVerifyMethod3a(v, pc, code, _N); \
		return; \
	}
	
	const constants* pool = CLASS_CONSTANTS(v->method->class);
	
	/* used for looking at method signatures... */
	const char* sig;
	
	uint32 codelen      = (uint32)METHOD_BYTECODE_LEN(v->method);
	unsigned char* code = METHOD_BYTECODE_CODE(v->method);
	
	uint32 pc = 0, newpc = 0, n = 0, idx = 0;
	int32 branchoffset = 0;
	int32 low, high;
	
	bool wide;
	bool inABlock; /* used when calculating the start/return address of each block */
	
	uint32 blockCount  = 0;
	BlockInfo** blocks = NULL;
	
	
	DBG(VERIFY3, dprintf("    Verifier Pass 3a: checking static constraints and finding basic blocks...\n"); );
	if (METHOD_BYTECODE_LEN(v->method) < 0) {
		verifyError(v, "method bytecode length is less than 0");
	}
	
	/* find the start of every instruction and basic block to determine legal branches
	 *
	 * also, this makes sure that only legal instructions follow the WIDE instruction
	 */
	v->status[0] |= START_BLOCK;
	wide = false;
	pc = 0;
	while(pc < codelen) {
		v->status[pc] |= IS_INSTRUCTION;
		
		DBG(VERIFY3, dprintf("        instruction: (%d) ", pc); printInstruction(code[pc]); dprintf("\n"); );
		
		if (codelen < getNextPC(code, pc)) {
			verifyError(v, "last operand in code array is cut off");
			return;
		}
		
		switch(code[pc]) {
		case ALOAD_0: case ASTORE_0:
		case ILOAD_0: case ISTORE_0:
		case FLOAD_0: case FSTORE_0:
			ENSURE_NON_WIDE;
			CHECK_LOCAL_INDEX(0);
			break;
		case ALOAD_1: case ASTORE_1:
		case ILOAD_1: case ISTORE_1:
		case FLOAD_1: case FSTORE_1:
		case LLOAD_0: case LSTORE_0:
		case DLOAD_0: case DSTORE_0:
			ENSURE_NON_WIDE;
			CHECK_LOCAL_INDEX(1);
			break;
		case ALOAD_2: case ASTORE_2:
		case ILOAD_2: case ISTORE_2:
		case FLOAD_2: case FSTORE_2:
		case LLOAD_1: case LSTORE_1:
		case DLOAD_1: case DSTORE_1:
			ENSURE_NON_WIDE;
			CHECK_LOCAL_INDEX(2);
			break;
		case ALOAD_3: case ASTORE_3:
		case ILOAD_3: case ISTORE_3:
		case FLOAD_3: case FSTORE_3:
		case LLOAD_2: case LSTORE_2:
		case DLOAD_2: case DSTORE_2:
			ENSURE_NON_WIDE;
			CHECK_LOCAL_INDEX(3);
			break;
		case LLOAD_3: case LSTORE_3:
		case DLOAD_3: case DSTORE_3:
			ENSURE_NON_WIDE;
			CHECK_LOCAL_INDEX(4);
			break;
			
			
		case LDC1:
			GET_IDX(idx, pc);
			goto LDC_common;
		case LDC2:
			GET_WIDX(idx, pc);
		LDC_common:
			n = CONST_TAG(idx, pool);
			if (n != CONSTANT_Integer && n != CONSTANT_Float &&
			    n != CONSTANT_String && n != CONSTANT_ResolvedString) {
				verifyError(v, "ldc* on constant pool entry other than int/float/string");
				return;
			}
			break;
			
		case LDC2W:
			GET_WIDX(idx, pc);
			n = CONST_TAG(idx, pool);
			if (n != CONSTANT_Double && n != CONSTANT_Long) {
				verifyError(v, "ldc2_w on constant pool entry other than long or double");
				return;
			}
			break;
			
		case GETFIELD:  case PUTFIELD:
		case GETSTATIC: case PUTSTATIC:
			ENSURE_NON_WIDE;
			
			GET_WIDX(idx, pc);
			idx = CONST_TAG(idx, pool);
			if (idx != CONSTANT_Fieldref) {
				verifyError(v, "[get/put][field/static] accesses something in the constant pool that is not a CONSTANT_Fieldref");
				return;
			}
			break;
			
		case INVOKEVIRTUAL:
		case INVOKESTATIC:
		case INVOKESPECIAL:
			ENSURE_NON_WIDE;
			
			GET_WIDX(idx, pc);
			n = CONST_TAG(idx, pool);
			if (n != CONSTANT_Methodref) {
				verifyError(v, "invoke* accesses something in the constant pool that is not a CONSTANT_Methodref");
				return;
			}
			
			sig = METHODREF_SIGD(idx, pool);
			if (*sig == '<') {
				if (!strcmp(constructor_name->data, sig)) {
					if (code[pc] != INVOKESPECIAL) {
						 verifyError(v, "only invokespecial can be used to execute <init> methods");
						 return;
					}
				} else {
					verifyError(v, "no method with a name whose first character is '<' may be called by an invoke instruction");
					return;
				}
			}
			
			break;
			
			
			/* invokeinterface is a 5 byte instruction.  the first byte is the instruction.
			 * the next two are the index into the constant pool for the methodreference.
			 * the fourth is the number of parameters expected by the method, and the verifier
			 * must check that the actual method signature of the method to be invoked matches
			 * this number.  the 5th must be zero.  these are apparently present for historical
			 * reasons (yeah Sun :::smirk:::).
			 */
		case INVOKEINTERFACE:
			ENSURE_NON_WIDE;
			
			GET_WIDX(idx, pc);
			n = CONST_TAG(idx, pool);
			if (n != CONSTANT_InterfaceMethodref) {
				verifyError(v, "invokeinterface accesses something in the constant pool that is not a CONSTANT_InterfaceMethodref");
				return;
			}
			
			sig = INTERFACEMETHODREF_SIGD(idx, pool);
			if (*sig == '<') {
				verifyError(v, "invokeinterface cannot be used to invoke any instruction with a name starting with '<'");
				return;
			}
			
			if (code[pc + 3] == 0) {
				verifyError(v, "fourth byte of invokeinterface is zero");
				return;
			} else if (code[pc + 4] != 0) {
				verifyError(v, "fifth byte of invokeinterface is not zero");
				return;
			}
			
			break;
			
			
		case INSTANCEOF:
		case CHECKCAST:
			ENSURE_NON_WIDE;
			
			GET_WIDX(n, pc);
			n = CONST_TAG(n, pool);
			if (n != CONSTANT_Class && n != CONSTANT_ResolvedClass) {
				verifyError(v, "instanceof/checkcast indexes a constant pool entry that is not type CONSTANT_Class or CONSTANT_ResolvedClass");
				return;
			}
			
			break;
			
			
		case MULTIANEWARRAY:
			ENSURE_NON_WIDE;
			
			GET_WIDX(idx, pc);
			n = CONST_TAG(idx, pool);
			if (n != CONSTANT_Class && n != CONSTANT_ResolvedClass) {
				verifyError(v, "multinewarray indexes a constant pool entry that is not type CONSTANT_Class or CONSTANT_ResolvedClass");
				return;
			}
			
			/* number of dimensions must be <= num dimensions of array type being created */
			sig = CLASS_NAMED(idx, pool);
			newpc = code[pc + 3];
			if (newpc == 0) {
				verifyError(v, "dimensions operand of multianewarray must be non-zero");
				return;
			}
			for(n = 0; *sig == '['; sig++, n++);
			if (n < newpc) {
				verifyError(v, "dimensions operand of multianewarray is > the number of dimensions in array being created");
				return;
			}
			
			break;
			
			
		case NEW:
			ENSURE_NON_WIDE;
			
			GET_WIDX(idx, pc);
			n = CONST_TAG(idx, pool);
			if (n != CONSTANT_Class && n != CONSTANT_ResolvedClass) {
				verifyError(v, "new indexes a constant pool entry that is not type CONSTANT_Class or CONSTANT_ResolvedClass");
				return;
			}
			
			/* cannot create arrays with NEW */
			sig = CLASS_NAMED(idx, pool);
			if (*sig == '[') {
				verifyError(v, "new instruction used to create a new array");
				return;
			}
			break;
			
			
		case ANEWARRAY:
			ENSURE_NON_WIDE;
			
			GET_WIDX(idx, pc);
			n = CONST_TAG(idx, pool);
			if (n != CONSTANT_Class && n != CONSTANT_ResolvedClass) {
				verifyError(v, "anewarray indexes a constant pool entry that is not type CONSTANT_Class or CONSTANT_ResolvedClass");
				return;
			}
			
			/* count the number of dimensions of the array being created...it must be <= 255 */
			sig = CLASS_NAMED(idx, pool);
			for (n = 0; *sig == '['; sig++, n++);
			if (n > 255) {
				verifyError(v, "anewarray used to create an array of > 255 dimensions");
				return;
			}
			
			break;
			
		case NEWARRAY:
			ENSURE_NON_WIDE;
			
			n = code[pc + 1];
			if (n < 4 || n > 11) {
				verifyError(v, "newarray operand must be in the range [4,11]");
				return;
			}
			
			break;
			
			
			
			/***********************************************************
			 * Instructions that can be modified by WIDE
			 ***********************************************************/
		case WIDE:
			ENSURE_NON_WIDE;
			wide = true;
			break;
			
			
		case ALOAD: case ASTORE:
		case ILOAD: case ISTORE:
		case FLOAD: case FSTORE:
			if (wide == true) {
			        /* the WIDE is considered the beginning of the instruction */
				v->status[pc] ^= IS_INSTRUCTION;
				v->status[pc] |= WIDE_MODDED;
				
				pc++;
				wide = false;
				
				n = getWord(code, pc);
			}
			else {
				n = code[pc + 1];
			}
			
			CHECK_LOCAL_INDEX(n);
			break;
			
		case LLOAD: case LSTORE:
		case DLOAD: case DSTORE:
			if (wide == true) {
			        /* the WIDE is considered the beginning of the instruction */
				v->status[pc] ^= IS_INSTRUCTION;
				v->status[pc] |= WIDE_MODDED;
				
				pc++;
				wide = false;
				
				n = getWord(code, pc);
			}
			else {
				GET_IDX(n, pc);
			}
			
			/* makes sure the index given for a local variable is within the correct index
			 *
			 * REM: longs and doubles take two consecutive local spots
			 */
			CHECK_LOCAL_INDEX(n + 1);
			break;
			
			
		case IINC:
			if (wide == true) {
			        /* the WIDE is considered the beginning of the instruction */
				v->status[pc] ^= IS_INSTRUCTION;
				v->status[pc] |= WIDE_MODDED;
				
				pc += 2;
				wide = false;
			}
			break;
			
			
			/********************************************************************
			 * BRANCHING INSTRUCTIONS
			 ********************************************************************/
		case GOTO:
			ENSURE_NON_WIDE;
			v->status[pc] |= END_BLOCK;
			
			n = pc + 1;
			branchoffset = getWord(code, n);
			newpc = pc + branchoffset;
			BRANCH_IN_BOUNDS(newpc, "goto");
			v->status[newpc] |= START_BLOCK;
			break;
			
		case GOTO_W:
			ENSURE_NON_WIDE;
			v->status[pc] |= END_BLOCK;
			
			n = pc + 1;
			branchoffset = getDWord(code, n);
			newpc = pc + branchoffset;
			BRANCH_IN_BOUNDS(newpc, "goto_w");
			v->status[newpc] |= START_BLOCK;
			break;
			
			
		case IF_ACMPEQ:  case IFNONNULL:
		case IF_ACMPNE:  case IFNULL:
		case IF_ICMPEQ:  case IFEQ:
		case IF_ICMPNE:	 case IFNE:
		case IF_ICMPGT:	 case IFGT:
		case IF_ICMPGE:	 case IFGE:
		case IF_ICMPLT:	 case IFLT:
		case IF_ICMPLE:	 case IFLE:
			ENSURE_NON_WIDE;
			v->status[pc] |= END_BLOCK;
			
			newpc = getNextPC(code, pc);
			BRANCH_IN_BOUNDS(newpc, "if<condition> = false");
			v->status[newpc] |= START_BLOCK;
			
			n            = pc + 1;
			branchoffset = getWord(code, n);
			newpc        = pc + branchoffset;
			BRANCH_IN_BOUNDS(newpc, "if<condition> = true");
			v->status[newpc] |= START_BLOCK;
			break;
			
			
		case JSR:
			newpc = pc + 1;
			newpc = pc + getWord(code, newpc);
			goto JSR_common;
		case JSR_W:
			newpc = pc + 1;
			newpc = pc + getDWord(code, newpc);
			
		JSR_common:
			ENSURE_NON_WIDE;
			v->status[pc] |= END_BLOCK;
			
			BRANCH_IN_BOUNDS(newpc, "jsr");
			v->status[newpc] |= START_BLOCK;
			
			/* the next instruction is a target for branching via RET */
			pc = getNextPC(code, pc);
			BRANCH_IN_BOUNDS(pc, "jsr/ret");
			v->status[pc] |= START_BLOCK;
			continue;
			
		case RET:
			v->status[pc] |= END_BLOCK;
			if (!wide) {
				GET_IDX(idx, pc);
			} else {
				GET_WIDX(idx, pc);
				
				v->status[pc] ^= IS_INSTRUCTION;
				v->status[pc] |= WIDE_MODDED;
				
				wide = false;
				pc += 2;
			}
			CHECK_LOCAL_INDEX(idx);
			pc = getNextPC(code, pc);
			continue;
			
			
		case LOOKUPSWITCH:
			ENSURE_NON_WIDE;
			v->status[pc] |= END_BLOCK;
			
			/* default branch...between 0 and 3 bytes of padding are added so that the
			 * default branch is at an address that is divisible by 4
			 */
			n = (pc + 1) % 4;
			if (n) n = pc + 5 - n;
			else   n = pc + 1;
			newpc = pc + getDWord(code, n);
			BRANCH_IN_BOUNDS(newpc, "lookupswitch");
			v->status[newpc] |= START_BLOCK;
			DBG(VERIFY3,
			    dprintf("          lookupswitch: pc = %d ... instruction = ", newpc);
			    printInstruction(code[newpc]);
			    dprintf("\n");
			    );
			
			/* get number of key/target pairs */
			n += 4;
			low = getDWord(code, n);
			if (low < 0) {
				verifyError(v, "lookupswitch with npairs < 0");
				return;
			}
			
			/* make sure all targets are in bounds */
			/* NOTE: the cast here is only there to keep gcc from complaining */
			for (n += 4, high = n + 8*low; n < (uint32)high; n += 8) {
				newpc = pc + getDWord(code, n+4);
				BRANCH_IN_BOUNDS(newpc, "lookupswitch");
				v->status[newpc] |= START_BLOCK;
				
				DBG(VERIFY3,
				    dprintf("          lookupswitch: pc = %d ... instruction = ", newpc);
				    printInstruction(code[newpc]);
				    dprintf("\n");
				    );
			}
			
			pc = high;
			continue;
			
			
		case TABLESWITCH:
			ENSURE_NON_WIDE;
			v->status[pc] |= END_BLOCK;
			
			/* default branch...between 0 and 3 bytes of padding are added so that the
			 * default branch is at an address that is divisible by 4
			 */
			n = (pc + 1) % 4;
			if (n) n = pc + 5 - n;
			else   n = pc + 1;
			newpc = pc + getDWord(code, n);
			BRANCH_IN_BOUNDS(newpc, "tableswitch");
			v->status[newpc] |= START_BLOCK;
			DBG(VERIFY3,
			    dprintf("          tableswitch: pc = %d ... instruction = ", newpc);
			    printInstruction(code[newpc]);
			    dprintf("\n");
			    );
			
			/* get the high and low values of the table */
			low  = getDWord(code, n + 4);
			high = getDWord(code, n + 8);
			if (high < low) {
				DBG(VERIFY3, dprintf("ERROR: low = %d, high = %d\n", low, high); );
				verifyError(v, "tableswitch high val < low val");
				return;
			}
			n += 12;
			
			/* high and low are used as temps in this loop that checks
			 * the validity of all the branches in the table
			 */
			/* NOTE: the cast is only to keep gcc from complaining */
			for (high = n + 4*(high - low + 1); n < (uint32)high; n += 4) {
				newpc = pc + getDWord(code, n);
				BRANCH_IN_BOUNDS(newpc, "tableswitch");
				v->status[newpc] |= START_BLOCK;
				
				DBG(VERIFY3,
				    dprintf("          tableswitch: pc = %d ... instruction = ", newpc);
				    printInstruction(code[newpc]);
				    dprintf("\n");
				    );
			}
			
			pc = high;
			continue;
			
			
			/* the rest of the ways to end a block */
		case RETURN:
		case ARETURN:
		case IRETURN:
		case FRETURN:
		case LRETURN:
		case DRETURN:
		case ATHROW:
			ENSURE_NON_WIDE;
			v->status[pc] |= END_BLOCK;
			break;
			
			
		default:
			if (wide == true) {
				verifyError(v, "illegal instruction following wide instruction");
				return;
			}
		}
		
		pc = getNextPC(code, pc);
	}
	
	
	DBG(VERIFY3, dprintf("    Verifier Pass 3a: second pass to locate illegal branches and count blocks...\n"); );
	
	/* newpc is going to stand for the PC of the previous instruction */
	for (newpc = 0, pc = 0; pc < codelen; pc++) {
		if (v->status[pc] & IS_INSTRUCTION) {
			if (v->status[pc] & START_BLOCK) {
				blockCount++;
				
				if (newpc < pc) {
				        /* make sure that the previous instruction is
					 * marked as the end of a block (it would only
					 * have been marked so if it were some kind of
					 * branch).
					 */
					v->status[newpc] |= END_BLOCK;
				}
			}
			
			newpc = pc;
		}
		else if (v->status[pc] & START_BLOCK) {
			verifyError(v, "branch into middle of instruction");
			return;
		}
	}
	
	
	DBG(VERIFY3, dprintf("        perusing exception table\n"); );
	if (v->method->exception_table != 0) {
		jexceptionEntry *entry;
		for (n = 0; n < v->method->exception_table->length; n++) {
			entry = &(v->method->exception_table->entry[n]);

			pc = entry->start_pc;
			if (pc >= codelen) {
				verifyError(v, "try block is beyond bound of method code");
				return;
			}
			else if (!(v->status[pc] & IS_INSTRUCTION)) {
				verifyError(v, "try block starts in the middle of an instruction");
				return;
			}
			
			pc = entry->end_pc;
			if (pc <= entry->start_pc) {
				verifyError(v, "try block ends before its starts");
				return;
			}
			else if (pc > codelen) {
				verifyError(v, "try block ends beyond bound of method code");
				return;
			}
			else if (!(v->status[pc] & IS_INSTRUCTION)) {
				verifyError(v, "try block ends in the middle of an instruction");
				return;
			}
			
			pc = entry->handler_pc;
			if (pc >= codelen) {
				verifyError(v, "exception handler is beyond bound of method code");
				return;
			}
			else if (!(v->status[pc] & IS_INSTRUCTION)) {
				verifyError(v, "exception handler starts in the middle of an instruction");
				return;
			}
			
			v->status[pc] |= (EXCEPTION_HANDLER & START_BLOCK);
			
			
			/* verify properties about the clause
			 *
			 * if entry->catch_type == 0, it's a finally clause
			 */
			if (entry->catch_type != 0) {
				if (entry->catch_type == NULL) {
					entry->catch_type = getClass(entry->catch_idx, v->method->class, v->einfo);
				}
				if (entry->catch_type == NULL) {
					DBG(VERIFY3, dprintf("        ERROR: could not resolve catch type...\n"); );
					entry->catch_type = UNRESOLVABLE_CATCHTYPE;
					
					verifyError(v, "unresolvable catch type");
					return;
				}
				if (!instanceof(javaLangThrowable, entry->catch_type)) {
					verifyError(v, "Exception to be handled by exception handler is not a subclass of Java/Lang/Throwable");
					return;
				}
			}
		}
	}

	if (v->method->lvars != NULL) {
		for (n = 0; n < v->method->lvars->length; n++) {
			localVariableEntry *lve;

			lve = &v->method->lvars->entry[n];

			pc = lve->start_pc;
			if (pc >= codelen) {
				verifyError(v, "local variable is beyond bound of method code");
				return;
			}
			else if (!(v->status[pc] & IS_INSTRUCTION)) {
				verifyError(v, "local variable starts in the middle of an instruction");
				return;
			}
			
			if ((pc + lve->length) > codelen) {
				verifyError(v, "local variable is beyond bound of method code");
				return;
			}
		}
	}
	
	
	DBG(VERIFY3, dprintf("        done, %d blocks found.\n", blockCount); );
	
	
	DBG(VERIFY3, dprintf("    Verifier Pass 3a: third pass to allocate memory for basic blocks...\n"); );
	
	blocks = checkPtr((BlockInfo**)gc_malloc(blockCount * sizeof(BlockInfo*), KGC_ALLOC_VERIFIER));
	
	for (inABlock = true, n = 0, pc = 0; pc < codelen; pc++) {
		if (v->status[pc] & START_BLOCK) {
			blocks[n] = createBlock(v->method);
			blocks[n]->startAddr = pc;
			n++;
			
			inABlock = true;
			
			
			DBG(VERIFY3, dprintf("        setting blocks[%d]->startAddr = %d\n",
					     n-1, blocks[n-1]->startAddr); );
		}
		
		if (inABlock && (v->status[pc] & END_BLOCK)) {
			blocks[n-1]->lastAddr = pc;
			
			inABlock = false;
			
			
			DBG(VERIFY3, dprintf("        setting blocks[%d]->lastAddr = %d\n",
					     n-1, blocks[n-1]->lastAddr); );
		}
	}
	
	
	DBG(VERIFY3, dprintf("    Verifier Pass 3a: done\n"); );
	
	v->numBlocks = blockCount;
	v->blocks = blocks;
	
	
#undef CHECK_LOCAL_INDEX	
#undef BRANCH_IN_BOUNDS
#undef GET_IDX
#undef GET_WIDX
#undef CHECK_POOL_IDX
#undef ENSURE_NON_WIDE
}
