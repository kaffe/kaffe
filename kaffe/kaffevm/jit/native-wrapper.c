/*
 * native-wrapper.c
 * Java Native Interface - JNI wrapper builder.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2004
 *      The kaffe.org's developers. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "jni_i.h"
#include "classMethod.h"
#include "native-wrapper.h"
#include "constpool.h"
#include "seq.h"
#include "slots.h"
#include "registers.h"
#include "labels.h"
#include "codeproto.h"
#include "basecode.h"
#include "icode.h"
#include "machine.h"
#include "gc.h"

#if defined(KAFFE_FEEDBACK)
#include "feedback.h"
#endif

static void*
startJNIcall(void)
{
	threadData 	*thread_data = THREAD_DATA();
	jnirefs* table;

	table = gc_malloc
	  (sizeof(jnirefs) + sizeof(jref)*DEFAULT_JNIREFS_NUMBER,
	   KGC_ALLOC_STATIC_THREADDATA);

	table->prev = thread_data->jnireferences;
	thread_data->jnireferences = table;
	table->frameSize = DEFAULT_JNIREFS_NUMBER;
	table->localFrames = 1;

	/* No pending exception when we enter JNI routine */
	thread_data->exceptObj = NULL;
	return( &thread_data->jniEnv ); 
}

static void
finishJNIcall(void)
{
	jref eobj;
	threadData	*thread_data = THREAD_DATA();
	jnirefs* table;
	int localFrames;

	table = thread_data->jnireferences;
	localFrames = table->localFrames;
	for (localFrames = table->localFrames; localFrames >= 1; localFrames--)
	  {
	    thread_data->jnireferences = table->prev;
	    gc_free(table);
	    table = thread_data->jnireferences;
	  }

	/* If we have a pending exception, throw it */
	eobj = thread_data->exceptObj;
	if (eobj != 0) {
		thread_data->exceptObj = NULL;
		throwExternalException(eobj);
	}
}


/*
 * Wrap up a native function in a calling wrapper, with JNI or KNI.
 */
static void
Kaffe_wrapper(Method* xmeth, void* func, bool use_JNI)
{
	errorInfo info;
	int count;
	nativeCodeInfo ncode;
	SlotInfo* tmp = NULL;
	SlotInfo* tmp2 = NULL;
	bool success = true;
	int j;
	int an;

	isStatic = METHOD_IS_STATIC(xmeth) ? 1 : 0;
	count = sizeofSigMethod(xmeth, false);
	count += 1 - isStatic;

#if defined(KAFFE_FEEDBACK)
	if( kaffe_feedback_file )
		lockMutex(kaffe_feedback_file);
#endif
	/* Construct a wrapper to call the JNI method with the correct
	 * arguments.
	 */
	enterTranslator();

#if defined(KAFFE_PROFILER)
	if (profFlag) {
		profiler_get_clicks(xmeth->jitClicks);
		xmeth->callsCount = 0;
		xmeth->totalClicks = 0;
		xmeth->totalChildrenClicks = 0;
	}
#endif
	globalMethod = xmeth;

	maxArgs = maxLocal = count; /* make sure args are spilled if needed */
	maxStack = 0;
#if defined(HAVE_FAKE_CALLS)
	initFakeCalls();
#endif
	success = initInsnSequence(xmeth, 0, maxLocal, maxStack, &info);
	if (!success) {
		goto done;
	}

	start_basic_block();
	prologue(xmeth);

	if (use_JNI) {
		/* Start a JNI call */
		slot_alloctmp(tmp);
		begin_func_sync();
		call_soft(startJNIcall);
		end_func_sync();
		return_ref(tmp);

#if 0
		{
			int j;
			int jcount;

			/* Make the necesary JNI ref calls first */
			if (!METHOD_IS_STATIC(xmeth)) {
				pusharg_ref(local(0), 0);
				end_sub_block();
				call_soft(KaffeJNI_addJNIref);
				popargs();
			}
			j = METHOD_NARGS(xmeth);
			jcount = count;
			while (j > 0) {
				j--;
				jcount -= sizeofSigChar(*METHOD_ARG_TYPE(xmeth, j), false);
				switch (*METHOD_ARG_TYPE(xmeth, j)) {
				case 'L':
				case '[':
					pusharg_ref(local(jcount), 0);
					end_sub_block();
					call_soft(KaffeJNI_addJNIref);
					popargs();
				}
			}
			start_sub_block();
		}
#endif
	}

	/* Add synchronisation if necessary */
	if (xmeth->accflags & ACC_SYNCHRONISED) {
		mon_enter(xmeth, local(0));
	}

#if defined(PUSHARG_FORWARDS)

	if (use_JNI) {
		/* Push the JNI info */
		pusharg_ref(tmp, 0);

		/* If static, push the class, else push the object */
		if (METHOD_IS_STATIC(xmeth)) {
			pusharg_ref_const(xmeth->class, 1);
			an = 0;
		}
		else {
			pusharg_ref(local(0), 1);
			an = 1;
		}
		count = 2;
	}
	else {
		/* If static, nothing, else push the object */
		if (!METHOD_IS_STATIC(xmeth)) {
			pusharg_ref(local(0), 0);
			an = 1;
		}
		else {
			an = 0;
		}
		count = an;
	}

	/* Push the specified arguments */
	for (j = 0; j < METHOD_NARGS(xmeth); j++) {
		switch (*METHOD_ARG_TYPE(xmeth, j)) {
		case 'L':
		case '[':
			pusharg_ref(local(an), count);
			break;
		case 'I':
		case 'Z':
		case 'S':
		case 'B':
		case 'C':
			pusharg_int(local(an), count);
			break;
		case 'F':
			pusharg_float(local(an), count);
			break;
		case 'J':
			pusharg_long(local(an), count);
			count += pusharg_long_idx_inc - 1;
			an++;
			break;
		case 'D':
			pusharg_double(local(an), count);
			count += pusharg_long_idx_inc - 1;
			an++;
			break;
		}
		count++;
		an++;
	}

#else
	/* TODO: Deal with 64bits where J and D use only one slot.  */
	/* Push the specified arguments */
	count = maxArgs;
	if (use_JNI) {
		count++;
		if (isStatic) {
			count++;
		}
	}
	an = maxArgs;

	for (j = METHOD_NARGS(xmeth); --j >= 0; ) {
		count--;
		an--;
		switch (*METHOD_ARG_TYPE(xmeth, j)) {
		case 'L':
		case '[':
			pusharg_ref(local(an), count);
			break;
		case 'I':
		case 'Z':
		case 'S':
		case 'B':
		case 'C':
			pusharg_int(local(an), count);
			break;
		case 'F':
			pusharg_float(local(an), count);
			break;
		case 'J':
			count--;
			an--;
			pusharg_long(local(an), count);
			break;
		case 'D':
			count--;
			an--;
			pusharg_double(local(an), count);
			break;
		default:
			break;
		}
	}

	if (use_JNI) {
		/* If static, push the class, else push the object */
		if (METHOD_IS_STATIC(xmeth)) {
			pusharg_ref_const(xmeth->class, 1);
		}
		else {
			pusharg_ref(local(0), 1);
		}

		/* Push the JNI info */
		pusharg_ref(tmp, 0);
	}
	else {
		/* If static, nothing, else push the object */
		if (!METHOD_IS_STATIC(xmeth)) {
			pusharg_ref(local(0), 0);
		}
	}

#endif

	/* Make the call */
	end_sub_block();
	call_soft(func);
	popargs();

	if (use_JNI) {
		slot_freetmp(tmp);
	}
	
	start_sub_block();

	/* Determine return type */
	switch (*METHOD_RET_TYPE(xmeth)) {
	case 'L':
	case '[':
		slot_alloctmp(tmp);
		return_ref(tmp);
		/* Remove synchronisation if necessary */
		if (xmeth->accflags & ACC_SYNCHRONISED) {
			mon_exit(xmeth, local(0));
		}
		if (use_JNI) {
		        slot_alloctmp(tmp2);
		  
#if SIZEOF_VOID_P == SIZEOF_INT
			and_int_const(tmp2, tmp, 1);
#elif SIZEOF_VOID_P == SIZEOF_LONG
			and_long_const(tmp2, tmp, 1);
#else
#error "Unsupported size of pointer"
#endif
			end_sub_block();
			begin_sync();
			cbranch_int_const_eq(tmp2, 0, reference_label(1, 1));
			end_sync();
			
			start_sub_block();
#if SIZEOF_VOID_P == SIZEOF_INT
			and_int_const(tmp2, tmp, ~(uintp)1);
#elif SIZEOF_VOID_P == SIZEOF_LONG
			and_long_const(tmp2, tmp, ~(uintp)1);
#else
#error "Unsupported size of pointer"
#endif
			load_ref(tmp, tmp2);
			slot_freetmp(tmp2);
			end_sub_block();
			set_label(1, 1);
			end_sub_block();
			call_soft(finishJNIcall);
			start_sub_block();
		}
		exit_method();
		returnarg_ref(tmp);
		break;
	case 'I':
	case 'Z':
	case 'S':
	case 'B':
	case 'C':
		slot_alloctmp(tmp);
		return_int(tmp);
		/* Remove synchronisation if necessary */
		if (xmeth->accflags & ACC_SYNCHRONISED) {
			mon_exit(xmeth, local(0));
		}
		if (use_JNI) {
			end_sub_block();
			call_soft(finishJNIcall);
			start_sub_block();
		}
		exit_method();
		returnarg_int(tmp);
		break;
	case 'F':
		slot_alloctmp(tmp);
		return_float(tmp);
		/* Remove synchronisation if necessary */
		if (xmeth->accflags & ACC_SYNCHRONISED) {
			mon_exit(xmeth, local(0));
		}
		if (use_JNI) {
			end_sub_block();
			call_soft(finishJNIcall);
			start_sub_block();
		}
		exit_method();
		returnarg_float(tmp);
		break;
	case 'J':
		slot_alloc2tmp(tmp);
		return_long(tmp);
		/* Remove synchronisation if necessary */
		if (xmeth->accflags & ACC_SYNCHRONISED) {
			mon_exit(xmeth, local(0));
		}
		if (use_JNI) {
			end_sub_block();
			call_soft(finishJNIcall);
			start_sub_block();
		}
		exit_method();
		returnarg_long(tmp);
		break;
	case 'D':
		slot_alloc2tmp(tmp);
		return_double(tmp);
		/* Remove synchronisation if necessary */
		if (xmeth->accflags & ACC_SYNCHRONISED) {
			mon_exit(xmeth, local(0));
		}
		if (use_JNI) {
			end_sub_block();
			call_soft(finishJNIcall);
			start_sub_block();
		}
		exit_method();
		returnarg_double(tmp);
		break;
	case 'V':
		/* Remove synchronisation if necessary */
		if (xmeth->accflags & ACC_SYNCHRONISED) {
			mon_exit(xmeth, local(0));
		}
		if (use_JNI)
			call_soft(finishJNIcall);
		exit_method();
		ret();
		break;
	default:
		break;
	}

	end_function();

	/* Generate the code */
	if (tmpslot > maxTemp) {
		maxTemp = tmpslot;
	}
	/* The codeinfo argument is only used in linkLabel, and it is 
	 * only needed if we have labels referring to bytecode.  This is
	 * not the case here.
	 */
	success = finishInsnSequence(NULL, &ncode, &info);
	if (!success) {
		goto done;
	}

	assert(xmeth->exception_table == 0);
	installMethodCode(NULL, xmeth, &ncode);

	if (use_JNI)
		xmeth->accflags |= ACC_JNI;

done:
	KJIT(resetConstants)();
	KJIT(resetLabels)();

#if defined(KAFFE_PROFILER)
	if (profFlag) {
		profiler_click_t end;

		profiler_get_clicks(end);
		xmeth->jitClicks = end - xmeth->jitClicks;
	}
#endif
	globalMethod = NULL;

	leaveTranslator();

#if defined(KAFFE_FEEDBACK)
	if( kaffe_feedback_file )
		unlockMutex(kaffe_feedback_file);
#endif
	if (!success) {
		throwError(&info);
	}
}


void
engine_create_wrapper (Method *meth, void *func)
{
	Kaffe_wrapper (meth, func, (meth->accflags&ACC_JNI)!=0);
}
