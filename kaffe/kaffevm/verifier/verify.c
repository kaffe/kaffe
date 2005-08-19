/*
 * verify.c
 *
 * Copyright 2004, 2005
 *   Kaffe.org contributors. See ChangeLog for details. All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 *
 * Perform stages 3 of class verification.
 * Stage 1 is performed when the class is being loaded (so isn't here).
 * Stage 2 is performed in verify2.
 * stage 4 is performed as the method is being executed.
 *
 * verify3() was also originally created by someone in Transvirtual, but it only
 * returned true :)  Questions regarding this one can be sent to Rob as well.
 *
 * All other code in this file was added to assist the full versions of verify2() and
 * verify3().
 */

#include "config.h"
#include "config-std.h"

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include "baseClasses.h"
#include "bytecode.h"
#include "code.h"
#include "constants.h"
#include "classMethod.h"
#include "debug.h"
#include "exception.h"
#include "errors.h"
#include "itypes.h"
#include "lookup.h"
#include "utf8const.h"

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
	return ((class->loader == 0 && (Kaffe_JavaVMArgs.verifyMode & 1) == 0) ||
		(class->loader != 0 && (Kaffe_JavaVMArgs.verifyMode & 2) == 0));
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
const uint8 insnLen[256] = {
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


/***********************************************************************************
 * Methods for Pass 3 Verification
 ***********************************************************************************/
static bool               verifyMethod(errorInfo* einfo,
				       Method* method);
static bool               loadInitialArgs(Verifier* v);


/* for verifying method calls */
static uint32             countSizeOfArgsInSignature(const char* sig);
static const char*        getNextArg(const char* sig,
				     char* buf);

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
		return true;
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
 *
 * NOTE: we don't free the Verifier object itself, just
 * its data.
 */
void
freeVerifierData(Verifier* v)
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
failInVerifyMethod(Verifier* v)
{
        DBG(VERIFY3, dprintf("    Verify Method 3b: %s.%s%s: FAILED\n",
			     CLASS_CNAME(v->method->class), METHOD_NAMED(v->method), METHOD_SIGD(v->method)); );
	if (v->einfo->type == 0) {
		DBG(VERIFY3, dprintf("      DBG ERROR: should have raised an exception\n"); );
		postException(v->einfo, JAVA_LANG(VerifyError));
	}
        freeVerifierData(v);
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
	
        v.status = checkPtr((uint32*)gc_malloc(codelen * sizeof(uint32), KGC_ALLOC_VERIFIER));
	
	/* find basic blocks and allocate memory for them */
	verifyMethod3a(&v);
	if (!v.blocks) {
		DBG(VERIFY3, dprintf("        some kinda error finding the basic blocks in pass 3a\n"); );
		
		/* propagate error */
		return failInVerifyMethod(&v);
	}
	
	DBG(VERIFY3, dprintf("        done allocating memory\n"); );
	/**************************************************************************************************
	 * Prepare for data-flow analysis
	 **************************************************************************************************/
	
	/* load initial arguments into local variable array */
	DBG(VERIFY3, dprintf("    about to load initial args...\n"); );
	if (!loadInitialArgs(&v)) {
	        /* propagate error */
		return failInVerifyMethod(&v);
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
	
	
	if (!verifyMethod3b(&v)) {
		return failInVerifyMethod(&v);
	}
	
	freeVerifierData(&v);
	DBG(VERIFY3, dprintf("    Verify Method 3b: done\n"); );
	return(true);
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
bool
checkMethodCall(Verifier* v,
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
	char* argbuf                     = checkPtr(gc_malloc(strlen(sig) * sizeof(char), KGC_ALLOC_VERIFIER));
	
	
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
					    uninit->type.data.class != getTOBJ()->data.class &&
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
			
			binfo->opstack[paramIndex] = *getTUNSTABLE();
			paramIndex++;
			break;
			
		case 'Z': case 'S': case 'B': case 'C':
		case 'I':
			if (binfo->opstack[paramIndex].data.class != getTINT()->data.class) {
				return typeErrorInCheckMethodCall(v, argbuf, pc, idx, pool, methSig);
			}
			
			binfo->opstack[paramIndex] = *getTUNSTABLE();
			paramIndex++;
			break;
			
		case 'F':
			if (binfo->opstack[paramIndex].data.class != getTFLOAT()->data.class) {
				return typeErrorInCheckMethodCall(v, argbuf, pc, idx, pool, methSig);
			}
			
			binfo->opstack[paramIndex] = *getTUNSTABLE();
			paramIndex++;
			break;
			
		case 'J':
			if (binfo->opstack[paramIndex].data.class != getTLONG()->data.class ||
			    !isWide(&binfo->opstack[paramIndex + 1])) {
				return typeErrorInCheckMethodCall(v, argbuf, pc, idx, pool, methSig);
			}
			
			binfo->opstack[paramIndex]    = *getTUNSTABLE();
			binfo->opstack[paramIndex+ 1] = *getTUNSTABLE();
			paramIndex += 2;
			break;
			
		case 'D':
			if (binfo->opstack[paramIndex].data.class != getTDOUBLE()->data.class ||
			    !isWide(&binfo->opstack[paramIndex + 1])) {
				return typeErrorInCheckMethodCall(v, argbuf, pc, idx, pool, methSig);
			}
			
			binfo->opstack[paramIndex]     = *getTUNSTABLE();
			binfo->opstack[paramIndex + 1] = *getTUNSTABLE();
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
		binfo->opstack[binfo->stacksz] = *getTUNSTABLE();
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
		binfo->opstack[binfo->stacksz++] = *getTINT();
		break;
		
	case 'F':
		binfo->opstack[binfo->stacksz++] = *getTFLOAT();
		break;
		
	case 'J':
		binfo->opstack[binfo->stacksz]     = *getTLONG();
		binfo->opstack[binfo->stacksz + 1] = *getTWIDE();
		binfo->stacksz += 2;
		break;
		
	case 'D':
		binfo->opstack[binfo->stacksz]     = *getTDOUBLE();
		binfo->opstack[binfo->stacksz + 1] = *getTWIDE();
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
		postExceptionMessage(v->einfo, JAVA_LANG(InternalError),
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
	char* argbuf    = checkPtr(gc_malloc((strlen(sig)+1) * sizeof(char), KGC_ALLOC_VERIFIER));
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
		case 'I': locals[paramCount++] = *getTINT(); break;
		case 'F': locals[paramCount++] = *getTFLOAT(); break;
			
		case 'J':
			if (paramCount + 1 > v->method->localsz) {
				return localOverflowErrorInLoadInitialArgs(v, argbuf);
			}
			locals[paramCount] = *getTLONG();
			locals[paramCount+1] = *getTWIDE();
			paramCount += 2;
			break;
			
		case 'D':
			if (paramCount + 1 > v->method->localsz) {
				return localOverflowErrorInLoadInitialArgs(v, argbuf);
			}
			locals[paramCount] = *getTDOUBLE();
			locals[paramCount+1] = *getTWIDE();
			paramCount += 2;
			break;
			
		case '[':
		case 'L':
			newsig = checkPtr(gc_malloc((strlen(argbuf) + 1) * sizeof(char), KGC_ALLOC_VERIFIER));
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
 * returns a pointer to the first character of the method's return value
 * in the method's type descriptor.
 */
const char*
getMethodReturnSig(const Method* method)
{
	const char* sig = METHOD_SIGD(method);
	
	/* skip the type parameters */
	for (sig++; *sig != ')'; sig = parseFieldTypeDescriptor(sig));
	sig++;
	
	return sig;
}
