/*
 * soft.h
 * Soft instruction prototypes.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __soft_h
#define	__soft_h

struct _dispatchTable;
struct Hjava_lang_Class;
struct Hjava_lang_Object;

void*	soft_new(struct Hjava_lang_Class*);
void*	soft_newarray(jint, jint);
void*	soft_anewarray(struct Hjava_lang_Class*, jint);
void	soft_initialise_class(struct Hjava_lang_Class*);
nativecode* soft_get_method_code (Method*);

void*	soft_lookupmethod(struct Hjava_lang_Object*, Utf8Const*, Utf8Const*);
void*	soft_checkcast(struct Hjava_lang_Class*, struct Hjava_lang_Object*);
jint	soft_instanceof(struct Hjava_lang_Class*, struct Hjava_lang_Object*);

void	soft_athrow(struct Hjava_lang_Object*);
void	soft_badarrayindex(void);
void	soft_nullpointer(void);
void	soft_nosuchmethod(struct Hjava_lang_Class*, Utf8Const*, Utf8Const*);
void	soft_checkarraystore(struct Hjava_lang_Object*, struct Hjava_lang_Object*);
void	soft_addreference(void*, void*);

jint	soft_dcmpg(jdouble, jdouble);
jint	soft_dcmpl(jdouble, jdouble);
jint	soft_fcmpg(jfloat, jfloat);
jint	soft_fcmpl(jfloat, jfloat);

jint	soft_mul(jint, jint);
jint	soft_div(jint, jint);
jint	soft_rem(jint, jint);

#if defined(INTERPRETER)
void*	soft_multianewarray(struct Hjava_lang_Class*, jint, slots*);
#endif

#if defined(TRANSLATOR)
jlong	soft_lmul(jlong, jlong);
jlong	soft_ldiv(jlong, jlong);
jlong	soft_lrem(jlong, jlong);
jfloat	soft_fdiv(jfloat, jfloat);
jdouble	soft_fdivl(jdouble, jdouble);
jfloat	soft_frem(jfloat, jfloat);
jdouble	soft_freml(jdouble, jdouble);
jlong	soft_lshll(jlong, jint);
jlong	soft_ashrl(jlong, jint);
jlong	soft_lshrl(jlong, jint);
jint	soft_lcmp(jlong, jlong);
void*	soft_multianewarray(struct Hjava_lang_Class*, jint, ...);
#endif

jlong	soft_cvtil(jint);
jfloat	soft_cvtif(jint);
jdouble	soft_cvtid(jint);
jint	soft_cvtli(jlong);
jfloat	soft_cvtlf(jlong);
jdouble	soft_cvtld(jlong);
jint	soft_cvtfi(jfloat);
jlong	soft_cvtfl(jfloat);
jdouble	soft_cvtfd(jfloat);
jint	soft_cvtdi(jdouble);
jlong	soft_cvtdl(jdouble);
jfloat	soft_cvtdf(jdouble);

jint instanceof(struct Hjava_lang_Class*, struct Hjava_lang_Class*);
jint instanceof_class(struct Hjava_lang_Class*, struct Hjava_lang_Class*);
jint instanceof_array(struct Hjava_lang_Class*, struct Hjava_lang_Class*);

#endif
