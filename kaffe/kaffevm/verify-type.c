/*
 * verify-type.c
 *
 * Copyright 2004
 *   Kaffe.org contributors. See ChangeLog for details. All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Code for handing of types in the verifier.
 */

#include "debug.h"
#include "itypes.h"
#include "soft.h"
#include "verify-debug.h"
#include "verify-type.h"
#include "verify-uninit.h"

/*
 * types for type checking (pass 3b)
 */
static Type  verify_UNSTABLE;
Type* TUNSTABLE = &verify_UNSTABLE;

static Type  verify_INT;
Type* TINT = &verify_INT;
       
static Type  verify_FLOAT;
Type* TFLOAT = &verify_FLOAT;
       
static Type  verify_LONG;
Type* TLONG = &verify_LONG;
       
static Type  verify_DOUBLE;
Type* TDOUBLE = &verify_DOUBLE;

/* used for the second space of LONGs and DOUBLEs
 * in local variables or on the operand stack
 */
static Type  _WIDE;
Type* TWIDE = &_WIDE;

bool
isWide(const Type * tinfo)
{
	return (tinfo->data.class == TWIDE->data.class);
}

static Type  verify_NULL;
Type* TNULL = &verify_NULL;

bool
isNull(const Type * tinfo)
{
	return (tinfo->data.class == TNULL->data.class);
}

static const char* OBJECT_SIG  = "Ljava/lang/Object;";
static Type  verify_OBJ;
Type* TOBJ = &verify_OBJ;

static const char* OBJARR_SIG = "[Ljava/lang/Object;";
static Type  verify_OBJARR;
Type* TOBJARR = &verify_OBJARR;

static const char* STRING_SIG = "Ljava/lang/String;";
static Type  verify_STRING;
Type* TSTRING = &verify_STRING;

static const char* CHARARR_SIG = "[C";
static Type  verify_CHARARR;
Type* TCHARARR = &verify_CHARARR;

static const char* BYTEARR_SIG = "[B";
static Type  verify_BYTEARR;
Type* TBYTEARR = &verify_BYTEARR;

static const char* BOOLARR_SIG = "[Z";
static Type  verify_BOOLARR;
Type* TBOOLARR = &verify_BOOLARR;

static const char* SHORTARR_SIG = "[S";
static Type  verify_SHORTARR;
Type* TSHORTARR = &verify_SHORTARR;

static const char* INTARR_SIG = "[I";
static Type  verify_INTARR;
Type* TINTARR = &verify_INTARR;

static const char* LONGARR_SIG = "[J";
static Type  verify_LONGARR;
Type* TLONGARR = &verify_LONGARR;

static const char* FLOATARR_SIG = "[F";
static Type  verify_FLOATARR;
Type* TFLOATARR = &verify_FLOATARR;

static const char* DOUBLEARR_SIG = "[D";
static Type  verify_DOUBLEARR;
Type* TDOUBLEARR = &verify_DOUBLEARR;

/*
 * Initialize Type structures needed for verification
 */
void
initVerifierPrimTypes(void)
{
	TUNSTABLE->tinfo = TINFO_SYSTEM;
	TUNSTABLE->data.class = (Hjava_lang_Class*)TUNSTABLE;
	
	TWIDE->tinfo = TINFO_SYSTEM;
	TWIDE->data.class = (Hjava_lang_Class*)TWIDE;
	
	
	TINT->tinfo = TINFO_PRIMITIVE;
	TINT->data.class = intClass;
	
	TLONG->tinfo = TINFO_PRIMITIVE;
	TLONG->data.class = longClass;
	
	TFLOAT->tinfo = TINFO_PRIMITIVE;
	TFLOAT->data.class = floatClass;
	
	TDOUBLE->tinfo = TINFO_PRIMITIVE;
	TDOUBLE->data.class = doubleClass;
	
	
	TNULL->tinfo = TINFO_CLASS;
	TNULL->data.class = (Hjava_lang_Class*)TNULL;
	
	TOBJ->tinfo = TINFO_SIG;
	TOBJ->data.sig = OBJECT_SIG;
	
	TOBJARR->tinfo = TINFO_SIG;	
	TOBJARR->data.sig = OBJARR_SIG;
	
	
	TSTRING->data.sig = STRING_SIG;
	TSTRING->tinfo = TINFO_SIG;
	
	
	TCHARARR->tinfo = TINFO_SIG;
	TCHARARR->data.sig = CHARARR_SIG;
	
	TBYTEARR->tinfo = TINFO_SIG;
	TBYTEARR->data.sig = BYTEARR_SIG;
	
	TBOOLARR->tinfo = TINFO_SIG;
	TBOOLARR->data.sig = BOOLARR_SIG;
	
	TSHORTARR->tinfo = TINFO_SIG;
	TSHORTARR->data.sig = SHORTARR_SIG;
	
	TINTARR->tinfo = TINFO_SIG;
	TINTARR->data.sig = INTARR_SIG;
	
	TLONGARR->tinfo = TINFO_SIG;
	TLONGARR->data.sig = LONGARR_SIG;
	
	TFLOATARR->tinfo = TINFO_SIG;
	TFLOATARR->data.sig = FLOATARR_SIG;
	
	TDOUBLEARR->tinfo = TINFO_SIG;	
	TDOUBLEARR->data.sig = DOUBLEARR_SIG;
}


/*
 * resolveType()
 *     Ensures that the type is a pointer to an instance of Hjava_lang_Class.
 */
void
resolveType(errorInfo* einfo, Hjava_lang_Class* this, Type *type)
{
	const char* sig;
	char* tmp = NULL;

	if (type->tinfo & TINFO_NAME) {
		sig = type->data.sig;
		
		if (*sig != '[') {
			tmp = checkPtr(gc_malloc((strlen(sig) + 3) * sizeof(char), GC_ALLOC_VERIFIER));
			sprintf(tmp, "L%s;", sig);
			sig = tmp;
		}
		
		type->tinfo = TINFO_CLASS;
		type->data.class = getClassFromSignature(sig, this->loader, einfo);
		
		if (tmp) {
			gc_free(tmp);
		}
	}
	else if (type->tinfo & TINFO_SIG) {
		type->tinfo = TINFO_CLASS;
		type->data.class = getClassFromSignature(type->data.sig, this->loader, einfo);
	}
}


/*
 * mergeTypes()
 *     merges two types, t1 and t2, into t2.  this result could
 *     be a common superclass, a common class that both types implement, or,
 *     in the event that the types are not compatible, TUNSTABLE.
 *
 * returns whether an actual merger was made (i.e. they weren't the same type)
 *
 * note: the precedence of merged types goes (from highest to lowest):
 *     actual pointer to Hjava_lang_Class*
 *     TINFO_SIG
 *     TINFO_NAME
 *
 * TODO: right now the priority is to be a common superclass, as stated in
 *       the JVML2 specs.  a better verification technique might check this first,
 *       and then check interfaces that both classes implement.  of course, depending
 *       on the complexity of the inheritance hirearchy, this could take a lot of time.
 *       
 *       the ideal solution is to remember *all* possible highest resolution types,
 *       which, of course, would require allocating more memory on the fly, etc., so,
 *       at least for now, we're not really even considering it.
 */
bool
mergeTypes(errorInfo* einfo, Hjava_lang_Class* this,
	   Type* t1, Type* t2)
{
	if (IS_ADDRESS(t1) || IS_ADDRESS(t2)) {
	        /* if one of the types is TADDR, the other one must also be TADDR */
		if (t1->tinfo != t2->tinfo) {
			return false;
		}
		
		/* TODO: should this be an error if they don't agree? */
		t2->tinfo = t1->tinfo;
		return true;
	}
	else if (t2->data.class == TUNSTABLE->data.class || sameType(t1, t2)) {
		return false;
	}
	else if (t1->tinfo & TINFO_UNINIT || t2->tinfo & TINFO_UNINIT ||
		 !isReference(t1) || !isReference(t2)) {
		
		*t2 = *TUNSTABLE;
		return true;
	}
	/* references only from here on out */
	else if (t1->data.class == TOBJ->data.class) {
		*t2 = *t1;
		return true;
	}
	
	
	/* not equivalent, must resolve them */
	resolveType(einfo, this, t1);
	if (t1->data.class == NULL) {
		return false;
	}

	resolveType(einfo, this, t2);
	if (t2->data.class == NULL) {
		return false;
	}
	
	if (CLASS_IS_INTERFACE(t1->data.class) &&
	    instanceof_interface(t1->data.class, t2->data.class)) {
	
		/* t1 is an interface and t2 implements it,
		 * so the interface is the merged type.
		 */

		*t2 = *t1;
		
		return true;
	
	} else if (CLASS_IS_INTERFACE(t2->data.class) &&
		   instanceof_interface(t2->data.class, t1->data.class)) {
		
		/* same as above, but we don't need to merge, since
		 * t2 already is the merged type
		 */

		return false;
	} else {
		/*
		 * neither of the types is an interface, so we have to
		 * check for common superclasses. Only merge iff t2 is
		 * not the common superclass.
		 */
		Hjava_lang_Class *tmp = t2->data.class;
		
		t2->data.class = getCommonSuperclass(t1->data.class, t2->data.class);
		
		return tmp != t2->data.class;
	} 
}


/*
 * returns the first (highest) common superclass of classes A and B.
 *
 * precondition: neither type is an array type
 *               nor is either a primitive type
 */
Hjava_lang_Class*
getCommonSuperclass(Hjava_lang_Class* t1, Hjava_lang_Class* t2)
{
	Hjava_lang_Class* A;
	Hjava_lang_Class* B;
	
	for (A = t1; A != NULL; A = A->superclass) {
		for (B = t2; B != NULL; B = B->superclass) {
			if (A == B) return A;
		}
	}
	
	/* error of some kind...at the very least, we shoulda gotten to Object
	 * when traversing the class hirearchy
	 */
	return TUNSTABLE->data.class;
}


/*
 * isReference()
 *    returns whether the type is a reference type
 */
bool
isReference(const Type* type)
{
	return (type->tinfo & TINFO_NAME ||
		type->tinfo & TINFO_SIG ||
		type->tinfo & TINFO_CLASS ||
		type->tinfo & TINFO_UNINIT);
}

/*
 * isArray()
 *     returns whether the Type is an array Type
 */
bool
isArray(const Type* type)
{
	if (!isReference(type)) {
		return false;
	}
	else if (type->tinfo & TINFO_NAME || type->tinfo & TINFO_SIG) {
		return (*(type->data.sig) == '[');
	}
	else if (type->tinfo != TINFO_CLASS) {
		return false;
	}
	else {
		return (*(CLASS_CNAME(type->data.class)) == '[');
	}
}


/*
 * sameType()
 *     returns whether two Types are effectively equivalent.
 */
bool
sameType(Type* t1, Type* t2)
{
	switch (t1->tinfo) {
	case TINFO_SYSTEM:
		return (t2->tinfo == TINFO_SYSTEM &&
			t1->data.class == t2->data.class);
		
	case TINFO_ADDR:
		return (t2->tinfo == TINFO_ADDR &&
			t1->data.addr == t2->data.addr);
		
	case TINFO_PRIMITIVE:
		return (t2->tinfo == TINFO_PRIMITIVE &&
			t1->data.class == t2->data.class);
		
	case TINFO_UNINIT:
	case TINFO_UNINIT_SUPER:
		return (t2->tinfo & TINFO_UNINIT &&
			(t1->data.uninit == t2->data.uninit ||
			 sameRefType(&(t1->data.uninit->type),
				     &(t2->data.uninit->type))));
		
	default:
		DBG(VERIFY3, dprintf("%ssameType(): unrecognized tinfo (%d)\n", indent, t1->tinfo); );
		return false;
		
	case TINFO_SIG:
	case TINFO_NAME:
	case TINFO_CLASS:
		return ((t2->tinfo == TINFO_SIG ||
			 t2->tinfo == TINFO_NAME || 
			 t2->tinfo == TINFO_CLASS) &&
			sameRefType(t1,t2));
	}
}

/*
 * sameRefType()
 *     returns whether two Types are effectively equivalent.
 *
 *     pre: t1 and t2 are both reference types
 */
bool
sameRefType(Type* t1, Type* t2)
{
	const char* sig1 = NULL;
	const char* sig2 = NULL;
	uint32 len1, len2;
	
	if (isNull(t1) || isNull(t2)) {
		return true;
	}
	
	if (t1->tinfo & TINFO_NAME) {
		sig1 = t1->data.name;
		
		if (t2->tinfo & TINFO_NAME) {
			return (!strcmp(sig1, t2->data.name));
		}
		else if (t2->tinfo & TINFO_SIG) {
			sig2 = t2->data.sig;
			
			len1 = strlen(sig1);
			len2 = strlen(sig2);
			
			sig2++;
			if ((len1 + 2 != len2) || strncmp(sig1, sig2, len1))
				return false;
		}
		else {
			if (strcmp(sig1, CLASS_CNAME(t2->data.class)))
				return false;
		}
		
		*t1 = *t2;
		return true;
	}
	else if (t1->tinfo & TINFO_SIG) {
		sig1 = t1->data.sig;
		
		if (t2->tinfo & TINFO_SIG) {
			return (!strcmp(sig1, t2->data.sig));
		}
		else if (t2->tinfo & TINFO_NAME) {
			sig2 = t2->data.name;
			
			len1 = strlen(sig1);
			len2 = strlen(sig2);
			sig1++;
			
			if ((len1 != len2 + 2) || strncmp(sig1, sig2, len2))
				return false;
			
			*t2 = *t1;
			return true;
		}
		else {
			sig2 = CLASS_CNAME(t2->data.class);
			
			len1 = strlen(sig1);
			len2 = strlen(sig2);
			sig1++;
			
			if ((len1 != len2 + 2) || strncmp(sig1, sig2, len2))
				return false;
			
			*t1 = *t2;
			return true;
		}
	}
	else {
		sig1 = CLASS_CNAME(t1->data.class);
		
		if (t2->tinfo & TINFO_SIG) {
			sig2 = t2->data.sig;
			
			len1 = strlen(sig1);
			len2 = strlen(sig2);
			sig2++;
			if ((len1 + 2 != len2) || strncmp(sig1, sig2, len1))
				return false;
			
			*t2 = *t1;
			return true;
		}
		else if (t2->tinfo & TINFO_NAME) {
			sig2 = t2->data.name;
			
			if (strcmp(sig1, sig2))
				return false;
			
			*t2 = *t1;
			return true;
		}
		else {
		        /* we should never get here */
			sig2 = CLASS_CNAME(t2->data.class);
			return (!strcmp(sig1, sig2));
		}
	}
}


/*
 * returns whether t2 can be a t1
 */
bool
typecheck(errorInfo* einfo, Hjava_lang_Class* this, Type* t1, Type* t2)
{
	DBG(VERIFY3, dprintf("%stypechecking ", indent); printType(t1); dprintf("  vs.  "); printType(t2); dprintf("\n"); );
	
	if (sameType(t1, t2)) {
		return true;
	}
	else if (t1->tinfo & TINFO_UNINIT || t2->tinfo & TINFO_UNINIT) {
		return false;
	}
	else if (!isReference(t1) || !isReference(t2)) {
		return false;
	}
	else if (sameType(t1, TOBJ)) {
		return true;
	}

	resolveType(einfo, this, t1);
	if (t1->data.class == NULL) {
		return false;
	}

	resolveType(einfo, this, t2);
	if (t2->data.class == NULL) {
		return false;
	}

	return instanceof(t1->data.class, t2->data.class);
}
