/*
 * javacall.c
 * Building call for java/native methods.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2002-2005 
 *      Kaffe.org contributors. See ChangeLog for details. All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "debug.h"
#include "config.h"

#if !defined(HAVE_LIBFFI)
#define NEED_sysdepCallMethod 1
#endif

#include "config-std.h"
#include "config-mem.h"
#include <stdarg.h>
#include "errors.h"
#include "jni_md.h"
#include "kaffe/jmalloc.h"
#include "gtypes.h"
#include "object.h"
#include "constants.h"
#include "baseClasses.h"
#include "slots.h"
#include "support.h"
#include "classMethod.h"
#include "machine.h"
#include "md.h"
#include "itypes.h"
#include "external.h"
#include "thread.h"
#include "jthread.h"
#include "methodcalls.h"
#include "native.h"
#include "jni_i.h"

/* This is defined in the alpha port.  It causes all integer arguments
   to be promoted to jlong, and all jfloats to be promoted to jdouble,
   unless explicitly overridden.  It implies NO_HOLES, unless
   explicitly defined otherwise.  */
#if defined(PROMOTE_TO_64bits)
# if !defined(PROMOTE_jint2jlong)
#  define PROMOTE_jint2jlong 1
# endif
# if !defined(PROMOTE_jfloat2jdouble)
#  define PROMOTE_jfloat2jdouble 1
# endif
/* NO_HOLES causes all types to occupy only one slot in callargs, but
   not affecting their callsizes, that can still be used to
   distinguish among types.  */
# if !defined(NO_HOLES)
#  define NO_HOLES 1
# endif
#endif

/* Make sure that undefined macros are defined as 0 */
#if !defined(PROMOTE_jfloat2jdouble)
#define PROMOTE_jfloat2jdouble 0
#endif

#if !defined(PROMOTE_jint2jlong)
#define PROMOTE_jint2jlong 0
#endif

#if !defined(NO_HOLES)
#define NO_HOLES 0
#endif

/* If PROMOTE_jint2jlong is enabled, all integer values are to be
   passed as jlongs.  It is only set by PROMOTE_TO_64bits.  */
#if PROMOTE_jint2jlong
# define PROM_i j
#else
# define PROM_i i
#endif

/* If PROMOTE_jfloat2jdouble is enabled, jfloats are to be passed as
   jdoubles.  Note that, when a jfloat is promoted, its calltype will
   be marked as 'D'.  No known port uses this.  In fact, alpha must
   explicitly set it to 0, to prevent PROMOTE_TO_64bits from enabling
   it.  */
#if PROMOTE_jfloat2jdouble
# define PROM_f d
#else
# define PROM_f f
#endif

/* This is currently defined in the MIPS O32 port.  It causes jlongs
   and jdoubles to be forced into even arguments, by introducing a
   padding integer argument. The argument DO is used to adjust the
   input argument list.  */
#if defined(ALIGN_AT_64bits)
# if NO_HOLES
#  error "ALIGN_AT_64bits is incompatible with NO_HOLES"
# endif
# define ENSURE_ALIGN64(DO) do { \
		if (call.callsize[i] == 2 && (i & 1)) { \
			char tmptype = call.calltype[i]; \
			call.callsize[i] = 0; \
			call.calltype[i] = 'I'; \
			DO; \
			++i; ++s; \
			call.calltype[i] = tmptype; \
			call.callsize[i] = 2; \
		} \
	} while (0)
#else
# define ENSURE_ALIGN64(DO) do {} while (0)
#endif

/**
 * Generic routine to call a native or Java method (array style).
 *
 * @param meth the struct _jmethodID of the method to be executed
 * @param func the code that's to be executed
 * @param obj  the object whose method is to be called (my be 0 iff method is static)
 * @param args the arguments to be passed to the method
 * @param ret  buffer for the return value of the method (may be 0 iff return type is void)
 * @param promoted true iff 64 bit values occupy two entries in args, otherwise false
 */
void
KaffeVM_callMethodA(Method* meth, void* func, void* obj, jvalue* args, jvalue* ret,
		    int promoted)
{
	int i;
	int j;
	int s;
	int numArgs;
	callMethodInfo call;	
	jvalue tmp;

	if (ret == 0) {
		ret = &tmp;
	}
	if (!NO_HOLES)
	        numArgs = sizeofSigMethod(meth, false);
	else
	        numArgs = METHOD_NARGS(meth);

	i = engine_reservedArgs(meth);
	s = 0;
	
	call.args = (jvalue *)alloca((numArgs+engine_reservedArgs(meth)+2)*(sizeof(jvalue)+2));
	call.callsize = (char *)&call.args[numArgs+engine_reservedArgs(meth)+2];
	call.calltype = (char *)&call.callsize[numArgs+engine_reservedArgs(meth)+2];

	/* If this method isn't static, we must insert the object as
	 * an argument.
 	 */
	if ((meth->accflags & ACC_STATIC) == 0) {
		call.callsize[i] = PTR_TYPE_SIZE / SIZEOF_INT;
		s += call.callsize[i];
		call.calltype[i] = 'L';
		call.args[i].l = obj;
		i++;
	}

	for (j = 0; j < METHOD_NARGS(meth); i++, j++) {
		call.calltype[i] = *METHOD_ARG_TYPE(meth, j);
		switch (call.calltype[i]) {
		case 'Z':
			if (promoted) goto use_int;
			call.callsize[i] = 1;
			call.args[i].PROM_i = args[j].z;
			break;

		case 'S':
			if (promoted) goto use_int;
			call.callsize[i] = 1;
			call.args[i].PROM_i = args[j].s;
			break;

		case 'B':
			if (promoted) goto use_int;
			call.callsize[i] = 1;
			call.args[i].PROM_i = args[j].b;
			break;

		case 'C':
			if (promoted) goto use_int;
			call.callsize[i] = 1;
			call.args[i].PROM_i = args[j].c;
			break;

		case 'F':
			call.callsize[i] = 1;
			call.args[i].PROM_f = args[j].f;
			if (PROMOTE_jfloat2jdouble) {
				call.calltype[i] = 'D';
			}
			break;
		case 'I':
		use_int:
			call.callsize[i] = 1;
			call.args[i].PROM_i = args[j].i;
			break;
		case 'D':
		case 'J':
			call.callsize[i] = 2;
			ENSURE_ALIGN64({});
			call.args[i] = args[j];
			if (promoted) { /* compensate for the second array element by incrementing args */
			  args++;
			}
			if (! NO_HOLES) {
				s += call.callsize[i];
				call.args[i+1].i = (&call.args[i].i)[1];
				i++; 
				call.calltype[i] = 0;
				call.callsize[i] = 0;
			}
			break;
		case '[':
			call.calltype[i] = 'L';
			/* fall through */
		case 'L':
			call.callsize[i] = PTR_TYPE_SIZE / SIZEOF_INT;
			call.args[i].l = unveil(args[j].l);
			break;
		default:
			KAFFEVM_ABORT();
		}
		s += call.callsize[i];
	}

#if defined(STACK_LIMIT)
	call.calltype[i] = 'L';
	call.callsize[i] = PTR_TYPE_SIZE / SIZEOF_INT;
	call.args[i].l = KTHREAD(stacklimit)();
	s += PTR_TYPE_SIZE / SIZEOF_INT;
	i++;
#endif

	/* Return info */
	call.rettype = *METHOD_RET_TYPE(meth);
	switch (call.rettype) {
	case '[':
		call.rettype = 'L';
		/* fall through */
	case 'L':
		call.retsize = PTR_TYPE_SIZE / SIZEOF_INT;
		break;
	case 'V':
		call.retsize = 0;
		break;
	case 'D':
	case 'J':
		call.retsize = 2;
		break;
	default:
		call.retsize = 1;
		break;
	}

	/* Call info and arguments */
	call.nrargs = i;
	call.argsize = s;
	call.ret = ret;
	call.function = func;

	/* GCDIAG wipes free memory with 0xf4... */
	assert(call.function);
	assert(*(uint32*)(call.function) != 0xf4f4f4f4);

	/* Make the call - system dependent */
	engine_callMethod(&call);

	memset(call.args, 0, (METHOD_NARGS(meth)+engine_reservedArgs(meth)+2)*(sizeof(jvalue)+2));
	memset(&call, 0, sizeof(call));
}

/**
 * Generic routine to call a native or Java method (varargs style).
 *
 * @param meth the struct _jmethodID of the method to be executed
 * @param func the code that's to be executed
 * @param obj  the object whose method is to be called (my be 0 iff method is static)
 * @param args the arguments to be passed to the method
 * @param ret  buffer for the return value of the method (may be 0 iff return type is void)
 */
void
KaffeVM_callMethodV(Method* meth, void* func, void* obj, va_list args, jvalue* ret)
{
  /* const char* sig; FIXME */
	int i;
	int s;
	int j;
	callMethodInfo call;
	jvalue tmp;
	int numArgs;

	if (ret == 0) {
		ret = &tmp;
	}
	i = engine_reservedArgs(meth);
	s = 0;
	
	if (!NO_HOLES)
	        numArgs = sizeofSigMethod(meth, false);
	else
	        numArgs = METHOD_NARGS(meth);

	call.args = (jvalue *)alloca((numArgs+engine_reservedArgs(meth)+2)*(sizeof(jvalue)+2));
	call.callsize = (char *)&call.args[numArgs+engine_reservedArgs(meth)+2];
	call.calltype = (char *)&call.callsize[numArgs+engine_reservedArgs(meth)+2];

	/* If this method isn't static, we must insert the object as
	 * the first argument and get the function code.
 	 */
	if ((meth->accflags & ACC_STATIC) == 0) {
		call.callsize[i] = PTR_TYPE_SIZE / SIZEOF_INT;
		s += call.callsize[i];
		call.calltype[i] = 'L';
		call.args[i].l = obj;
		i++;
	}

	for (j = 0; j < METHOD_NARGS(meth); i++, j++) {
		call.calltype[i] = *METHOD_ARG_TYPE(meth, j);
		switch (call.calltype[i]) {
		case 'I':
		case 'Z':
		case 'S':
		case 'B':
		case 'C':
			call.callsize[i] = 1;
			call.args[i].PROM_i = va_arg(args, jint);
			break;
		case 'F':
			call.callsize[i] = 1;
			call.args[i].PROM_f = (jfloat)va_arg(args, jdouble);
			if (PROMOTE_jfloat2jdouble) {
				call.calltype[i] = 'D';
			}
			break;
		case 'D':
			call.callsize[i] = 2;
			ENSURE_ALIGN64({});
			call.args[i].d = va_arg(args, jdouble);
			goto second_word;
		case 'J':
			call.callsize[i] = 2;
			ENSURE_ALIGN64({});
			call.args[i].j = va_arg(args, jlong);
		second_word:
			if(! NO_HOLES) {
				s += call.callsize[i];
				call.args[i+1].i = (&call.args[i].i)[1];
				i++;
				call.callsize[i] = 0;
				call.calltype[i] = 0;
			}
			break;
		case '[':
			call.calltype[i] = 'L';
			/* fall through */
		case 'L':
			call.callsize[i] = PTR_TYPE_SIZE / SIZEOF_INT;
			call.args[i].l = va_arg(args, jref);
			call.args[i].l = unveil(call.args[i].l);
			break;
		default:
			KAFFEVM_ABORT();
		}
		s += call.callsize[i];
	}

#if defined(STACK_LIMIT)
	call.calltype[i] = 'L';
	call.callsize[i] = PTR_TYPE_SIZE / SIZEOF_INT;
	call.args[i].l = KTHREAD(stacklimit)();
	s += PTR_TYPE_SIZE / SIZEOF_INT;
	i++;
#endif

	/* Return info */
	call.rettype = *METHOD_RET_TYPE(meth);
	switch (call.rettype) {
	case '[':
		call.rettype = 'L';
		/* fall through */
	case 'L':
		call.retsize = PTR_TYPE_SIZE / SIZEOF_INT;
		break;
	case 'V':
		call.retsize = 0;
		break;
	case 'D':
	case 'J':
		call.retsize = 2;
		break;
	default:
		call.retsize = 1;
		break;
	}

	/* Call info and arguments */
	call.nrargs = i;
	call.argsize = s;
	call.ret = ret;
	call.function = func;

	/* GCDIAG wipes free memory with 0xf4... */
	assert(call.function);
	assert(*(uint32*)(call.function) != 0xf4f4f4f4);

	/* Make the call - system dependent */
	engine_callMethod(&call);
}

int KaffeVM_countRealNumberOfArgs(parsed_signature_t *sig)
{
  unsigned int i = 0, size = 0;
  
  while (i < PSIG_NARGS(sig)) {
    char ch = *(PSIG_DATA(sig)+PSIG_ARG(sig, i));

    switch (ch) {
    case 'V':
      break;
    case 'I':
    case 'Z':
    case 'S':
    case 'B':
    case 'C':
    case 'F':
      size++;
      break;
    case 'D':
    case 'J':
#if defined(ALIGN_AT_64bits)
      if ((size & 1) != 0)
	size++;
#endif
      if (! NO_HOLES)
	size += 2;
      else
	size++;
      break;
    case '[':
    case 'L':
      size++;
      break;
    default:
      kprintf(stderr, "Invalid signature item %c in KaffeVM_countRealNumberOfArgs\n", ch);
      KAFFEVM_ABORT();
    }
    
    i++;
  }
  
  return size;
}
