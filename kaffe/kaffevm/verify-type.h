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

typedef struct Type
{
	uint32 tinfo;
	
	union {
	        /* different ways to refer to an object reference */
		const char* name;
		const char* sig;
		Hjava_lang_Class* class;
		
	        /* uninitialized object reference */
		struct unitialized_types_double_list* uninit;
		
	        /* list of supertypes in the event of multiple inheritence of interfaces. */
		Hjava_lang_Class** supertypes;
		
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
 *   TINFO_SUPERLIST    a list of supertypes.  used when merging two types that have multiple common supertypes.
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
#define TINFO_SUPERLIST    128

#define IS_ADDRESS(_TINFO) ((_TINFO)->tinfo & TINFO_ADDR)
#define IS_PRIMITIVE_TYPE(_TINFO) ((_TINFO)->tinfo & TINFO_PRIMITIVE)

/*
 * types for type checking (pass 3b)
 */
extern Type* TUNSTABLE;

extern Type* TINT;
extern Type* TFLOAT;
extern Type* TLONG;
extern Type* TDOUBLE;
extern Type* TNULL;
extern Type* TWIDE;
extern Type* TOBJ;

extern Type* TOBJARR;
extern Type* TCHARARR;
extern Type* TBYTEARR;
extern Type* TBOOLARR;
extern Type* TSHORTARR;
extern Type* TINTARR;
extern Type* TLONGARR;
extern Type* TFLOATARR;
extern Type* TDOUBLEARR;

extern Type* TSTRING;

extern void initVerifierPrimTypes(void);
extern bool isNull(const Type * tinfo);
extern bool isWide(const Type * tinfo);

extern bool isReference(const Type* type);
extern bool isArray(const Type* type);
extern bool sameRefType(Type* t1, Type* t2);
extern bool sameType(Type* t1, Type* t2);
extern void resolveType(errorInfo* einfo, Hjava_lang_Class* this, Type *type);

extern bool mergeTypes(errorInfo*, Hjava_lang_Class* this,
				     Type* t1, Type* t2);
extern Hjava_lang_Class*  getCommonSuperclass(Hjava_lang_Class* t1,
					      Hjava_lang_Class* t2);

extern bool typecheck(errorInfo*, Hjava_lang_Class* this, Type* t1, Type* t2);

#endif /* !defined(VERIFY_TYPE_H) */
