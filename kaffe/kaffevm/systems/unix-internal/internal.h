/*
 * thread-internal.h
 * Thread support using internal system.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __thread_internal_h
#define __thread_internal_h

#include "config-setjmp.h"

struct Hjava_lang_Thread;

typedef struct _ctx {
	struct Hjava_lang_Thread*	nextlive;
	uint8				status;
	uint8				priority;
	uint8*				restorePoint;
	uint8*				stackBase;
	uint8*				stackEnd;
	jlong				time;
	struct Hjava_lang_Thread*	nextQ;
	struct Hjava_lang_Thread*	nextalarm;
	struct Hjava_lang_Thread**	blockqueue;
	uint8				flags;
	void				(*func)(void*);
	jmp_buf				env;
	/* for alignment (Gcc extension) */
	double				align[0];
} ctx;

int	blockOnFile(int, int);
void	reschedule(void);
void	freeThreadCtx(int);

#define GET_SP(E)	(((void**)(E))[SP_OFFSET])
#define SET_SP(E, V)	((void**)(E))[SP_OFFSET] = (V)

/* Flags used for threading I/O calls */
#define	TH_READ				0
#define	TH_WRITE			1
#define	TH_ACCEPT			TH_READ
#define	TH_CONNECT			TH_WRITE

#define THREAD_SUSPENDED		0
#define THREAD_RUNNING			1
#define THREAD_DEAD			2

#define	THREAD_FLAGS_GENERAL		0
#define	THREAD_FLAGS_NOSTACKALLOC	1
#define	THREAD_FLAGS_KILLED		2
#define	THREAD_FLAGS_ALARM		4
#define	THREAD_FLAGS_USERSUSPEND	8
#define	THREAD_FLAGS_ERROR		16
#define	THREAD_FLAGS_UNCAUGHTEXCEPTION	32

#endif
