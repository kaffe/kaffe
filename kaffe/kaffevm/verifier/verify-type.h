/*
 * verify-type.h
 *
 * Copyright 2004
 *   Kaffe.org contributors. See ChangeLog for details. All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Interface to code for handing of types in the verifier.
 */

#if !defined(VERIFY_TYPE_H)
#define VERIFY_TYPE_H

#include "classMethod.h"


struct Verifier;


/* structure to hold the merger of two types in the event of
 * multiple inheritence.
 * it is implemented as a stack.
 *
 * also is a list of all supertype lists allocated during
 * the verification of a specific method.  the memory used here
 * is freed after verification of the method is completed.
 */
typedef struct SupertypeSet
{
	uint32 count;
	Hjava_lang_Class** list;
	
	struct SupertypeSet* next;
} SupertypeSet;


/* represents a Type, which could get a primitive type, a reference type,
 * or a system type such as an address or uninitialized.
 */
typedef struct Type
{
	uint32 tinfo;
	
	union {
	        /* different ways to refer to an object reference */
		const char* name;
		const char* sig;
		Hjava_lang_Class* class;
		
	        /* uninitialized object reference */
		struct UninitializedType* uninit;
		
		SupertypeSet* supertypes;
		
	        /* return address for TINFO_ADDR */
		uint32 addr;
	} data;
} Type;

/* status flags for opstack/local info arrays
 *
 *   TINFO_SYSTEM       internal type, such as UNSTABLE or VOID
 *   TINFO_ADDR         return address type
 *   TINFO_PRIMITIVE    Type.data.class is some primitive class, like intClass
 *   TINFO_CLASS        Type.data.class
 *   TINFO_NAME         Type.data.name represents the class' fully qualified name
 *   TINFO_SIG          Type.data.sig  represents the class' fully qualified type signature
 *   TINFO_UNINIT       is a class instance created by NEW that has yet to be initialized.
 *                      the type is really an (UninitializedType*), so that dups, moves, etc. ensure that whatever
 *                      copies of the type are around are all initialized when the <init>() is called.
 *   TINFO_UNINIT_SUPER reserved for the self-reference in a constructor method.  when the receiver of a call to <init>()
 *                      is of type TINFO_UNINIT_SUPER, then the <init>() referenced may be in the current class of in its
 *                      superclass.
 *   TINFO_SUPERTYPES   a set of supertypes.  used when merging two types that have multiple common supertypes.
 *                      this can occur with the multiple inheritence of interfaces.
 *                      the zeroth element is always a common superclass, the rest are common superinterfaces.
 */
#define TINFO_SYSTEM       0
#define TINFO_ADDR         1
#define TINFO_PRIMITIVE    2
#define TINFO_SIG          4
#define TINFO_NAME         8
#define TINFO_CLASS        16
#define TINFO_UNINIT       32
#define TINFO_UNINIT_SUPER 96
#define TINFO_SUPERTYPES   128

#define IS_ADDRESS(_TINFO) ((_TINFO)->tinfo & TINFO_ADDR)
#define IS_PRIMITIVE_TYPE(_TINFO) ((_TINFO)->tinfo & TINFO_PRIMITIVE)

/*
 * types for type checking (pass 3b)
 */
extern Type* getTUNSTABLE(void);

extern Type* getTINT(void);
extern Type* getTFLOAT(void);
extern Type* getTLONG(void);
extern Type* getTDOUBLE(void);
extern Type* getTNULL(void);
extern Type* getTWIDE(void);

extern Type* getTOBJ(void);

extern Type* getTOBJARR(void);
extern Type* getTCHARARR(void);
extern Type* getTBYTEARR(void);
extern Type* getTBOOLARR(void);
extern Type* getTSHORTARR(void);
extern Type* getTINTARR(void);
extern Type* getTLONGARR(void);
extern Type* getTFLOATARR(void);
extern Type* getTDOUBLEARR(void);

extern Type* getTSTRING(void);

extern void initVerifierPrimTypes(void);

extern bool isNull(const Type * t);
extern bool isWide(const Type * t);

extern bool isReference(const Type* t);
extern bool isArray(const Type* t);

extern bool sameRefType(Type* t1,
			Type* t2);
extern bool sameType(Type* t1,
		     Type* t2);
extern void resolveType(struct Verifier* v,
			Type *t);

extern bool mergeTypes(struct Verifier* v,
		       Type* t1,
		       Type* t2);
extern Hjava_lang_Class*  getCommonSuperclass(Hjava_lang_Class* t1,
					      Hjava_lang_Class* t2);

extern bool typecheck(struct Verifier* v,
		      Type* t1,
		      Type* t2);


/* for dealing with the supertype lists */
extern void createSupertypeSet(struct Verifier* v,
			       Hjava_lang_Class* class_a,
			       uint32 num_interfaces_a,
			       Hjava_lang_Class** interfaces_a,
			       Hjava_lang_Class* class_b,
			       uint32 num_interfaces_b,
			       Hjava_lang_Class** interfaces_b);
extern void freeSupertypes(SupertypeSet* supertypes);


#endif /* !defined(VERIFY_TYPE_H) */
