/*
 * verify.c
 * Perform stages 2 & 3 of class verification.  Stage 1 is performed
 *  when the class is being loaded (so isn't here) and stage 4 is performed
 *  as the method is being executed.
 *
 * verify2() was originally created by someone in Transvirtual Technologies.  however,
 * it did almost nothing (only a shrivel of the stuff needed by pass 2...
 * specifically part 3 of of pass 2, which has been modified),
 * so questions regarding pass 2 should be sent to:
 *     Rob Gonzalez <rob@kaffe.org>
 *
 * verify3() was also originally created by someone in Transvirtual, but it only
 * returned true :)  Questions regarding this one can be sent to Rob as well.
 *
 * All other code in this file was added to assist the full versions of verify2() and
 * verify3().
 */

#include "config.h"
#include "config-std.h"
#include "constants.h"
#include "classMethod.h"
#include "baseClasses.h"
#include "lookup.h"
#include "exception.h"
#include "errors.h"
#include "jni.h"
#include "debug.h"
#include "utf8const.h"

/* needed for pass 3 */
#include "bytecode.h"
#include "itypes.h"
#include "soft.h"
#include "code.h"

#include "verify.h"
#include "verify-block.h"
#include "verify-debug.h"
#include "verify-sigstack.h"
#include "verify-type.h"
#include "verify-uninit.h"
#include "verify-errors.h"

/*********************************************************************************
 * Type Descriptor Parsing Methods
 *********************************************************************************/
/*
 * Returns whether the given class is "trusted" (i.e. does not require verification).
 */
bool
isTrustedClass(Hjava_lang_Class* class) {
        /* recall (from main.c): -verifyremote (default) ==> verifyMode = 2
	 *                       -verify                 ==> verifyMode = 3
	 *                       -noverify               ==> verifyMode = 0
	 */
	return ((class->loader == 0 && (Kaffe_JavaVMArgs[0].verifyMode & 1) == 0) ||
		(class->loader != 0 && (Kaffe_JavaVMArgs[0].verifyMode & 2) == 0));
}

/*
 * parses a BaseType descriptor (p.100 JVML Spec 2)
 * 
 *   BaseType: B | C | D | F | I | J | S | Z
 *
 * returns the sig pointing right after the BaseType
 * or NULL if an error occurred.
 */
const char*
parseBaseTypeDescriptor(const char* sig)
{
	switch(*sig) {
	case 'B': case 'C': case 'D': case 'F': case 'I': case 'J': case 'S': case 'Z':
		return sig + 1;
	default:
		break;
	}
	
	return NULL;
}

/*
 * parses an ObjectType descriptor (p.101 JVML Spec 2)
 *
 *   ObjectType: L<classname>;
 *   <classname> is a string made from unicode characters
 *
 * precondition: *sig == 'L'
 *
 * returns the sig pointing right after the ObjectType
 * or NULL if an error occurred.
 */
const char*
parseObjectTypeDescriptor(const char* sig)
{
	for (sig++; sig != '\0' && *sig != ';'; sig++);
	
	if (sig == '\0') return NULL;
	else             return sig + 1;
}

/* parses an ArrayType descriptor (p.101 JVML Spec 2)
 *
 *   ArrayType    : [ComponentType
 *   ComponentType: FieldType
 *
 * precondition: *sig == '['
 *
 * returns the sig pointing right after the ArrayType
 * or NULL if an error occurred.
 */
const char*
parseArrayTypeDescriptor(const char* sig)
{
	while (*sig != '\0' && *sig == '[') sig++;
	
	if (*sig == '\0') return NULL;
	else              return parseFieldTypeDescriptor(sig);
}


/*
 * parses a field type descriptor (i.e. its type signature).
 *
 *   FieldType:
 *     BaseType | ObjectType | ArrayType
 *
 * returns the signatures position immediately after the field's
 * type signature, or NULL if any error has occurred.
 */
const char*
parseFieldTypeDescriptor(const char* sig)
{
	if (sig == NULL)      return NULL;
	else if (*sig == '[') return parseArrayTypeDescriptor(sig);
	else if (*sig == 'L') return parseObjectTypeDescriptor(sig);
	else                  return parseBaseTypeDescriptor(sig);
}


/*
 * parses a method type descriptor (i.e. its signature).  (p.103 JVML Spec 2)
 *
 *   MethodDescriptor:
 *     ( ParameterDescriptor* ) ReturnDescriptor
 *   ParameterDescriptor:
 *     FieldType
 *   ReturnDescriptor:
 *     FieldType | V
 *
 * returns whether the descriptor is legal
 */
bool
parseMethodTypeDescriptor(const char* sig)
{
	if (sig == NULL || *sig != '(') return false;
	
	DBG(VERIFY2, dprintf("        parsing method type descriptor: %s\n", sig); );
	
	/* parse the type parameters */
	for (sig++; sig && *sig != ')' && *sig != '\0'; sig = parseFieldTypeDescriptor(sig)) {
		DBG(VERIFY2, dprintf("            parameter sig: %s\n", sig); );
	}
	
	if (sig == NULL || *sig == '\0') {
		DBG(VERIFY2, dprintf("            error: no ReturnDescriptor\n"); );
		return false;
	}
	sig++;
	
	DBG(VERIFY2, dprintf("            ReturnDescriptor: %s\n", sig); );	
	if (*sig == 'V') {
		sig++;
		return *sig == '\0';
	}
	
	if (parseFieldTypeDescriptor(sig) != NULL) return true;
	
	return false;
}

/************************************************************************************
 * Pass 3 Verification
 *
 * Pass 3 of verification is broken up into two distinct passes.  The first, 3a,
 * checks static constraints of the bytecode (detailed on p. 134 of the JVML Spec 2).
 * The second, 3b, performs data-flow analysis on the bytecode, which checks structural
 * constraints and does type checking.
 *
 * NOTE: according to p.171 of JVML Spec 2, "Performing verification at link time
 *       is attractive in that the checks are performed just once, substantially
 *       reducing the amount of work that must be done at run time.  Other
 *       implementation strategies are possible, provided that they comply with
 *       The Java Language Specification and The Java Virtual Machine Specification."
 *
 *       In other words, we can perform pass 3 verification whenever convenient, and
 *       not necessarily at link time.  This could provide for optimizations in the
 *       future, as verification generates a lot of information that is useful to
 *       the JIT compilers.  At the moment, this same information is generated by
 *       code-analyze.c again later on.
 ************************************************************************************/

/* lengths in bytes of all the instructions */
/* 16 rows of 16 */
static const uint8 insnLen[256] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	2, 3, 2, 3, 3, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 3, 3, 3, 
	3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 0, 0, 1, 1, 1, 1, 
	1, 1, 3, 3, 3, 3, 3, 3, 3, 5, 1, 3, 2, 3, 1, 1, 
	3, 3, 1, 1, 1, 4, 3, 3, 5, 5, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
};


/* these retrieve the word (16 bits) or double world (32 bits) of bytecode starting
 * at pc = _PC
 */

static inline
int16
getWord(const unsigned char* code,
	const unsigned int pc)
{
	return ((int16)((code[pc+0] << 8)
			| (code[pc+1])));
}

static inline
int32
getDWord(const unsigned char* code,
	 const unsigned int pc)
{
	return ((int32)((code[pc+0] << 24)
			| (code[pc+1] << 16)
			| (code[pc+2] << 8)
			| (code[pc+3])));
}

/***********************************************************************************
 * Methods for Pass 3 Verification
 ***********************************************************************************/
static bool               verifyMethod(errorInfo* einfo,
				       Method* method);

/* checks static constraints in bytecode */
static void               verifyMethod3a(errorInfo* einfo,
					 Verifier* v);

/* typechecks bytecode and performs the rest of verification */
static bool               verifyMethod3b(errorInfo* einfo,
					 Verifier* v);


static bool               loadInitialArgs(Verifier* v);
static bool               verifyBasicBlock(errorInfo*,
					   Verifier* v,
					   BlockInfo*);
static bool               mergeBasicBlocks(Verifier* v,
					   BlockInfo* fromBlock,
					   BlockInfo* toBlock);

/* for verifying method calls */
static const char*        getReturnSig(const Method*);
static uint32             countSizeOfArgsInSignature(const char* sig);
static const char*        getNextArg(const char* sig, char* buf);
static bool               checkMethodCall(errorInfo* einfo, Verifier* v,
					  BlockInfo* binfo, uint32 pc);

/*
 * Verify pass 3:  Check the consistency of the bytecode.
 *
 * This is the k-razy step that does data-flow analysis to prove the safety of the code.
 */
bool
verify3(Hjava_lang_Class* class, errorInfo *einfo)
{
	uint32 n;
	bool success = true;
	Method* method;
	
	/* see if verification is turned on, and whether the class we're about to verify requires verification
	 *
	 * NOTE: we don't skip interfaces here because an interface may contain a <clinit> method with bytecode
	 */
	if (isTrustedClass(class)) {
		return success;
	}
	
	
	/* make sure it's initialized...we had some problems because of this */
	einfo->type = 0;
	
	
	DBG(VERIFY3, dprintf("\nPass 3 Verifying Class \"%s\"\n", CLASS_CNAME(class)); );
	
	DBG(VERIFY3, {
		/* print out the superclass hirearchy */
		Hjava_lang_Class* tmp;
		for (tmp = class->superclass; tmp; tmp = tmp->superclass) {
			dprintf("                       C|-> %s\n", CLASS_CNAME(tmp));
		}
	});
	DBG(VERIFY3, {
		/* print out the complete list of implemented interfaces */
		int i;
		for (i = class->total_interface_len - 1; i >=0; i--) {
			dprintf("                       I|-> %s\n", CLASS_CNAME(class->interfaces[i]));
		}
	});
	
	
	for (n = CLASS_NMETHODS(class), method = CLASS_METHODS(class);
	     n > 0;
	     --n, ++method) {
		
		DBG(VERIFY3, dprintf("\n  -----------------------------------\n  considering method %s.%s%s\n",
				     CLASS_CNAME(class),
				     METHOD_NAMED(method), METHOD_SIGD(method)); );
		
		/* if it's abstract or native, no verification necessary */
		if (!(METHOD_IS_ABSTRACT(method) || METHOD_IS_NATIVE(method))) {
			DBG(VERIFY3, dprintf("  verifying method %s\n", METHOD_NAMED(method)); );
			
			if (!parseMethodTypeDescriptor(METHOD_SIGD(method))) {
				postExceptionMessage(einfo, JAVA_LANG(ClassFormatError),
						     "Method %s.%s has invalid signature, %s",
						     CLASS_CNAME(class), METHOD_NAMED(method), METHOD_SIGD(method));
				success = false;
				goto done;
			}
			else if (!verifyMethod(einfo, method)) {
				if (einfo->type == 0) {
					postExceptionMessage(einfo, JAVA_LANG(InternalError),
							     "failure to verify method %s.%s ... reason unspecified",
							     CLASS_CNAME(class), METHOD_NAMED(method));
				}
				success = false;
				goto done;
			}
		}
	}
	
	
 done:
	DBG(VERIFY3, dprintf("\nDone Pass 3 Verifying Class \"%s\"\n", CLASS_CNAME(class)); );
	return success;
}

	
/**************************************************************************************************
 * Memory Management Macros
 **************************************************************************************************/
/* to make sure we don't forget to unalloc anything...
 * should be called during ANY EXIT FROM verifyMethod
 */
static
void
cleanupInVerifyMethod(Verifier* v)
{
	DBG(VERIFY3, dprintf("    cleaning up..."); );
	gc_free(v->status);
	if (v->blocks != NULL) {
		while (v->numBlocks > 0) {
			freeBlock(v->blocks[--v->numBlocks]);
		}
		gc_free(v->blocks);
	}
        freeSigStack(v->sigs);
        freeUninits(v->uninits);
	freeSupertypes(v->supertypes);
        DBG(VERIFY3, dprintf(" done\n"); );
}

static inline
bool
failInVerifyMethod(errorInfo *einfo, Verifier* v)
{
        DBG(VERIFY3, dprintf("    Verify Method 3b: %s.%s%s: FAILED\n",
			     CLASS_CNAME(v->method->class), METHOD_NAMED(v->method), METHOD_SIGD(v->method)); );
	if (einfo->type == 0) {
		DBG(VERIFY3, dprintf("      DBG ERROR: should have raised an exception\n"); );
		postException(einfo, JAVA_LANG(VerifyError));
	}
        cleanupInVerifyMethod(v);
        return(false);
}

/*
 * Controls the verification of a single method.  It allocates most of the memory needed for
 * verification (when encountering JSRs, more memory will need to be allocated later),
 * loads the initial arguments, calls pass3a, then calls pass3b and cleans up.
 */
static
bool
verifyMethod(errorInfo *einfo, Method* method)
{
        /* to save some typing, etc. */
	int codelen  = METHOD_BYTECODE_LEN(method);
	
	Verifier v;
	v.einfo = einfo;
	v.class = method->class;
	v.method = method;
	v.numBlocks = 0;
	v.status = NULL;
	v.blocks = NULL;
	v.sigs = NULL;
	v.uninits = NULL;
	v.supertypes = NULL;
	v.uninits = NULL;
	
	
	/**************************************************************************************************
	 * Memory Allocation
	 **************************************************************************************************/
	DBG(VERIFY3, dprintf("        allocating memory for verification (codelen = %d)...\n", codelen); );
	
        v.status = checkPtr((uint32*)gc_malloc(codelen * sizeof(uint32), GC_ALLOC_VERIFIER));
	
	/* find basic blocks and allocate memory for them */
	verifyMethod3a(einfo, &v);
	if (!v.blocks) {
		DBG(VERIFY3, dprintf("        some kinda error finding the basic blocks in pass 3a\n"); );
		
		/* propagate error */
		return failInVerifyMethod(einfo, &v);
	}
	
	DBG(VERIFY3, dprintf("        done allocating memory\n"); );
	/**************************************************************************************************
	 * Prepare for data-flow analysis
	 **************************************************************************************************/
	
	/* load initial arguments into local variable array */
	DBG(VERIFY3, dprintf("    about to load initial args...\n"); );
	if (!loadInitialArgs(&v)) {
	        /* propagate error */
		return failInVerifyMethod(einfo, &v);
	}
	DBG(VERIFY3, {
	        /* print out the local arguments */
		int n;
		for(n = 0; n < v.method->localsz; n++) {
			dprintf("        local %d: ", n);
			printType(&(v.blocks[0]->locals[n]));
			dprintf("\n");
		}
	} );
	
	
	if (!verifyMethod3b(einfo, &v)) {
		return failInVerifyMethod(einfo, &v);
	}
	
	cleanupInVerifyMethod(&v);
	DBG(VERIFY3, dprintf("    Verify Method 3b: done\n"); );
	return(true);
}

/*
 * Helper function for byte code access.
 */
static inline
uint32
getIdx(const unsigned char * code, const uint32 pc)
{
	return ((uint32) code[pc + 1]);
}

/*
 * Helper function for byte code access.
 */
static inline
uint32
getWIdx(const unsigned char * code, const uint32 pc)
{
	return ((uint32) getWord(code, pc + 1));
}

/*
 * Helper function for pc access.
 */
static inline
uint32
getNextPC(const unsigned char * code, const uint32 pc)
{
	return (pc + insnLen[code[pc]]);
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
 */
static
void
verifyMethod3a(errorInfo* einfo, Verifier* v)
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
	
	const constants* pool     = CLASS_CONSTANTS(v->method->class);
	
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
					entry->catch_type = getClass(entry->catch_idx, v->method->class, einfo);
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
	
	blocks = checkPtr((BlockInfo**)gc_malloc(blockCount * sizeof(BlockInfo*), GC_ALLOC_VERIFIER));
	
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


/*
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
static
bool
verifyMethod3b(errorInfo* einfo, Verifier* v)
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
		
		
		if (!verifyBasicBlock(einfo, v, curBlock)) {
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
				curBlock->locals[n] = *TUNSTABLE;
				
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


/*
 * Helper function for error reporting in ENSURE_LOCAL_TYPE macro in verifyBasicBlock.
 */
static inline
bool
ensureLocalTypeErrorInVerifyBasicBlock(Verifier* v,
				       const BlockInfo* block,
				       const unsigned int n)
{
	if (block->locals[n].data.class == TUNSTABLE->data.class) {
		return verifyError(v, "attempt to access an unstable local variable");
	} else {
		return verifyError(v, "attempt to access a local variable not of the correct type");
	}
}

/*
 * Helper function for error reporting in ENSURE_OPSTACK_SIZE macro in verifyBasicBlock.
 */
static inline
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
	opstackPushBlind(block, TWIDE);
}

/*
 * Helper function for opstack access in verifyBasicBlock.
 */
static inline
void
opstackPopBlind(BlockInfo* block)
{
	block->stacksz--;
	block->opstack[block->stacksz] = *TUNSTABLE;
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
static inline
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
 *   Simulates execution of a basic block by modifying its simulated operand stack and local variable array.
 */
static
bool
verifyBasicBlock(errorInfo* einfo, Verifier* v, BlockInfo* block)
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
	else if (block->locals[_N + 1].data.class != TWIDE->data.class) { \
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
	if (getOpstackTop(block)->data.class != TWIDE->data.class) { \
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
			OPSTACK_PUSH(TNULL);
			break;
			
			/* iconst_<n> pushes n onto the stack */
		case ICONST_0: case ICONST_1: case ICONST_2:
		case ICONST_3: case ICONST_4: case ICONST_5:
			
		case ICONST_M1: /* pushes -1 onto the stack */
		case BIPUSH:    /* sign extends an 8-bit int to 32-bits and pushes it onto stack */
		case SIPUSH:    /* sign extends a 16-bit int to 32-bits and pushes it onto stack */
			OPSTACK_PUSH(TINT);
			break;
			
		case FCONST_0:
		case FCONST_1:
		case FCONST_2:
			OPSTACK_PUSH(TFLOAT);
			break;
			
		case LCONST_0:
		case LCONST_1:
			OPSTACK_WPUSH(TLONG);
			break;
			
		case DCONST_0:
		case DCONST_1:
			OPSTACK_WPUSH(TDOUBLE);
			break;
			
			
		case LDC1:
			GET_IDX;
			goto LDC_common;
		case LDC2:
			GET_WIDX;
		LDC_common:
			tag = CONST_TAG(idx, pool);
			switch(tag) {
			case CONSTANT_Integer: OPSTACK_PUSH(TINT);    break;
			case CONSTANT_Float:   OPSTACK_PUSH(TFLOAT);  break;
			case CONSTANT_ResolvedString:
			case CONSTANT_String:
			        /* we do this because we might be loading a class before
				 * loading String
				 */
				OPSTACK_PUSH(TSTRING);
				break;
			}
			break;
			
		case LDC2W:
			GET_WIDX;
			tag = CONST_TAG(idx, pool);
			if (tag == CONSTANT_Long) {
				OPSTACK_WPUSH(TLONG);
			} else {
				OPSTACK_WPUSH(TDOUBLE);
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
			ENSURE_LOCAL_TYPE(idx, TINT);
			OPSTACK_PUSH(TINT);
			break;
			
			
		case ISTORE_0: idx =0; goto ISTORE_common;
		case ISTORE_1: idx =1; goto ISTORE_common;
		case ISTORE_2: idx =2; goto ISTORE_common;
		case ISTORE_3: idx =3; goto ISTORE_common;
		case ISTORE:
			GET_CONST_INDEX;
		ISTORE_common:
			OPSTACK_POP_T(TINT);
			block->locals[idx] = *TINT;
			break;
			
			
			/* fload_<n> takes the variable at location <n> and pushes it onto the stack */
		case FLOAD_0: idx =0; goto FLOAD_common;
		case FLOAD_1: idx =1; goto FLOAD_common;
		case FLOAD_2: idx =2; goto FLOAD_common;
		case FLOAD_3: idx = 3; goto FLOAD_common;
		case FLOAD:
			GET_CONST_INDEX;
		FLOAD_common:
			ENSURE_LOCAL_TYPE(idx, TFLOAT);
			OPSTACK_PUSH(TFLOAT);
			break;
			
			
			/* stores a float from top of stack into local <n> */
		case FSTORE_0: idx = 0; goto FSTORE_common;
		case FSTORE_1: idx = 1; goto FSTORE_common;
		case FSTORE_2: idx = 2; goto FSTORE_common;
		case FSTORE_3: idx = 3; goto FSTORE_common;
		case FSTORE:
			GET_CONST_INDEX;
		FSTORE_common:
			OPSTACK_POP_T(TFLOAT);
			block->locals[idx] = *TFLOAT;
			break;
			
			
			/* lload_<n> takes the variable at location <n> and pushes it onto the stack */
		case LLOAD_0: idx = 0; goto LLOAD_common;
		case LLOAD_1: idx = 1; goto LLOAD_common;
		case LLOAD_2: idx = 2; goto LLOAD_common;
		case LLOAD_3: idx = 3; goto LLOAD_common;
		case LLOAD:
			GET_CONST_INDEX;
		LLOAD_common:
			ENSURE_LOCAL_WTYPE(idx, TLONG);
			OPSTACK_WPUSH(TLONG);
			break;
			
			
			/* lstore_<n> stores a long from top of stack into local <n> */
		case LSTORE_0: idx = 0; goto LSTORE_common;
		case LSTORE_1: idx = 1; goto LSTORE_common;
		case LSTORE_2: idx = 2; goto LSTORE_common;
		case LSTORE_3: idx = 3; goto LSTORE_common;
		case LSTORE:
			GET_CONST_INDEX;
		LSTORE_common:
			OPSTACK_WPOP_T(TLONG);
			block->locals[idx] = *TLONG;
			block->locals[idx + 1] = *TWIDE;
			break;
			
			
			/* dload_<n> takes the double at local <n> and pushes it onto the stack */
		case DLOAD_0: idx = 0; goto DLOAD_common;
		case DLOAD_1: idx = 1; goto DLOAD_common;
		case DLOAD_2: idx = 2; goto DLOAD_common;
		case DLOAD_3: idx = 3; goto DLOAD_common;
		case DLOAD:
			GET_CONST_INDEX;
		DLOAD_common:
			ENSURE_LOCAL_WTYPE(idx, TDOUBLE);
			OPSTACK_WPUSH(TDOUBLE);
			break;
			
			
			/* dstore stores a double from the top of stack into a local variable */
		case DSTORE_0: idx = 0; goto DSTORE_common;
		case DSTORE_1: idx = 1; goto DSTORE_common;
		case DSTORE_2: idx = 2; goto DSTORE_common;
		case DSTORE_3: idx = 3; goto DSTORE_common;
		case DSTORE:
			GET_CONST_INDEX;
		DSTORE_common:
			OPSTACK_WPOP_T(TDOUBLE);
			block->locals[idx] = *TDOUBLE;
			block->locals[idx + 1] = *TWIDE;
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
		        OPSTACK_POP_T(TINT);   /* array size */
			
			switch(code[pc + 1]) {
			case TYPE_Boolean: OPSTACK_PUSH(TBOOLARR);   break;
			case TYPE_Char:    OPSTACK_PUSH(TCHARARR);   break;
			case TYPE_Float:   OPSTACK_PUSH(TFLOATARR);  break;
			case TYPE_Double:  OPSTACK_PUSH(TDOUBLEARR); break;
			case TYPE_Byte:    OPSTACK_PUSH(TBYTEARR);   break;
			case TYPE_Short:   OPSTACK_PUSH(TSHORTARR);  break;
			case TYPE_Int:     OPSTACK_PUSH(TINTARR);    break;
			case TYPE_Long:    OPSTACK_PUSH(TLONGARR);   break;
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
			
			*type = *TINT;
			break;
			
			
#define ARRAY_LOAD(_T, _ARRT) \
                                OPSTACK_POP_T(TINT); \
                                OPSTACK_POP_T(_ARRT); \
				OPSTACK_PUSH(_T);

#define ARRAY_WLOAD(_T, _ARRT) \
                                OPSTACK_POP_T(TINT); \
                                OPSTACK_POP_T(_ARRT); \
				OPSTACK_WPUSH(_T);
			
			
		case AALOAD:
			ENSURE_OPSTACK_SIZE(2);
			
			if (getOpstackTop(block)->data.class != TINT->data.class) {
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
			else if (type->data.class != TNULL->data.class) {
				type->tinfo = TINFO_SIG;
				type->data.sig = CLASS_CNAME(type->data.class) + 1;
			}
			DBG(VERIFY3, dprintf("%sarray type: ", indent); printType(type); dprintf("\n"); );
			break;
			
		case IALOAD: ARRAY_LOAD(TINT,   TINTARR);   break;
		case FALOAD: ARRAY_LOAD(TFLOAT, TFLOATARR); break;
		case CALOAD: ARRAY_LOAD(TINT,   TCHARARR);  break;
		case SALOAD: ARRAY_LOAD(TINT,   TSHORTARR); break;
			
		case LALOAD: ARRAY_WLOAD(TLONG,   TLONGARR);   break;
		case DALOAD: ARRAY_WLOAD(TDOUBLE, TDOUBLEARR); break;
#undef ARRAY_LOAD
#undef ARRAY_WLOAD

		case BALOAD:
			/* BALOAD can be used for bytes or booleans .... */
			OPSTACK_POP_T(TINT);

			if (!typecheck (v, TBYTEARR, getOpstackTop(block)) &&
			    !typecheck (v, TBOOLARR, getOpstackTop(block))) {
                                DBG(VERIFY3,
                                    dprintf("                OPSTACK_TOP: ");
                                    printType(getOpstackTop(block));
                                    dprintf(" vs. what's we wanted: TBYTEARR or TBOOLARR"); )
                                return verifyError(v, "top of opstack does not have desired type");
			}

			opstackPopBlind(block);
			OPSTACK_PUSH(TINT);
			break;


		case AASTORE:
		        /* the runtime value of the type on the top of the stack must be
			 * assignment compatible with the type of the array
			 */
			ENSURE_OPSTACK_SIZE(3);
			
			if (getOpstackItem(block, 2)->data.class != TINT->data.class) {
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
				if (arrayType->data.class == TOBJARR->data.class) {
					*arrayType = *TOBJ;
				} else if (arrayType->data.class != TNULL->data.class) {
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
				OPSTACK_POP_T(TINT); \
				OPSTACK_POP_T(_ARRT);
			
#define ARRAY_WSTORE(_T, _ARRT) \
				OPSTACK_WPOP_T(_T); \
				OPSTACK_POP_T(TINT); \
				OPSTACK_POP_T(_ARRT);
			
			
			
			
		case IASTORE: ARRAY_STORE(TINT,   TINTARR);   break;
		case FASTORE: ARRAY_STORE(TFLOAT, TFLOATARR); break;
		case CASTORE: ARRAY_STORE(TINT,   TCHARARR);  break;
		case SASTORE: ARRAY_STORE(TINT,   TSHORTARR); break;
			
		case LASTORE: ARRAY_WSTORE(TLONG,   TLONGARR);   break;
		case DASTORE: ARRAY_WSTORE(TDOUBLE, TDOUBLEARR); break;
#undef ARRAY_STORE
#undef ARRAY_WSTORE

		case BASTORE: 
			/* BASTORE can store either bytes or booleans .... */
			OPSTACK_POP_T(TINT);
			OPSTACK_POP_T(TINT);

			if ( !typecheck(v, TBYTEARR, getOpstackTop(block)) &&
			     !typecheck(v, TBOOLARR, getOpstackTop(block))) {
				DBG(VERIFY3,
				    dprintf("                OPSTACK_TOP: ");
				    printType(getOpstackTop(block));
				    dprintf(" vs. what's we wanted: TBYTEARR or TBOOLARR"); )
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
			OPSTACK_POP_T(TINT);
			break;
		case INEG:
			OPSTACK_PEEK_T(TINT);
			break;
			
			
		case LAND: case LOR:  case LXOR:
		case LADD: case LSUB: case LMUL: case LDIV: case LREM:
			OPSTACK_WPOP_T(TLONG);
			break;
		case LNEG:
			OPSTACK_WPEEK_T(TLONG);
			break;
			
		case LSHL: case LSHR: case LUSHR:
			OPSTACK_POP_T(TINT);
			OPSTACK_WPEEK_T(TLONG);
			break;
			
			
		case FADD: case FSUB: case FMUL: case FDIV: case FREM:
			OPSTACK_POP_T(TFLOAT);
			break;
		case FNEG:
			OPSTACK_PEEK_T(TFLOAT);
			break;
			
			
		case DADD: case DSUB: case DDIV: case DMUL: case DREM:
			OPSTACK_WPOP_T(TDOUBLE);
			break;
		case DNEG:
			OPSTACK_WPEEK_T(TDOUBLE);
			break;
			
			
		case LCMP:
			OPSTACK_WPOP_T(TLONG);
			OPSTACK_WPOP_T(TLONG);
			opstackPushBlind(block, TINT);
			break;
			
		case FCMPG:
		case FCMPL:
			OPSTACK_POP_T(TFLOAT);
			OPSTACK_POP_T(TFLOAT);
			opstackPushBlind(block, TINT);
			break;
				
		case DCMPG:
		case DCMPL:
			OPSTACK_WPOP_T(TDOUBLE);
			OPSTACK_WPOP_T(TDOUBLE);
			opstackPushBlind(block, TINT);
			break;
			
			
		case IINC:
			if (wide == true) { GET_WIDX; }
			else              { GET_IDX; }
			
			ENSURE_LOCAL_TYPE(idx, TINT);
			
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
			OPSTACK_PEEK_T(TINT);
			break;
			
		case I2F:
			OPSTACK_POP_T(TINT);
			opstackPushBlind(block, TFLOAT);
			break;
		case I2L:
			OPSTACK_POP_T(TINT);
			CHECK_STACK_OVERFLOW(2);
			opstackWPushBlind(block, TLONG);
			break;
		case I2D:
			OPSTACK_POP_T(TINT);
			CHECK_STACK_OVERFLOW(2);
			opstackWPushBlind(block, TDOUBLE);
			break;
			
		case F2I:
			OPSTACK_POP_T(TFLOAT);
			opstackPushBlind(block, TINT);
			break;
		case F2L:
			OPSTACK_POP_T(TFLOAT);
			OPSTACK_WPUSH(TLONG);
			break;
		case F2D:
			OPSTACK_POP_T(TFLOAT);
			OPSTACK_WPUSH(TDOUBLE);
			break;
			
		case L2I:
			OPSTACK_WPOP_T(TLONG);
			opstackPushBlind(block, TINT);
			break;
		case L2F:
			OPSTACK_WPOP_T(TLONG);
			opstackPushBlind(block, TFLOAT);
			break;
		case L2D:
			OPSTACK_WPOP_T(TLONG);
			opstackWPushBlind(block, TDOUBLE);
			break;
			
		case D2I:
			OPSTACK_WPOP_T(TDOUBLE);
			opstackPushBlind(block, TINT);
			break;
		case D2F:
			OPSTACK_WPOP_T(TDOUBLE);
			opstackPushBlind(block, TFLOAT);
			break;
		case D2L:
			OPSTACK_WPOP_T(TDOUBLE);
			opstackWPushBlind(block, TLONG);
			break;
			
			
			
			/**************************************************************
			 * OBJECT CREATION/TYPE CHECKING
			 **************************************************************/
		case INSTANCEOF:
			ENSURE_OPSTACK_SIZE(1);
			if (!isReference(getOpstackItem(block, 1))) {
				return verifyError(v, "instanceof: top of stack is not a reference type");
			}
			*getOpstackTop(block) = *TINT;
			break;
			
		case CHECKCAST:
			ENSURE_OPSTACK_SIZE(1);
			opstackPopBlind(block);
			goto NEW_COMMON;
			
		case MULTIANEWARRAY:
			n = code[pc + 3];
			ENSURE_OPSTACK_SIZE(n);
			while (n > 0) {
				if (getOpstackTop(block)->data.class != TINT->data.class) {
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
			OPSTACK_PEEK_T(TINT);
			
			type = getOpstackTop(block);
			if (pool->tags[idx] == CONSTANT_ResolvedClass) {
				class = CLASS_CLASS(idx, pool);
				type->tinfo = TINFO_CLASS;
				type->data.class  = lookupArray(class, einfo);
				
				if (type->data.class == NULL) {
					return verifyError(v, "anewarray: error creating array type");
				}
			} else {
				char* namestr;
				
				sig = CLASS_NAMED(idx, pool);
				if (*sig == '[') {
					namestr = checkPtr(gc_malloc(sizeof(char) * (strlen(sig) + 2), GC_ALLOC_VERIFIER));
					v->sigs = pushSig(v->sigs, namestr);
					sprintf(namestr, "[%s", sig);
				} else {
					namestr = checkPtr(gc_malloc(sizeof(char) * (strlen(sig) + 4), GC_ALLOC_VERIFIER));
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
				opstackPushBlind(block, TINT);
				break;
				
			case 'F': opstackPushBlind(block, TFLOAT); break;
			case 'J': OPSTACK_WPUSH(TLONG); break;
			case 'D': OPSTACK_WPUSH(TDOUBLE); break;
				
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
				OPSTACK_POP_T_BLIND(TINT);
				break;
				
			case 'F': OPSTACK_POP_T_BLIND(TFLOAT);   break;
			case 'J': OPSTACK_WPOP_T_BLIND(TLONG);   break;
			case 'D': OPSTACK_WPOP_T_BLIND(TDOUBLE); break;
				
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
			if (getOpstackTop(block) == TWIDE) n = 2;
			else                      n = 1;
			ENSURE_OPSTACK_SIZE(n);
			
			GET_WIDX;
			sig = FIELDREF_SIGD(idx, pool);
			
			DBG(VERIFY3, dprintf("                field name::type: %s::%s\n",
					     FIELDREF_NAMED(idx, pool), sig); );
			
			switch (*sig) {
			case 'I': case 'Z': case 'S': case 'B': case 'C':
				OPSTACK_POP_T_BLIND(TINT);
				break;
				
			case 'F': OPSTACK_POP_T_BLIND(TFLOAT);   break;
			case 'J': OPSTACK_WPOP_T_BLIND(TLONG);   break;
			case 'D': OPSTACK_WPOP_T_BLIND(TDOUBLE); break;
				
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
			OPSTACK_POP_T(TINT);
		case IFEQ:
		case IFNE:
		case IFGT:
		case IFGE:
		case IFLT:
		case IFLE:
			OPSTACK_POP_T(TINT);
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
			OPSTACK_POP_T(TINT);
			return(true);
			
			
			/**************************************************************
			 * METHOD CALLING/RETURNING
			 **************************************************************/
		case INVOKEVIRTUAL:
		case INVOKESPECIAL:
		case INVOKEINTERFACE:
			
		case INVOKESTATIC:
			if (!checkMethodCall(einfo, v, block, pc)) {
				DBG(VERIFY3,
				    dprintf("\n                some problem with a method call...here's the block:\n");
				    printBlock(v->method, block, "                "); );
				
				/* propagate error */
				return verifyError(v, "invoke* error");
			}
			break;
			
			
		case IRETURN:
			OPSTACK_PEEK_T(TINT);
			sig = getReturnSig(v->method);
			if (strlen(sig) != 1 || (*sig != 'I' && *sig != 'Z' && *sig != 'S' && *sig != 'B' && *sig != 'C')) {
				return verifyError(v, "ireturn: method doesn't return an integer");
			}
			break;
		case FRETURN:
			OPSTACK_PEEK_T(TFLOAT);
			sig = getReturnSig(v->method);
			if (strcmp(sig, "F")) {
				return verifyError(v, "freturn: method doesn't return an float");
			}
			break;
		case LRETURN:
			OPSTACK_WPEEK_T(TLONG);
			sig = getReturnSig(v->method);
			if (strcmp(sig, "J")) {
				return verifyError(v, "lreturn: method doesn't return a long");
			}
			break;
		case DRETURN:
			OPSTACK_WPEEK_T(TDOUBLE);
			sig = getReturnSig(v->method);
			if (strcmp(sig, "D")) {
				return verifyError(v, "dreturn: method doesn't return a double");
			}
			break;
		case RETURN:
			sig = getReturnSig(v->method);
			if (strcmp(sig, "V")) {
				return verifyError(v, "return: must return something in a non-void function");
			}
			break;
		case ARETURN:
			ENSURE_OPSTACK_SIZE(1);
			t->tinfo = TINFO_SIG;
			t->data.sig  = getReturnSig(v->method);
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


/* 
 * parses the next argument from sig into buf, returning pointer beyond arg.
 */
static
const char*
getNextArg(const char* sig, char* buf)
{
	const char* afterSig;
	
	if (*sig == ')') {
		buf[0] = ')';
		buf[1] = '\0';
		return sig;
	}
	/* parseFieldTypeDescriptor doesn't deal with void signatures */
	else if (*sig == 'V') {
		buf[0] = 'V';
		buf[1] = '\0';
		sig++;
		return sig;
	}
	
	for (afterSig = parseFieldTypeDescriptor(sig);
	     sig < afterSig;
	     sig++, buf++) {
		*buf = *sig;
	}
	
	*buf = '\0';
	
	return afterSig;
}


/*
 * countSizeOfArgsInSignature()
 *    Longs & Double count for 2, all else counts for one.
 */
static
uint32
countSizeOfArgsInSignature(const char* sig)
{
	uint32 count = 0;
	
	for (sig++; *sig != ')'; sig = parseFieldTypeDescriptor(sig)) {
		if (*sig == 'J' || *sig == 'D')
			count += 2;
		else
			count++;
	}
	
	return count;
}

/*
 * Helper function for error reporting in checkMethodCall.
 * Provided basically to give better debugging output.
 */
static inline
bool
verifyErrorInCheckMethodCall(Verifier* v,
			     char* argbuf,
			     uint32 pc,
			     const uint32 idx,
			     const constants* pool,
			     const char* methSig,
			     const char* msg)
{
	gc_free(argbuf);
	DBG(VERIFY3,
	    dprintf("                error with method invocation, pc = %d, method = %s%s\n",
		    pc,
		    METHODREF_NAMED(idx, pool),
		    methSig);
	    );
	return verifyError(v, msg);
}

/*
 * Helper function for error reporting in checkMethodCall.
 */
static inline
bool
typeErrorInCheckMethodCall(Verifier* v,
			   char* argbuf,
			   uint32 pc,
			   const uint32 idx,
			   const constants* pool,
			   const char* methSig)
{
	return verifyErrorInCheckMethodCall(v, argbuf, pc, idx, pool, methSig,
					    "parameters fail type checking in method invocation");
}

/* 
 * checkMethodCall()
 *    verify an invoke instruction.  this includes making sure that the types
 *    on the operand stack are type compatible with those expected by the method
 *    being called.
 *
 *    note: we don't check to make sure that the class being referenced by the
 *          method call actually has the method, or that we have permission to
 *          access it, as those checks are deferred until pass 4.
 *
 * returns whether the method's arguments type check correctly.
 * it also pushes the return type onto binfo's operand stack.
 */
static
bool
checkMethodCall(errorInfo* einfo, Verifier* v,
		BlockInfo* binfo, uint32 pc)
{
	const unsigned char* code        = METHOD_BYTECODE_CODE(v->method);
	const uint32 opcode              = code[pc];
	
	const constants* pool            = CLASS_CONSTANTS(v->class);
	const uint32 idx                 = getWord(code, pc + 1);
	
	const uint32 classIdx            = METHODREF_CLASS(idx, pool);
	Type  mrc;
	Type* methodRefClass             = &mrc;
	Type* t                          = &mrc; /* for shorthand :> */
	Type* receiver                   = NULL;
	
	const char* methSig              = METHODREF_SIGD(idx, pool);
	const char* sig                  = methSig;
	uint32 nargs                     = countSizeOfArgsInSignature(sig);
	
	uint32 paramIndex                = 0;
	char* argbuf                     = checkPtr(gc_malloc(strlen(sig) * sizeof(char), GC_ALLOC_VERIFIER));
	
	
	DBG(VERIFY3, dprintf("%scalling method %s%s\n", indent, METHODREF_NAMED(idx, pool), sig); );
	
	
	if (nargs > binfo->stacksz) {
		return verifyErrorInCheckMethodCall(v, argbuf, pc, idx, pool, methSig, "not enough stuff on opstack for method invocation");
	}
	
	
	/* make sure that the receiver is type compatible with the class being invoked */
	if (opcode != INVOKESTATIC) {
		if (nargs == binfo->stacksz) {
			return verifyErrorInCheckMethodCall(v, argbuf, pc, idx, pool, methSig, "not enough stuff on opstack for method invocation");
		}
		
		
		receiver = &binfo->opstack[binfo->stacksz - (nargs + 1)];
		if (!(receiver->tinfo & TINFO_UNINIT) && !isReference(receiver)) {
			return verifyErrorInCheckMethodCall(v, argbuf, pc, idx, pool, methSig, "invoking a method on something that is not a reference");
		}
		
		if (pool->tags[classIdx] == CONSTANT_Class) {
			methodRefClass->tinfo = TINFO_NAME;
			methodRefClass->data.name = UNRESOLVED_CLASS_NAMED(classIdx, pool);
		} else {
			methodRefClass->tinfo = TINFO_CLASS;
			methodRefClass->data.class = CLASS_CLASS(classIdx, pool);
		}
		
		
		if (!strcmp(METHODREF_NAMED(idx,pool), constructor_name->data)) {
			if (receiver->tinfo & TINFO_UNINIT) {
				UninitializedType* uninit = receiver->data.uninit;
				
				if (receiver->tinfo == TINFO_UNINIT_SUPER) {
					Type t_uninit_super;
					t_uninit_super.tinfo = TINFO_CLASS;
					t_uninit_super.data.class = uninit->type.data.class->superclass;
					
					if (!sameType(methodRefClass, &uninit->type) &&
					    uninit->type.data.class != TOBJ->data.class &&
					    !sameType(methodRefClass, &t_uninit_super)) {
						return verifyErrorInCheckMethodCall(v, argbuf, pc, idx, pool, methSig, "incompatible receiving type for superclass constructor call");
					}
				} else if (!sameType(methodRefClass, &uninit->type)) {
					DBG(VERIFY3,
					    dprintf("%smethodRefClass: ", indent); printType(methodRefClass);
					    dprintf("\n%sreceiver: ", indent); printType(&uninit->type); dprintf("\n"); );
					return verifyErrorInCheckMethodCall(v, argbuf, pc, idx, pool, methSig, "incompatible receiving type for constructor call");
				}
				
				/* fix front of list, if necessary */
				if (uninit == v->uninits) {
					v->uninits = v->uninits->next;
					if (v->uninits) {
						(v->uninits)->prev = NULL;
					}
					uninit->next = NULL;
				}
				
				popUninit(v->method, uninit, binfo);
			}
			else if (!sameType(methodRefClass, receiver)) {
				return verifyErrorInCheckMethodCall(v, argbuf, pc, idx, pool, methSig, "incompatible receiving type for constructor call");
			}
		}
		else if (!typecheck(v, methodRefClass, receiver)) {
			if (receiver->tinfo & TINFO_UNINIT) {
				return verifyErrorInCheckMethodCall(v, argbuf, pc, idx, pool, methSig, "invoking a method on an uninitialized object reference");
			}
			
			DBG(VERIFY3,
			    dprintf("%srequired receiver type: ", indent);
			    printType(methodRefClass);
			    dprintf("\n%sactual   receiver type: ", indent);
			    printType(receiver);
			    dprintf("\n");
			    );
			return verifyErrorInCheckMethodCall(v, argbuf, pc, idx, pool, methSig, "expected method receiver does not typecheck with object on operand stack");
		}
	}
	
	
	/* here we use paramIndex to represent which parameter we're currently considering.
	 * remember, when we call a method, the first parameter is deepest in the stack,
	 * so when we traverse the parameter list in the method signature we have to look
	 * from the bottom up.
	 */
	paramIndex = binfo->stacksz - nargs;
	for (sig = getNextArg(sig + 1, argbuf); *argbuf != ')'; sig = getNextArg(sig, argbuf)) {
		
		if (paramIndex >= binfo->stacksz) {
			gc_free(argbuf);
			return verifyErrorInCheckMethodCall(v, argbuf, pc, idx, pool, methSig, "error: not enough parameters on stack for method invocation");
		}
		
		
		switch (*argbuf) {
		case '[':
		case 'L':
			t->tinfo = TINFO_SIG;
			t->data.sig = argbuf;
			
			if (!typecheck(v, t, &binfo->opstack[paramIndex])) {
				return typeErrorInCheckMethodCall(v, argbuf, pc, idx, pool, methSig);
			}
			
			binfo->opstack[paramIndex] = *TUNSTABLE;
			paramIndex++;
			break;
			
		case 'Z': case 'S': case 'B': case 'C':
		case 'I':
			if (binfo->opstack[paramIndex].data.class != TINT->data.class) {
				return typeErrorInCheckMethodCall(v, argbuf, pc, idx, pool, methSig);
			}
			
			binfo->opstack[paramIndex] = *TUNSTABLE;
			paramIndex++;
			break;
			
		case 'F':
			if (binfo->opstack[paramIndex].data.class != TFLOAT->data.class) {
				return typeErrorInCheckMethodCall(v, argbuf, pc, idx, pool, methSig);
			}
			
			binfo->opstack[paramIndex] = *TUNSTABLE;
			paramIndex++;
			break;
			
		case 'J':
			if (binfo->opstack[paramIndex].data.class != TLONG->data.class ||
			    !isWide(&binfo->opstack[paramIndex + 1])) {
				return typeErrorInCheckMethodCall(v, argbuf, pc, idx, pool, methSig);
			}
			
			binfo->opstack[paramIndex]    = *TUNSTABLE;
			binfo->opstack[paramIndex+ 1] = *TUNSTABLE;
			paramIndex += 2;
			break;
			
		case 'D':
			if (binfo->opstack[paramIndex].data.class != TDOUBLE->data.class ||
			    !isWide(&binfo->opstack[paramIndex + 1])) {
				return typeErrorInCheckMethodCall(v, argbuf, pc, idx, pool, methSig);
			}
			
			binfo->opstack[paramIndex]     = *TUNSTABLE;
			binfo->opstack[paramIndex + 1] = *TUNSTABLE;
			paramIndex += 2;
			break;
			
		default:
			return typeErrorInCheckMethodCall(v, argbuf, pc, idx, pool, methSig);
		}
	}
	binfo->stacksz -= nargs;
	
	
	if (opcode != INVOKESTATIC) {
	        /* pop object reference off the stack */
		binfo->stacksz--;
		binfo->opstack[binfo->stacksz] = *TUNSTABLE;
	}
	
	
	/**************************************************************
	 * Process Return Type
	 **************************************************************/
	sig++;
	sig = getNextArg(sig, argbuf);
	
	if (*argbuf == 'J' || *argbuf == 'D') {
		if (v->method->stacksz < binfo->stacksz + 2) {
			return verifyErrorInCheckMethodCall(v, argbuf, pc, idx, pool, methSig, "not enough room on operand stack for method call's return value");
		}
	}
	else if (*argbuf != 'V') {
		if (v->method->stacksz < binfo->stacksz + 1) {
			return verifyErrorInCheckMethodCall(v, argbuf, pc, idx, pool, methSig, "not enough room on operand stack for method call's return value");
		}
	}
	
	switch (*argbuf) {
	case 'Z': case 'S': case 'B': case 'C':
	case 'I':
		binfo->opstack[binfo->stacksz++] = *TINT;
		break;
		
	case 'F':
		binfo->opstack[binfo->stacksz++] = *TFLOAT;
		break;
		
	case 'J':
		binfo->opstack[binfo->stacksz]     = *TLONG;
		binfo->opstack[binfo->stacksz + 1] = *TWIDE;
		binfo->stacksz += 2;
		break;
		
	case 'D':
		binfo->opstack[binfo->stacksz]     = *TDOUBLE;
		binfo->opstack[binfo->stacksz + 1] = *TWIDE;
		binfo->stacksz += 2;
		break;
		
	case 'V':
		break;
		
	case '[':
	case 'L':
		v->sigs = pushSig(v->sigs, argbuf);
		
		binfo->opstack[binfo->stacksz].data.class = (Hjava_lang_Class*)argbuf;
		binfo->opstack[binfo->stacksz].tinfo = TINFO_SIG;
		binfo->stacksz++;
		
		/* no freeing of the argbuf here... */
		return(true);
		
	default:
	        /* shouldn't get here because of parsing during pass 2... */
		DBG(VERIFY3, dprintf("                unrecognized return type signature: %s\n", argbuf); );
		gc_free(argbuf);
		postExceptionMessage(einfo, JAVA_LANG(InternalError),
				     "unrecognized return type signature");
		return(false);
	}
	
	gc_free(argbuf);
	return(true);
}


static inline
bool
verifyErrorInLoadInitialArgs(Verifier* v, const char* msg, char* argbuf) {
	gc_free(argbuf);
	return verifyError(v, msg);
}

static inline
bool
localOverflowErrorInLoadInitialArgs(Verifier* v, char* argbuf) {
	return verifyErrorInLoadInitialArgs(v, "method arguments cannot fit into local variables", argbuf);
}

/*
 * pushes the initial method arguments into local variable array
 */
static
bool
loadInitialArgs(Verifier* v)
{
	uint32 paramCount = 0;
	
	/* the +1 skips the initial '(' */
	const char* sig = METHOD_SIGD(v->method) + 1;
	char* argbuf    = checkPtr(gc_malloc((strlen(sig)+1) * sizeof(char), GC_ALLOC_VERIFIER));
	char* newsig    = NULL;
	
	/* load the initial argument into the first basic block.
	 */
	BlockInfo* block = v->blocks[0];
	Type* locals = block->locals;
	
	
	DBG(VERIFY3, dprintf("        sig: %s\n", sig); );
	
	/* must have at least 1 local variable for the object reference	*/
	if (!METHOD_IS_STATIC(v->method)) {
		if (v->method->localsz <= 0) {
			return verifyErrorInLoadInitialArgs(v, "number of locals in non-static method must be > 0", argbuf);
		}
		
		/* the first local variable in every method is the class to which it belongs */
		locals[0].tinfo = TINFO_CLASS;
		locals[0].data.class = v->method->class;
		paramCount++;
		if (!strcmp(METHOD_NAMED(v->method), constructor_name->data)) {
		        /* the local reference in a constructor is uninitialized */
			v->uninits = pushUninit(v->uninits, &locals[0]);
			locals[0].tinfo = TINFO_UNINIT_SUPER;
			locals[0].data.uninit = v->uninits;
		}
	}
	
	for (sig = getNextArg(sig, argbuf); *argbuf != ')'; sig = getNextArg(sig, argbuf)) {
		if (paramCount > v->method->localsz) {
			return localOverflowErrorInLoadInitialArgs(v, argbuf);
		}
		
		switch (*argbuf) {
		case 'Z': case 'S': case 'B': case 'C':
		case 'I': locals[paramCount++] = *TINT; break;
		case 'F': locals[paramCount++] = *TFLOAT; break;
			
		case 'J':
			if (paramCount + 1 > v->method->localsz) {
				return localOverflowErrorInLoadInitialArgs(v, argbuf);
			}
			locals[paramCount] = *TLONG;
			locals[paramCount+1] = *TWIDE;
			paramCount += 2;
			break;
			
		case 'D':
			if (paramCount + 1 > v->method->localsz) {
				return localOverflowErrorInLoadInitialArgs(v, argbuf);
			}
			locals[paramCount] = *TDOUBLE;
			locals[paramCount+1] = *TWIDE;
			paramCount += 2;
			break;
			
		case '[':
		case 'L':
			newsig = checkPtr(gc_malloc((strlen(argbuf) + 1) * sizeof(char), GC_ALLOC_VERIFIER));
			v->sigs = pushSig(v->sigs, newsig);
			sprintf(newsig, "%s", argbuf);
			locals[paramCount].tinfo = TINFO_SIG;
			locals[paramCount].data.sig = newsig;
			paramCount++;
			break;
			
		default:
			DBG(VERIFY3,
			    dprintf("ERROR, loadInitialArgs(): argument to method has bad signature.\n");
			    dprintf("        it starts with an unrecognized character: %c\n", *argbuf);
			    dprintf("        the rest of argbuf: %s\n", argbuf);
			    );
			
			return verifyErrorInLoadInitialArgs(v, "unrecognized first character in parameter type descriptor", argbuf);
		}
	}
	
	
	/* success! */
	gc_free(argbuf);
	return(true);

#undef LOCAL_OVERFLOW_ERROR
#undef VERIFY_ERROR
}


/*
 * getReturnSig()
 */
static
const char*
getReturnSig(const Method* method)
{
	const char* sig = METHOD_SIGD(method);
	
	/* skip the type parameters */
	for (sig++; *sig != ')'; sig = parseFieldTypeDescriptor(sig));
	sig++;
	
	return sig;
}
