/*
 * verify.h
 *
 * only publically accessible (i.e. required outside of the verifier)
 * are declared here.
 *
 * copyright 2003, Rob Gonzalez (rob@kaffe.org)
 */

#ifndef __verify_h
#define __verify_h

#include "classMethod.h"
#include "errors.h"
#include "gtypes.h"


/*****************************************************
 * For dealing with instructions and bytecode.
 *****************************************************/

/* lengths in bytes of all the instructions */
extern const uint8 insnLen[256];

static inline
uint32
getNextPC(const unsigned char * code, const uint32 pc)
{
	return (pc + insnLen[code[pc]]);
}

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

static inline
uint32
getIdx(const unsigned char * code, const uint32 pc)
{
	return ((uint32) code[pc + 1]);
}

static inline
uint32
getWIdx(const unsigned char * code, const uint32 pc)
{
	return ((uint32) getWord(code, pc + 1));
}


/*****************************************************
 * This structure eases memory management and parameter
 * passing by storing all dynamically allocated structures
 * that are passed around a lot in one place.
 *
 * The verification of one method will have one Verifier
 * associated with it.
 *****************************************************/
typedef struct Verifier
{
	/* error object needed in all methods */
	errorInfo* einfo;
	
	/* the method being verified */
	Hjava_lang_Class* class; /* not const because classloader may need it */
	const Method* method;
	
	/* information about the basic blocks */
	uint32             numBlocks;
	uint32*            status;
	struct BlockInfo** blocks;
	
	/* memory allocated for type checking */
	struct SigStack*          sigs;
	struct UninitializedType* uninits;
	struct SupertypeSet*      supertypes;
} Verifier;

/* frees the data allocated and stored in a Verifier */
extern void freeVerifierData(Verifier* v);


/*****************************************************
 * Methods used in both pass 2 and pass 3
 *****************************************************/
extern bool isTrustedClass(Hjava_lang_Class* class);
extern const char* parseBaseTypeDescriptor(const char* sig);
extern const char* parseObjectTypeDescriptor(const char* sig);
extern const char* parseArrayTypeDescriptor(const char* sig);
extern const char* parseFieldTypeDescriptor(const char* sig);
extern bool parseMethodTypeDescriptor(const char* sig);

/*****************************************************
 * Pass 2
 *****************************************************/
extern bool verify2(Hjava_lang_Class* class, errorInfo *einfo);

/*****************************************************
 * Pass 3
 *****************************************************/
extern bool verify3(Hjava_lang_Class* class,
		    errorInfo *einfo);
extern void verifyMethod3a(struct Verifier* v);
extern bool verifyMethod3b(struct Verifier* v);
extern bool verifyBasicBlock(struct Verifier* v,
			     struct BlockInfo*);

extern bool checkMethodCall(struct Verifier* v,
			    struct BlockInfo* binfo,
			    uint32 pc);
extern const char* getMethodReturnSig(const Method*);

#endif /* __verify_h */
