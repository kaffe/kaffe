/*
 * stackTrace.h
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __stacktrace_h
#define __stacktrace_h

#include "exception.h"

/*
 * STACKTRACEMETHCREATE is a pointer to a method object that is stored when 
 * the stack trace is built.  This value is easily available in the interpreter.
 *
 * In the jitter, however, it is found using findMethodFromPC, which is
 * expensive.  That is why there is STACKTRACEMETHPRINT, which will compute
 * the method ptr corresponding to a given pc if and only when the stack trace
 * is in fact printed.  This should speed up constructing stack traces.
 */
#if defined(INTERPRETER)

typedef struct _stackTrace {
	vmException* frame;
} stackTrace;

#define STACKTRACEINIT(S,I,O)	((S).frame = (vmException*)unhand(getCurrentThread())->exceptPtr)
#define	STACKTRACESTEP(S)	((S).frame = nextFrame((S).frame))
#define STACKTRACEPC(S)		((S).frame->pc)
#define STACKTRACEMETHCREATE(S)	((S).frame->meth)
#define STACKTRACEEND(S)	((S).frame == 0 || (S).frame->meth == (Method*)1)

#elif defined(TRANSLATOR)

typedef struct _stackTrace {
	struct _exceptionFrame	nframe;
	struct _exceptionFrame* frame;
} stackTrace;

#define STACKTRACEINIT(S, I, O)				\
	{						\
		if ((I) == NULL) {			\
			FIRSTFRAME((S).nframe, O);	\
			(S).frame = &((S).nframe);	\
		} else {				\
			(S).frame = (I);		\
		}					\
	}
#define	STACKTRACESTEP(S)	((S).frame = nextFrame((S).frame))
#define STACKTRACEPC(S)		(PCFRAME((S).frame))
#define	STACKTRACEMETHCREATE(S)	(0)
#define	STACKTRACEEND(S)	((S).frame == 0)

#endif

struct _methods;

typedef struct _stackTraceInfo {
	uintp   pc;
	struct _methods* meth;
} stackTraceInfo;

#define ENDOFSTACK	((struct _methods*)-1)

Hjava_lang_Object*	buildStackTrace(struct _exceptionFrame*);
Method*			stacktraceFindMethod(stackTraceInfo *);

#endif
