
/*
 * verify-block.c
 *
 * Copyright 2004, 2005
 *   Kaffe.org contributors. See ChangeLog for details. All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Code for handing of blocks in the verifier.
 */

#include "config.h"

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include "bytecode.h"
#include "baseClasses.h"
#include "classMethod.h"
#include "code.h"
#include "constants.h"
#include "debug.h"
#include "errors.h"
#include "exception.h"
#include "gc.h"
#include "itypes.h"

#include "verify.h"
#include "verify-block.h"
#include "verify-debug.h"
#include "verify-errors.h"
#include "verify-sigstack.h"
#include "verify-type.h"
#include "verify-uninit.h"


/*********************************************************
 * BASIC BLOCK MEMORY MANAGEMENT
 *********************************************************/

/*
 * allocate memory for a block info and fill in with default values
 */
BlockInfo*
createBlock(const Method* method)
{
	int i;
	
	BlockInfo* binfo = checkPtr((BlockInfo*)gc_malloc(sizeof(BlockInfo), KGC_ALLOC_VERIFIER));
	
	binfo->startAddr   = 0;
	binfo->status      = IS_INSTRUCTION | START_BLOCK;  /* not VISITED or CHANGED */
	
	/* allocate memory for locals */
	if (method->localsz > 0) {
		binfo->locals = checkPtr(gc_malloc(method->localsz * sizeof(Type), KGC_ALLOC_VERIFIER));
		
		for (i = 0; i < method->localsz; i++) {
			binfo->locals[i] = *getTUNSTABLE();
		}
	} else {
		binfo->locals = NULL;
	}
	
	
	/* allocate memory for operand stack */
	binfo->stacksz = 0;
	if (method->stacksz > 0) {
		binfo->opstack = checkPtr(gc_malloc(method->stacksz * sizeof(Type), KGC_ALLOC_VERIFIER));
		
		for (i = 0; i < method->stacksz; i++) {
			binfo->opstack[i] = *getTUNSTABLE();
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


/*********************************************************
 * BASIC BLOCK VERIFICATION
 *********************************************************/
/*
 * Helper function for error reporting in ENSURE_LOCAL_TYPE macro in verifyBasicBlock.
 */
static inline
bool
ensureLocalTypeErrorInVerifyBasicBlock(Verifier* v,
				       const BlockInfo* block,
				       const unsigned int n)
{
	if (block->locals[n].data.class == getTUNSTABLE()->data.class) {
		return verifyError(v, "attempt to access an unstable local variable");
	} else {
		return verifyError(v, "attempt to access a local variable not of the correct type");
	}
}

/*
 * Helper function for error reporting in ENSURE_OPSTACK_SIZE macro in verifyBasicBlock.
 */
static
bool
ensureOpstackSizeErrorInVerifyBasicBlock(Verifier* v,
					 const BlockInfo* block)

{
	DBG(VERIFY3,
	    dprintf("                here's the stack: \n");
	    printBlock(v->method, block, "                    ");
	    );
	return verifyError(v, "not enough items on stack for operation");
}

/*
 * Helper function for error reporting in CHECK_STACK_OVERFLOW macro in verifyBasicBlock.
 */
static inline
bool
checkStackOverflowErrorInVerifyBasicBlock(Verifier* v,
					  const BlockInfo* block,
					  const unsigned int n)
{
	DBG(VERIFY3,
	    dprintf("                block->stacksz: %d :: N = %d :: method->stacksz = %d\n",
		    block->stacksz,
		    n,
		    v->method->stacksz);
	    );
	DBG(VERIFY3,
	    dprintf("                here's the stack: \n");
	    printBlock(v->method, block, "                    ");
	    );
	return verifyError(v, "stack overflow");
}

/*
 * Helper function for opstack access in verifyBasicBlock.
 *
 * @return nth item on the operand stack from the top.
 */
static inline
Type *
getOpstackItem(BlockInfo* block,
	       unsigned int n)
{
	return (&block->opstack[block->stacksz - n]);
}

/*
 * Helper function for opstack access in verifyBasicBlock.
 *
 * @return first item on the operand stack from the top.
 */
static inline
Type *
getOpstackTop(BlockInfo* block)
{
	return getOpstackItem(block, 1);
}

/*
 * Helper function for opstack access in verifyBasicBlock.
 *
 * @return second item on the operand stack from the top.
 */
static inline
Type *
getOpstackWTop(BlockInfo* block)
{
	return getOpstackItem(block, 2);
}

/*
 * Helper function for opstack access in verifyBasicBlock.
 */
static inline
void
opstackPushBlind(BlockInfo* block,
		 const Type* type)
{
	block->opstack[block->stacksz++] = *(type);
}

/*
 * Helper function for opstack access in verifyBasicBlock.
 * only use for LONGs and DOUBLEs.
 */
static inline
void
opstackWPushBlind(BlockInfo* block,
		  const Type* type)
{
	opstackPushBlind(block, type);
	opstackPushBlind(block, getTWIDE());
}

/*
 * Helper function for opstack access in verifyBasicBlock.
 */
static inline
void
opstackPopBlind(BlockInfo* block)
{
	block->stacksz--;
	block->opstack[block->stacksz] = *getTUNSTABLE();
}

/*
 * Helper function for opstack access in verifyBasicBlock.
 */
static inline
void
opstackWPopBlind(BlockInfo* block)
{
	opstackPopBlind(block);
	opstackPopBlind(block);
}

/*
 * Helper function for opstack access in verifyBasicBlock.
 * pop _N things off the stack off the stack.
 */
static inline
void
opstackPopNBlind(BlockInfo* block,
		 unsigned int n)
{
	unsigned int i;

	for (i = 0; i < n; ++i) {
		opstackPopBlind(block);
	}
}

/*
 * Helper function for error reporting in OPSTACK_PEEK_T_BLIND macro in verifyBasicBlock.
 */
static
bool
opstackPeekTBlindErrorInVerifyBasicBlock(Verifier* v,
					 BlockInfo* block,
					 const Type* type)
{
	DBG(VERIFY3,
	    dprintf("                OPSTACK_TOP: ");
	    printType(getOpstackTop(block));
	    dprintf(" vs. what's we wanted: ");
	    printType(type);
	    dprintf("\n");
	    );
	return verifyError(v, "top of opstack does not have desired type");
}

/*
 * verifyBasicBlock()
 *   Simulates execution of a basic block by modifying its
 *   simulated operand stack and local variable array.
 *
 * TODO: turn at least some of the macros into static
 * inline methods
 */
bool
verifyBasicBlock(Verifier* v, BlockInfo* block)
{
	/**************************************************************************************************
	 * VARIABLES
	 **************************************************************************************************/
	uint32            pc   = 0;
	unsigned char*    code = METHOD_BYTECODE_CODE(v->method);
	
	bool wide = false;       /* was the previous opcode a WIDE instruction? */
	
	uint32 n = 0;            /* used as a general temporary variable, often as a temporary pc */
	
	Type* type = NULL;
	Type* arrayType = NULL;
	Hjava_lang_Class* class; /* for when we need a pointer to an actual class */
	
	/* for the rare occasions when we actually need a Type */
	Type  tt;
	Type* t = &tt;
	
	int tag;                 /* used for constant tag stuff */
	
	uint32     idx;          /* index into constant pool */
	const constants* pool = CLASS_CONSTANTS(v->class);
	
	const char* sig;
	
	
	/**************************************************************************************************
	 * HANDY MACROS USED ONLY IN THIS METHOD
	 *    most of these belong to one of two categories:
	 *         - those dealing with locals variables
	 *         - those dealing with the operand stack
	 **************************************************************************************************/

#define GET_IDX \
	idx = getIdx(code, pc)
	
#define GET_WIDX \
	idx = getWIdx(code, pc)
	
	
	/* checks whether the specified local variable is of the specified type. */
#define ENSURE_LOCAL_TYPE(_N, _TINFO) \
	if (!typecheck(v, (_TINFO), &block->locals[_N])) { \
		return ensureLocalTypeErrorInVerifyBasicBlock(v, block, _N); \
	} 
	
	/* only use with TLONG and TDOUBLE */
#define ENSURE_LOCAL_WTYPE(_N, _TINFO) \
	if (block->locals[_N].data.class != (_TINFO)->data.class) { \
		return verifyError(v, "local variable not of correct type"); \
	} \
	else if (block->locals[_N + 1].data.class != getTWIDE()->data.class) { \
		return verifyError(v, "accessing a long or double in a local where the following local has been corrupted"); \
	}

	
#define ENSURE_OPSTACK_SIZE(_N) \
	if (block->stacksz < (_N)) { \
		return ensureOpstackSizeErrorInVerifyBasicBlock(v, block); \
	}

#define CHECK_STACK_OVERFLOW(_N) \
	if (block->stacksz + _N > v->method->stacksz) { \
		return checkStackOverflowErrorInVerifyBasicBlock(v, block, _N); \
	}
	
#define OPSTACK_PUSH(_TINFO) \
	CHECK_STACK_OVERFLOW(1); \
	opstackPushBlind(block, _TINFO)

#define OPSTACK_WPUSH(_T) \
	CHECK_STACK_OVERFLOW(2); \
        opstackWPushBlind(block, _T)
	
	
	
	/* ensure that the top item on the stack is of type _T	*/
#define OPSTACK_PEEK_T_BLIND(_TINFO) \
	if (!typecheck(v, _TINFO, getOpstackTop(block))) { \
		return opstackPeekTBlindErrorInVerifyBasicBlock(v, block, _TINFO); \
	}
	
#define OPSTACK_PEEK_T(_TINFO) \
        ENSURE_OPSTACK_SIZE(1); \
	OPSTACK_PEEK_T_BLIND(_TINFO)
	
	/* ensure that the top item on the stack is of wide type _T
	 * this only works with doubles and longs
	 */
#define OPSTACK_WPEEK_T_BLIND(_TINFO) \
	if (getOpstackTop(block)->data.class != getTWIDE()->data.class) { \
		return verifyError(v, "trying to pop a wide value off operand stack where there is none"); \
	} else if (getOpstackWTop(block)->data.class != (_TINFO)->data.class) { \
		return verifyError(v, "mismatched stack types"); \
	}
	
#define OPSTACK_WPEEK_T(_TINFO) \
	ENSURE_OPSTACK_SIZE(2); \
	OPSTACK_WPEEK_T_BLIND(_TINFO)
	
#define OPSTACK_POP \
        ENSURE_OPSTACK_SIZE(1); \
	opstackPopBlind(block)

	/* pop a type off the stack and typecheck it */
#define OPSTACK_POP_T_BLIND(_TINFO) \
	OPSTACK_PEEK_T_BLIND(_TINFO); \
	opstackPopBlind(block)

#define OPSTACK_POP_T(_TINFO) \
	OPSTACK_PEEK_T(_TINFO); \
        opstackPopBlind(block)

#define OPSTACK_WPOP \
	ENSURE_OPSTACK_SIZE(2); \
	opstackWPopBlind(block)

	/* pop a wide type off the stack and typecheck it */
#define OPSTACK_WPOP_T_BLIND(_TINFO) \
	OPSTACK_WPEEK_T_BLIND(_TINFO); \
	opstackWPopBlind(block)

#define OPSTACK_WPOP_T(_TINFO) \
        OPSTACK_WPEEK_T(_TINFO); \
	opstackWPopBlind(block)
	
#define OPSTACK_POP_N(_N) \
        ENSURE_OPSTACK_SIZE(_N); \
	opstackPopNBlind(block, _N)
	
	
	
	/**************************************************************************************************
	 * BLOCK-LEVEL DATA FLOW ANALYASIS
	 *    this is actually pretty easy, since there are never any branches.  basically, it just
	 *    manipulates the working stack after every instruction as if it were actually running the
	 *    code so that, after verifying the block, the working block can be used to merge this block
	 *    with its successors.
	 **************************************************************************************************/
	DBG(VERIFY3,
	    dprintf("        about to verify the block...\n");
	    dprintf("        block->startAddr = %d, block->lastAddr = %d, first instruction = %d\n",
		    block->startAddr, block->lastAddr, code[block->startAddr]);
	    );
	
	pc = block->startAddr;
	while (pc <= block->lastAddr) {
		DBG(VERIFY3,
		    dprintf("            pc = %d, opcode = %d == ", pc, code[pc]);
		    printInstruction(code[pc]);
		    dprintf("\n");
		    );
		
		switch(code[pc]) {
			/**************************************************************
			 * INSTRUCTIONS FOR PUSHING CONSTANTS ONTO THE STACK
			 **************************************************************/
			/* pushes NULL onto the stack, which matches any object */
		case ACONST_NULL:
			OPSTACK_PUSH(getTNULL());
			break;
			
			/* iconst_<n> pushes n onto the stack */
		case ICONST_0: case ICONST_1: case ICONST_2:
		case ICONST_3: case ICONST_4: case ICONST_5:
			
		case ICONST_M1: /* pushes -1 onto the stack */
		case BIPUSH:    /* sign extends an 8-bit int to 32-bits and pushes it onto stack */
		case SIPUSH:    /* sign extends a 16-bit int to 32-bits and pushes it onto stack */
			OPSTACK_PUSH(getTINT());
			break;
			
		case FCONST_0:
		case FCONST_1:
		case FCONST_2:
			OPSTACK_PUSH(getTFLOAT());
			break;
			
		case LCONST_0:
		case LCONST_1:
			OPSTACK_WPUSH(getTLONG());
			break;
			
		case DCONST_0:
		case DCONST_1:
			OPSTACK_WPUSH(getTDOUBLE());
			break;
			
			
		case LDC1:
			GET_IDX;
			goto LDC_common;
		case LDC2:
			GET_WIDX;
		LDC_common:
			tag = CONST_TAG(idx, pool);
			switch(tag) {
			case CONSTANT_Integer: OPSTACK_PUSH(getTINT());    break;
			case CONSTANT_Float:   OPSTACK_PUSH(getTFLOAT());  break;
			case CONSTANT_ResolvedString:
			case CONSTANT_String:
			        /* we do this because we might be loading a class before
				 * loading String
				 */
				OPSTACK_PUSH(getTSTRING());
				break;
			default:
				break;
			}
			break;
			
		case LDC2W:
			GET_WIDX;
			tag = CONST_TAG(idx, pool);
			if (tag == CONSTANT_Long) {
				OPSTACK_WPUSH(getTLONG());
			} else {
				OPSTACK_WPUSH(getTDOUBLE());
			}
			break;
			
			
			/**************************************************************
			 * INSTRUCTIONS DEALING WITH THE LOCALS AND STACK
			 **************************************************************/
		case POP:
			OPSTACK_POP;
			break;
		case POP2:
			OPSTACK_WPOP;
			break;
			
			
#define GET_CONST_INDEX \
			if (wide == true) { GET_WIDX; } \
			else              { GET_IDX;  }
			
			
			/* aload_<n> takes the object reference in location <n> and pushes it onto the stack */
		case ALOAD_0: idx = 0; goto ALOAD_common;
		case ALOAD_1: idx = 1; goto ALOAD_common;
		case ALOAD_2: idx = 2; goto ALOAD_common;
		case ALOAD_3: idx = 3; goto ALOAD_common;
		case ALOAD:
			GET_CONST_INDEX;
		ALOAD_common:
			if (!isReference(&block->locals[idx])) {
				DBG(VERIFY3, dprintf("%sERROR: ", indent); printType(&block->locals[idx]); dprintf("\n"); );
				return verifyError(v, "aload<_n> where local variable does not contain an object reference");
			}
			
			OPSTACK_PUSH(&block->locals[idx]);
			break;
			
			
			/* stores whatever's on the top of the stack in local <n> */
		case ASTORE_0: idx = 0; goto ASTORE_common;
		case ASTORE_1: idx = 1; goto ASTORE_common;
		case ASTORE_2: idx = 2; goto ASTORE_common;
		case ASTORE_3: idx = 3; goto ASTORE_common;
		case ASTORE:
			GET_CONST_INDEX;
		ASTORE_common:
			ENSURE_OPSTACK_SIZE(1);
			type = getOpstackTop(block);
			
			if (!IS_ADDRESS(type) && !isReference(type)) {
				return verifyError(v, "astore: top of stack is not a return address or reference type");
			}
			
			block->locals[idx] = *type;
			opstackPopBlind(block);
			break;
			
			
			
			/* iload_<n> takes the variable in location <n> and pushes it onto the stack */
		case ILOAD_0: idx = 0; goto ILOAD_common;
		case ILOAD_1: idx = 1; goto ILOAD_common;
		case ILOAD_2: idx = 2; goto ILOAD_common;
		case ILOAD_3: idx = 3; goto ILOAD_common;
		case ILOAD:
			GET_CONST_INDEX;
		ILOAD_common:
			ENSURE_LOCAL_TYPE(idx, getTINT());
			OPSTACK_PUSH(getTINT());
			break;
			
			
		case ISTORE_0: idx =0; goto ISTORE_common;
		case ISTORE_1: idx =1; goto ISTORE_common;
		case ISTORE_2: idx =2; goto ISTORE_common;
		case ISTORE_3: idx =3; goto ISTORE_common;
		case ISTORE:
			GET_CONST_INDEX;
		ISTORE_common:
			OPSTACK_POP_T(getTINT());
			block->locals[idx] = *getTINT();
			break;
			
			
			/* fload_<n> takes the variable at location <n> and pushes it onto the stack */
		case FLOAD_0: idx =0; goto FLOAD_common;
		case FLOAD_1: idx =1; goto FLOAD_common;
		case FLOAD_2: idx =2; goto FLOAD_common;
		case FLOAD_3: idx = 3; goto FLOAD_common;
		case FLOAD:
			GET_CONST_INDEX;
		FLOAD_common:
			ENSURE_LOCAL_TYPE(idx, getTFLOAT());
			OPSTACK_PUSH(getTFLOAT());
			break;
			
			
			/* stores a float from top of stack into local <n> */
		case FSTORE_0: idx = 0; goto FSTORE_common;
		case FSTORE_1: idx = 1; goto FSTORE_common;
		case FSTORE_2: idx = 2; goto FSTORE_common;
		case FSTORE_3: idx = 3; goto FSTORE_common;
		case FSTORE:
			GET_CONST_INDEX;
		FSTORE_common:
			OPSTACK_POP_T(getTFLOAT());
			block->locals[idx] = *getTFLOAT();
			break;
			
			
			/* lload_<n> takes the variable at location <n> and pushes it onto the stack */
		case LLOAD_0: idx = 0; goto LLOAD_common;
		case LLOAD_1: idx = 1; goto LLOAD_common;
		case LLOAD_2: idx = 2; goto LLOAD_common;
		case LLOAD_3: idx = 3; goto LLOAD_common;
		case LLOAD:
			GET_CONST_INDEX;
		LLOAD_common:
			ENSURE_LOCAL_WTYPE(idx, getTLONG());
			OPSTACK_WPUSH(getTLONG());
			break;
			
			
			/* lstore_<n> stores a long from top of stack into local <n> */
		case LSTORE_0: idx = 0; goto LSTORE_common;
		case LSTORE_1: idx = 1; goto LSTORE_common;
		case LSTORE_2: idx = 2; goto LSTORE_common;
		case LSTORE_3: idx = 3; goto LSTORE_common;
		case LSTORE:
			GET_CONST_INDEX;
		LSTORE_common:
			OPSTACK_WPOP_T(getTLONG());
			block->locals[idx] = *getTLONG();
			block->locals[idx + 1] = *getTWIDE();
			break;
			
			
			/* dload_<n> takes the double at local <n> and pushes it onto the stack */
		case DLOAD_0: idx = 0; goto DLOAD_common;
		case DLOAD_1: idx = 1; goto DLOAD_common;
		case DLOAD_2: idx = 2; goto DLOAD_common;
		case DLOAD_3: idx = 3; goto DLOAD_common;
		case DLOAD:
			GET_CONST_INDEX;
		DLOAD_common:
			ENSURE_LOCAL_WTYPE(idx, getTDOUBLE());
			OPSTACK_WPUSH(getTDOUBLE());
			break;
			
			
			/* dstore stores a double from the top of stack into a local variable */
		case DSTORE_0: idx = 0; goto DSTORE_common;
		case DSTORE_1: idx = 1; goto DSTORE_common;
		case DSTORE_2: idx = 2; goto DSTORE_common;
		case DSTORE_3: idx = 3; goto DSTORE_common;
		case DSTORE:
			GET_CONST_INDEX;
		DSTORE_common:
			OPSTACK_WPOP_T(getTDOUBLE());
			block->locals[idx] = *getTDOUBLE();
			block->locals[idx + 1] = *getTWIDE();
			break;
			
			
#undef GET_CONST_INDEX
			/**************************************************************
			 * ARRAY INSTRUCTIONS!
			 **************************************************************/
			/* i put ANEWARRAY code by NEW instead of in the array instructions
			 * section because of similarities with NEW
			
			 * for creating a primitive array
			 */
		case NEWARRAY:
		        OPSTACK_POP_T(getTINT());   /* array size */
			
			switch(code[pc + 1]) {
			case TYPE_Boolean: OPSTACK_PUSH(getTBOOLARR());   break;
			case TYPE_Char:    OPSTACK_PUSH(getTCHARARR());   break;
			case TYPE_Float:   OPSTACK_PUSH(getTFLOATARR());  break;
			case TYPE_Double:  OPSTACK_PUSH(getTDOUBLEARR()); break;
			case TYPE_Byte:    OPSTACK_PUSH(getTBYTEARR());   break;
			case TYPE_Short:   OPSTACK_PUSH(getTSHORTARR());  break;
			case TYPE_Int:     OPSTACK_PUSH(getTINTARR());    break;
			case TYPE_Long:    OPSTACK_PUSH(getTLONGARR());   break;
			default: return verifyError(v, "newarray of unknown type");
			}
			break;
			
		case ARRAYLENGTH:
			ENSURE_OPSTACK_SIZE(1);
			
			type = getOpstackTop(block);
			if (!isArray(type)) {
				DBG(VERIFY3, dprintf("%stype = ", indent); printType(type); dprintf("\n"); );
				return verifyError(v, "arraylength: top of operand stack is not an array");
			}
			
			*type = *getTINT();
			break;
			
			
#define ARRAY_LOAD(_T, _ARRT) \
                                OPSTACK_POP_T(getTINT()); \
                                OPSTACK_POP_T(_ARRT); \
				OPSTACK_PUSH(_T);

#define ARRAY_WLOAD(_T, _ARRT) \
                                OPSTACK_POP_T(getTINT()); \
                                OPSTACK_POP_T(_ARRT); \
				OPSTACK_WPUSH(_T);
			
			
		case AALOAD:
			ENSURE_OPSTACK_SIZE(2);
			
			if (getOpstackTop(block)->data.class != getTINT()->data.class) {
				return verifyError(v, "aaload: item on top of stack is not an integer");
			}
			opstackPopBlind(block);
			
			type = getOpstackTop(block);
			if (!isArray(type)) {
				DBG(VERIFY3, dprintf("%serror: type = ", indent); printType(type); dprintf("\n"); );
				return verifyError(v, "aaload: top of operand stack is not an array");
			}
			
			if (type->tinfo & TINFO_NAME || type->tinfo & TINFO_SIG) {
				type->tinfo = TINFO_SIG;
				(type->data.sig)++;
			}
			else if (!isNull(type)) {
				type->tinfo = TINFO_SIG;
				type->data.sig = CLASS_CNAME(type->data.class) + 1;
			}
			DBG(VERIFY3, dprintf("%sarray type: ", indent); printType(type); dprintf("\n"); );
			break;
			
		case IALOAD: ARRAY_LOAD(getTINT(),   getTINTARR());   break;
		case FALOAD: ARRAY_LOAD(getTFLOAT(), getTFLOATARR()); break;
		case CALOAD: ARRAY_LOAD(getTINT(),   getTCHARARR());  break;
		case SALOAD: ARRAY_LOAD(getTINT(),   getTSHORTARR()); break;
			
		case LALOAD: ARRAY_WLOAD(getTLONG(),   getTLONGARR());   break;
		case DALOAD: ARRAY_WLOAD(getTDOUBLE(), getTDOUBLEARR()); break;
#undef ARRAY_LOAD
#undef ARRAY_WLOAD

		case BALOAD:
			/* BALOAD can be used for bytes or booleans .... */
			OPSTACK_POP_T(getTINT());

			if (!typecheck (v, getTBYTEARR(), getOpstackTop(block)) &&
			    !typecheck (v, getTBOOLARR(), getOpstackTop(block))) {
                                DBG(VERIFY3,
                                    dprintf("                OPSTACK_TOP: ");
                                    printType(getOpstackTop(block));
                                    dprintf(" vs. what's we wanted: TBYTEARR or TBOOLARR"); );
                                return verifyError(v, "top of opstack does not have desired type");
			}

			opstackPopBlind(block);
			OPSTACK_PUSH(getTINT());
			break;


		case AASTORE:
		        /* the runtime value of the type on the top of the stack must be
			 * assignment compatible with the type of the array
			 */
			ENSURE_OPSTACK_SIZE(3);
			
			if (getOpstackItem(block, 2)->data.class != getTINT()->data.class) {
				return verifyError(v, "aastore: array index is not an integer");
			}
			
			type      = getOpstackItem(block, 1);
			arrayType = getOpstackItem(block, 3);
			
			DBG(VERIFY3,
			    dprintf("%sarrayType: ", indent); printType(arrayType);
			    dprintf(" vs. type: "); printType(type);
			    dprintf("\n");
			    );
			
			if (!isArray(arrayType)) {
				DBG(VERIFY3, dprintf("%serror: type = ", indent); printType(type); dprintf("\n"); );
				return verifyError(v, "aastore: top of operand stack is not an array");
			}
			
			if (arrayType->tinfo & TINFO_NAME || arrayType->tinfo & TINFO_SIG) {
				arrayType->tinfo = TINFO_SIG;
				(arrayType->data.sig)++;
			}
			else {
				if (arrayType->data.class == getTOBJARR()->data.class) {
					*arrayType = *getTOBJ();
				} else if (!isNull(arrayType)) {
					arrayType->tinfo = TINFO_SIG;
					arrayType->data.sig = CLASS_CNAME(arrayType->data.class) + 1;
				}
			}
			
			if (!typecheck(v, arrayType, type)) {
				return verifyError(v, "attempting to store incompatible type in array");
			}
			
			opstackPopNBlind(block, 3);
			break;

#define ARRAY_STORE(_T, _ARRT) \
				OPSTACK_POP_T(_T); \
				OPSTACK_POP_T(getTINT()); \
				OPSTACK_POP_T(_ARRT);
			
#define ARRAY_WSTORE(_T, _ARRT) \
				OPSTACK_WPOP_T(_T); \
				OPSTACK_POP_T(getTINT()); \
				OPSTACK_POP_T(_ARRT);
			
			
			
			
		case IASTORE: ARRAY_STORE(getTINT(),   getTINTARR());   break;
		case FASTORE: ARRAY_STORE(getTFLOAT(), getTFLOATARR()); break;
		case CASTORE: ARRAY_STORE(getTINT(),   getTCHARARR());  break;
		case SASTORE: ARRAY_STORE(getTINT(),   getTSHORTARR()); break;
			
		case LASTORE: ARRAY_WSTORE(getTLONG(),   getTLONGARR());   break;
		case DASTORE: ARRAY_WSTORE(getTDOUBLE(), getTDOUBLEARR()); break;
#undef ARRAY_STORE
#undef ARRAY_WSTORE

		case BASTORE: 
			/* BASTORE can store either bytes or booleans .... */
			OPSTACK_POP_T(getTINT());
			OPSTACK_POP_T(getTINT());

			if ( !typecheck(v, getTBYTEARR(), getOpstackTop(block)) &&
			     !typecheck(v, getTBOOLARR(), getOpstackTop(block))) {
				DBG(VERIFY3,
				    dprintf("                OPSTACK_TOP: ");
				    printType(getOpstackTop(block));
				    dprintf(" vs. what's we wanted: TBYTEARR or TBOOLARR"); );
				return verifyError(v, "top of opstack does not have desired type");
			}
			opstackPopBlind(block);
			break;			
			
			
			/**************************************************************
			 * ARITHMETIC INSTRUCTIONS
			 **************************************************************/
		case IAND: case IOR:  case IXOR:
		case IADD: case ISUB: case IMUL: case IDIV: case IREM:
		case ISHL: case ISHR: case IUSHR:
			OPSTACK_POP_T(getTINT());
			break;
		case INEG:
			OPSTACK_PEEK_T(getTINT());
			break;
			
			
		case LAND: case LOR:  case LXOR:
		case LADD: case LSUB: case LMUL: case LDIV: case LREM:
			OPSTACK_WPOP_T(getTLONG());
			break;
		case LNEG:
			OPSTACK_WPEEK_T(getTLONG());
			break;
			
		case LSHL: case LSHR: case LUSHR:
			OPSTACK_POP_T(getTINT());
			OPSTACK_WPEEK_T(getTLONG());
			break;
			
			
		case FADD: case FSUB: case FMUL: case FDIV: case FREM:
			OPSTACK_POP_T(getTFLOAT());
			break;
		case FNEG:
			OPSTACK_PEEK_T(getTFLOAT());
			break;
			
			
		case DADD: case DSUB: case DDIV: case DMUL: case DREM:
			OPSTACK_WPOP_T(getTDOUBLE());
			break;
		case DNEG:
			OPSTACK_WPEEK_T(getTDOUBLE());
			break;
			
			
		case LCMP:
			OPSTACK_WPOP_T(getTLONG());
			OPSTACK_WPOP_T(getTLONG());
			opstackPushBlind(block, getTINT());
			break;
			
		case FCMPG:
		case FCMPL:
			OPSTACK_POP_T(getTFLOAT());
			OPSTACK_POP_T(getTFLOAT());
			opstackPushBlind(block, getTINT());
			break;
				
		case DCMPG:
		case DCMPL:
			OPSTACK_WPOP_T(getTDOUBLE());
			OPSTACK_WPOP_T(getTDOUBLE());
			opstackPushBlind(block, getTINT());
			break;
			
			
		case IINC:
			if (wide == true) { GET_WIDX; }
			else              { GET_IDX; }
			
			ENSURE_LOCAL_TYPE(idx, getTINT());
			
			pc = getNextPC(code, pc);
			if (wide == true) {
				pc += 2;
				wide = false;
			}
			continue;
			
			
			/**************************************************************
			 * PRIMITIVE CONVERSION STUFF
			 **************************************************************/
		case INT2BYTE:
		case INT2CHAR:
		case INT2SHORT:
			OPSTACK_PEEK_T(getTINT());
			break;
			
		case I2F:
			OPSTACK_POP_T(getTINT());
			opstackPushBlind(block, getTFLOAT());
			break;
		case I2L:
			OPSTACK_POP_T(getTINT());
			CHECK_STACK_OVERFLOW(2);
			opstackWPushBlind(block, getTLONG());
			break;
		case I2D:
			OPSTACK_POP_T(getTINT());
			CHECK_STACK_OVERFLOW(2);
			opstackWPushBlind(block, getTDOUBLE());
			break;
			
		case F2I:
			OPSTACK_POP_T(getTFLOAT());
			opstackPushBlind(block, getTINT());
			break;
		case F2L:
			OPSTACK_POP_T(getTFLOAT());
			OPSTACK_WPUSH(getTLONG());
			break;
		case F2D:
			OPSTACK_POP_T(getTFLOAT());
			OPSTACK_WPUSH(getTDOUBLE());
			break;
			
		case L2I:
			OPSTACK_WPOP_T(getTLONG());
			opstackPushBlind(block, getTINT());
			break;
		case L2F:
			OPSTACK_WPOP_T(getTLONG());
			opstackPushBlind(block, getTFLOAT());
			break;
		case L2D:
			OPSTACK_WPOP_T(getTLONG());
			opstackWPushBlind(block, getTDOUBLE());
			break;
			
		case D2I:
			OPSTACK_WPOP_T(getTDOUBLE());
			opstackPushBlind(block, getTINT());
			break;
		case D2F:
			OPSTACK_WPOP_T(getTDOUBLE());
			opstackPushBlind(block, getTFLOAT());
			break;
		case D2L:
			OPSTACK_WPOP_T(getTDOUBLE());
			opstackWPushBlind(block, getTLONG());
			break;
			
			
			
			/**************************************************************
			 * OBJECT CREATION/TYPE CHECKING
			 **************************************************************/
		case INSTANCEOF:
			ENSURE_OPSTACK_SIZE(1);
			if (!isReference(getOpstackItem(block, 1))) {
				return verifyError(v, "instanceof: top of stack is not a reference type");
			}
			*getOpstackTop(block) = *getTINT();
			break;
			
		case CHECKCAST:
			ENSURE_OPSTACK_SIZE(1);
			opstackPopBlind(block);
			goto NEW_COMMON;
			
		case MULTIANEWARRAY:
			n = code[pc + 3];
			ENSURE_OPSTACK_SIZE(n);
			while (n > 0) {
				if (getOpstackTop(block)->data.class != getTINT()->data.class) {
					return verifyError(v, "multinewarray: first <n> things on opstack must be integers");
				}
				opstackPopBlind(block);
				n--;
			}
			goto NEW_COMMON;
			
		NEW_COMMON:
			GET_WIDX;
			
			CHECK_STACK_OVERFLOW(1);
			block->stacksz++;
			type = getOpstackTop(block);
			
			if (pool->tags[idx] == CONSTANT_ResolvedClass) {
				type->tinfo = TINFO_CLASS;
				type->data.class = CLASS_CLASS(idx, pool);
			} else {
				const char* namestr;
				
				namestr = CLASS_NAMED(idx, pool);
				
				if (*namestr == '[') {
					type->tinfo = TINFO_SIG;
					type->data.sig = namestr;
				} else {
					type->tinfo = TINFO_NAME;
					type->data.sig = namestr;
				}
			}
			
			DBG(VERIFY3,
			    dprintf("%s", indent);
			    printType(getOpstackTop(block));
			    dprintf("\n"); );
			break;
			
		case NEW:
			GET_WIDX;
			
			CHECK_STACK_OVERFLOW(1);
			block->stacksz++;
			type = getOpstackTop(block);
			if (pool->tags[idx] == CONSTANT_ResolvedClass) {
				type->tinfo = TINFO_CLASS;
				type->data.class = CLASS_CLASS(idx, pool);
			} else {
				const char* namestr = CLASS_NAMED(idx, pool);
				
				if (*namestr == '[') {
					return verifyError(v, "new: used to create an array");
				}
				
				type->tinfo = TINFO_NAME;				
				type->data.name = namestr;
			}
			
			v->uninits = pushUninit(v->uninits, type);
			type->tinfo = TINFO_UNINIT;
			type->data.uninit  = v->uninits;
			
			DBG(VERIFY3,
			    dprintf("%s", indent);
			    printType(getOpstackTop(block));
			    dprintf("\n"); );
			break;
			
			
		case ANEWARRAY:
			GET_WIDX;
			OPSTACK_PEEK_T(getTINT());
			
			type = getOpstackTop(block);
			if (pool->tags[idx] == CONSTANT_ResolvedClass) {
				class = CLASS_CLASS(idx, pool);
				type->tinfo = TINFO_CLASS;
				type->data.class  = lookupArray(class, v->einfo);
				
				if (type->data.class == NULL) {
					return verifyError(v, "anewarray: error creating array type");
				}
			} else {
				char* namestr;
				
				sig = CLASS_NAMED(idx, pool);
				if (*sig == '[') {
					namestr = checkPtr(gc_malloc(sizeof(char) * (strlen(sig) + 2), KGC_ALLOC_VERIFIER));
					v->sigs = pushSig(v->sigs, namestr);
					sprintf(namestr, "[%s", sig);
				} else {
					namestr = checkPtr(gc_malloc(sizeof(char) * (strlen(sig) + 4), KGC_ALLOC_VERIFIER));
					v->sigs = pushSig(v->sigs, namestr);
					sprintf(namestr, "[L%s;", sig);
				}
				
				type->tinfo = TINFO_SIG;
				type->data.sig  = namestr;
			}
			
			DBG(VERIFY3,
			    dprintf("%s", indent);
			    printType(getOpstackTop(block));
			    dprintf("\n"); );
			break;
			
			
		case GETFIELD:
			ENSURE_OPSTACK_SIZE(1);
			if (!checkUninit(v->class, getOpstackTop(block))) {
				return verifyError(v, "getfield: uninitialized type on top of operand stack");
			}
			
			GET_WIDX;
			n = FIELDREF_CLASS(idx, pool);
			
			if (pool->tags[n] == CONSTANT_ResolvedClass) {
				t->tinfo = TINFO_CLASS;
				t->data.class = CLASS_CLASS(n, pool);
			} else {
				t->tinfo = TINFO_NAME;
				t->data.name = CLASS_NAMED(n, pool);
			}
			
			OPSTACK_POP_T_BLIND(t);
			goto GET_COMMON;
			
		case GETSTATIC:
			GET_WIDX;
			CHECK_STACK_OVERFLOW(1);
		GET_COMMON:
			sig = FIELDREF_SIGD(idx, pool);
			
			DBG(VERIFY3, dprintf("%sfield type: %s\n", indent, sig); );
			
			/* TODO: we should just have a function that returns a type based on a signature */
			switch (*sig) {
			case 'I': case 'Z': case 'S': case 'B': case 'C':
				opstackPushBlind(block, getTINT());
				break;
				
			case 'F': opstackPushBlind(block, getTFLOAT()); break;
			case 'J': OPSTACK_WPUSH(getTLONG()); break;
			case 'D': OPSTACK_WPUSH(getTDOUBLE()); break;
				
			case '[':
			case 'L':
				CHECK_STACK_OVERFLOW(1);
				block->stacksz++;
				type = getOpstackTop(block);
				type->tinfo = TINFO_SIG;
				type->data.name = sig;
				break;
				
			default:
				DBG(VERIFY3, dprintf("%sweird type signature: %s", indent, sig); );
				return verifyError(v, "get{field/static}: unrecognized type signature");
				break;
			}
			break;
			
			
		case PUTFIELD:
			if (isWide(getOpstackTop(block))) n = 3;
			else                      n = 2;
			ENSURE_OPSTACK_SIZE(n);
			
			if (!checkUninit(v->class, getOpstackTop(block))) {
				return verifyError(v, "putfield: uninitialized type on top of operand stack");
			}
			
			GET_WIDX;
			sig = FIELDREF_SIGD(idx, pool);
			DBG(VERIFY3, dprintf("                field name::type: %s::%s\n",
					     FIELDREF_NAMED(idx, pool), sig); );
			
			switch (*sig) {
			case 'I': case 'Z': case 'S': case 'B': case 'C':
				OPSTACK_POP_T_BLIND(getTINT());
				break;
				
			case 'F': OPSTACK_POP_T_BLIND(getTFLOAT());   break;
			case 'J': OPSTACK_WPOP_T_BLIND(getTLONG());   break;
			case 'D': OPSTACK_WPOP_T_BLIND(getTDOUBLE()); break;
				
			case '[':
			case 'L':
				t->tinfo = TINFO_SIG;
				t->data.sig = sig;
				OPSTACK_POP_T_BLIND(t);
				break;
				
			default:
				DBG(VERIFY3, dprintf("%sweird type signature: %s", indent, sig); );
				return verifyError(v, "put{field/static}: unrecognized type signature");
				break;
			}
			
			
			n = FIELDREF_CLASS(idx, pool);
			if (pool->tags[n] == CONSTANT_ResolvedClass) {
				t->tinfo = TINFO_CLASS;
				t->data.class = CLASS_CLASS(n, pool);
			} else {
				t->tinfo = TINFO_NAME;
				t->data.name = CLASS_NAMED(n, pool);
			}
			
			OPSTACK_POP_T_BLIND(t);
			break;
			
			
		case PUTSTATIC:
			if (getOpstackTop(block) == getTWIDE()) n = 2;
			else                      n = 1;
			ENSURE_OPSTACK_SIZE(n);
			
			GET_WIDX;
			sig = FIELDREF_SIGD(idx, pool);
			
			DBG(VERIFY3, dprintf("                field name::type: %s::%s\n",
					     FIELDREF_NAMED(idx, pool), sig); );
			
			switch (*sig) {
			case 'I': case 'Z': case 'S': case 'B': case 'C':
				OPSTACK_POP_T_BLIND(getTINT());
				break;
				
			case 'F': OPSTACK_POP_T_BLIND(getTFLOAT());   break;
			case 'J': OPSTACK_WPOP_T_BLIND(getTLONG());   break;
			case 'D': OPSTACK_WPOP_T_BLIND(getTDOUBLE()); break;
				
			case '[':
			case 'L':
				t->tinfo = TINFO_SIG;
				t->data.sig = sig;
				OPSTACK_POP_T_BLIND(t);
				break;
				
			default:
				DBG(VERIFY3, dprintf("%sweird type signature: %s", indent, sig); );
				return verifyError(v, "put{field/static}: unrecognized type signature");
				break;
			}
			break;
			
			
			/**************************************************************
			 * BRANCHING INSTRUCTIONS...END OF BASIC BLOCKS
			 **************************************************************/
		case GOTO:
		case GOTO_W:
			break;
			
		case JSR_W:
		case JSR:
			CHECK_STACK_OVERFLOW(1);
			block->stacksz++;
			type = getOpstackTop(block);
			type->tinfo = TINFO_ADDR;
			type->data.addr = getNextPC(code, pc);
			break;

		case RET:
		        /* type checking done during merging stuff... */
			break;
			
		case IF_ACMPEQ:
		case IF_ACMPNE:
			ENSURE_OPSTACK_SIZE(2);
			if (!isReference(getOpstackTop(block)) ||
			    !isReference(getOpstackWTop(block))) {
				return verifyError(v, "if_acmp* when item on top of stack is not a reference type");
			}
			opstackPopBlind(block);
			opstackPopBlind(block);
			break;
			
		case IF_ICMPEQ:
		case IF_ICMPNE:
		case IF_ICMPGT:
		case IF_ICMPGE:
		case IF_ICMPLT:
		case IF_ICMPLE:
			OPSTACK_POP_T(getTINT());
		case IFEQ:
		case IFNE:
		case IFGT:
		case IFGE:
		case IFLT:
		case IFLE:
			OPSTACK_POP_T(getTINT());
			break;
			
		case IFNONNULL:
		case IFNULL:
			ENSURE_OPSTACK_SIZE(1);
			if (!isReference(getOpstackItem(block, 1))) {
				return verifyError(v, "if[non]null: thing on top of stack is not a reference");
			}
			opstackPopBlind(block);
			break;
			
		case LOOKUPSWITCH:
		case TABLESWITCH:
			OPSTACK_POP_T(getTINT());
			return(true);
			
			
			/**************************************************************
			 * METHOD CALLING/RETURNING
			 **************************************************************/
		case INVOKEVIRTUAL:
		case INVOKESPECIAL:
		case INVOKEINTERFACE:
			
		case INVOKESTATIC:
			if (!checkMethodCall(v, block, pc)) {
				DBG(VERIFY3,
				    dprintf("\n                some problem with a method call...here's the block:\n");
				    printBlock(v->method, block, "                "); );
				
				/* propagate error */
				return verifyError(v, "invoke* error");
			}
			break;
			
			
		case IRETURN:
			OPSTACK_PEEK_T(getTINT());
			sig = getMethodReturnSig(v->method);
			if (strlen(sig) != 1 || (*sig != 'I' && *sig != 'Z' && *sig != 'S' && *sig != 'B' && *sig != 'C')) {
				return verifyError(v, "ireturn: method doesn't return an integer");
			}
			break;
		case FRETURN:
			OPSTACK_PEEK_T(getTFLOAT());
			sig = getMethodReturnSig(v->method);
			if (strcmp(sig, "F")) {
				return verifyError(v, "freturn: method doesn't return an float");
			}
			break;
		case LRETURN:
			OPSTACK_WPEEK_T(getTLONG());
			sig = getMethodReturnSig(v->method);
			if (strcmp(sig, "J")) {
				return verifyError(v, "lreturn: method doesn't return a long");
			}
			break;
		case DRETURN:
			OPSTACK_WPEEK_T(getTDOUBLE());
			sig = getMethodReturnSig(v->method);
			if (strcmp(sig, "D")) {
				return verifyError(v, "dreturn: method doesn't return a double");
			}
			break;
		case RETURN:
			sig = getMethodReturnSig(v->method);
			if (strcmp(sig, "V")) {
				return verifyError(v, "return: must return something in a non-void function");
			}
			break;
		case ARETURN:
			ENSURE_OPSTACK_SIZE(1);
			t->tinfo = TINFO_SIG;
			t->data.sig  = getMethodReturnSig(v->method);
			if (!typecheck(v, t, getOpstackTop(block))) {
				return verifyError(v, "areturn: top of stack is not type compatible with method return type");
			}
			break;
			
		case ATHROW:
			ENSURE_OPSTACK_SIZE(1);
			if (!javaLangThrowable) {
			        /* TODO: this is here for now, but perhaps we should have a TTHROWABLE that initialized as
				 *       a signature, like we do for String and Object
				 */
				loadStaticClass(&javaLangThrowable, "java/lang/Throwable");
			}
			t->tinfo = TINFO_CLASS;
			t->data.class = javaLangThrowable;
			if (!typecheck(v, t, getOpstackTop(block))) {
				DBG(VERIFY3, dprintf("%sATHROW error: ", indent); printType(getOpstackTop(block)); dprintf ("\n"); );
				return verifyError(v, "athrow: object on top of stack is not a subclass of throwable");
			}
			
			for (n = 0; n < v->method->localsz; n++) {
				if (block->locals[n].tinfo & TINFO_UNINIT) {
					return verifyError(v, "athrow: uninitialized class instance in a local variable");
				}
			}
			break;
			
			
			/**************************************************************
			 * MISC
			 **************************************************************/
		case NOP:
			break;
			
			
		case BREAKPOINT:
		        /* for internal use only: cannot appear in a class file */
			return verifyError(v, "breakpoint instruction cannot appear in classfile");
			break;
			
			
		case MONITORENTER:
		case MONITOREXIT:
			ENSURE_OPSTACK_SIZE(1);
			if(!isReference(getOpstackTop(block))) {
				return verifyError(v, "monitor*: top of stack is not an object reference");
			}
			opstackPopBlind(block);
			break;
			
			
		case DUP:
			ENSURE_OPSTACK_SIZE(1);
			if (isWide(getOpstackTop(block))) {
				return verifyError(v, "dup: on a long or double");
			}
			
			OPSTACK_PUSH(getOpstackTop(block));
			break;
			
		case DUP_X1:
			ENSURE_OPSTACK_SIZE(2);
			if (isWide(getOpstackTop(block)) || isWide(getOpstackWTop(block))) {
				return verifyError(v, "dup_x1: splits up a double or long");
			}
			
			OPSTACK_PUSH(getOpstackTop(block));
			
			*getOpstackItem(block, 2) = *getOpstackItem(block, 3);
			*getOpstackItem(block, 3) = *getOpstackItem(block, 1);
			break;
			
		case DUP_X2:
			ENSURE_OPSTACK_SIZE(3);
			if (isWide(getOpstackTop(block))) {
				return verifyError(v, "cannot dup_x2 when top item on operand stack is a two byte item");
			}
			
			OPSTACK_PUSH(getOpstackTop(block));
			
			*getOpstackItem(block, 2) = *getOpstackItem(block, 3);
			*getOpstackItem(block, 3) = *getOpstackItem(block, 4);
			*getOpstackItem(block, 4) = *getOpstackItem(block, 1);
			break;
			
		case DUP2:
			ENSURE_OPSTACK_SIZE(2);
			
			OPSTACK_PUSH(getOpstackWTop(block));
			OPSTACK_PUSH(getOpstackWTop(block));
			break;
			
		case DUP2_X1:
			ENSURE_OPSTACK_SIZE(2);
			if (isWide(getOpstackItem(block, 2))) {
				return verifyError(v, "dup_x1 requires top 2 bytes on operand stack to be single bytes items");
			}
			CHECK_STACK_OVERFLOW(2);
			
			opstackPushBlind(block, getOpstackItem(block, 2));
			opstackPushBlind(block, getOpstackItem(block, 2));
			
			*getOpstackItem(block, 3) = *getOpstackItem(block, 5);
			*getOpstackItem(block, 4) = *getOpstackItem(block, 1);
			*getOpstackItem(block, 5) = *getOpstackItem(block, 2);
			break;
			
		case DUP2_X2:
			ENSURE_OPSTACK_SIZE(4);
			if (isWide(getOpstackItem(block, 2)) || isWide(getOpstackItem(block, 4))) {
				return verifyError(v, "dup2_x2 where either 2nd or 4th byte is 2nd half of a 2 byte item");
			}
			CHECK_STACK_OVERFLOW(2);
			
			opstackPushBlind(block, getOpstackItem(block, 2));
			opstackPushBlind(block, getOpstackItem(block, 2));
			
			*getOpstackItem(block, 3) = *getOpstackItem(block, 5);
			*getOpstackItem(block, 4) = *getOpstackItem(block, 6);
			*getOpstackItem(block, 5) = *getOpstackItem(block, 1);
			*getOpstackItem(block, 6) = *getOpstackItem(block, 2);
			break;
			
			
		case SWAP:
			ENSURE_OPSTACK_SIZE(2);
			if (isWide(getOpstackTop(block)) || isWide(getOpstackWTop(block))) {
				return verifyError(v, "cannot swap 2 bytes of a long or double");
			}
			
			tt         = *getOpstackWTop(block);
			*getOpstackWTop(block)  = *getOpstackTop(block);
			*getOpstackTop(block) = tt;
			break;
			
			
		case WIDE:
			wide = true;
			pc = getNextPC(code, pc);
			continue;
			
		default:
		        /* should never get here because of preprocessing in defineBasicBlocks() */
			return verifyError(v, "unknown opcode encountered");
		}
		
		
		pc = getNextPC(code, pc);
		if (wide == true) {
			wide = false;
			pc++;
		}
	}
		
	
	/* SUCCESS! */
	return(true);


	
	/* take care of the namespace */
#undef OPSTACK_POP_N
#undef OPSTACK_POP_N_BLIND

#undef OPSTACK_WPOP_T
#undef OPSTACK_WPOP_T_BLIND
#undef OPSTACK_WPOP

#undef OPSTACK_POP_T
#undef OPSTACK_POP_T_BLIND
#undef OPSTACK_POP

#undef OPSTACK_WPEEK_T
#undef OPSTACK_WPEEK_T_BLIND
#undef OPSTACK_PEEK_T
#undef OPSTACK_PEEK_T_BLIND

#undef OPSTACK_WPUSH
#undef OPSTACK_WPUSH_BLIND
#undef OPSTACK_PUSH
#undef OPSTACK_PUSH_BLIND

#undef CHECK_STACK_OVERFLOW
#undef ENSURE_OPSTACK_SIZE

#undef ENSURE_LOCAL_WTYPE
#undef ENSURE_LOCAL_TYPE

#undef GET_WIDX
#undef GET_IDX
}
