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

#if defined(INTERPRETER)

typedef struct _stackTrace {
	vmException* frame;
} stackTrace;

#define STACKTRACEINIT(S,I,O)	((S).frame = (vmException*)unhand((*Kaffe_ThreadInterface.currentJava)())->exceptPtr)
#define	STACKTRACESTEP(S)	((S).frame = (*Kaffe_ThreadInterface.nextFrame)((S).frame))
#define STACKTRACEPC(S)		((S).frame->pc)
#define STACKTRACEMETH(S)	((S).frame->meth)
#define STACKTRACEEND(S)	((S).frame == 0)

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
#define	STACKTRACESTEP(S)	((S).frame = (*Kaffe_ThreadInterface.nextFrame)((S).frame))
#define STACKTRACEPC(S)		(PCFRAME((S).frame))
#define	STACKTRACEMETH(S)	(findMethodFromPC(PCFRAME((S).frame)))
#define	STACKTRACEEND(S)	((S).frame == 0)

#endif

Hjava_lang_Object*	buildStackTrace(struct _exceptionFrame* base);
Hjava_lang_Object*	getClassContext(void* bulk);
Hjava_lang_Class*	getClassWithLoader(int* depth);
jint			classDepth(char* name);

#endif
