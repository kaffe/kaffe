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


/**
 * This structure eases memory management and parameter
 * passing by storing all dynamically allocated structures
 * that are passed around a lot in one place.
 *
 * The verification of one method will have one Verifier
 * associated with it.
 */
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


/**********************************************************
 * Methods used in both pass 2 and pass 3
 **********************************************************/

extern bool isTrustedClass(Hjava_lang_Class* class);
extern const char* parseBaseTypeDescriptor(const char* sig);
extern const char* parseObjectTypeDescriptor(const char* sig);
extern const char* parseArrayTypeDescriptor(const char* sig);
extern const char* parseFieldTypeDescriptor(const char* sig);
extern bool parseMethodTypeDescriptor(const char* sig);

/**********************************************************
 * Pass 2
 **********************************************************/
extern bool verify2(Hjava_lang_Class* class, errorInfo *einfo);

/**********************************************************
 * Pass 3
 **********************************************************/

extern bool verify3(Hjava_lang_Class* class, errorInfo *einfo);

#endif /* __verify_h */
