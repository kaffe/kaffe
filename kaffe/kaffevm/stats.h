/*
 * stats.h
 *
 * Copyright (c) 1999
 *	Archie Cobbs <archie@whistle.com>
 *
 * Copyright (c) 2004
 *	Kaffe.org contributors. See ChangeLog for details. All rights reserved.
 *   
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __kaffevm_stats_h
#define __kaffevm_stats_h

/*
 * Structures and prototypes for getrusage based execution timing
 * and other statistics gathering.
 * 
 * We provide timers, hit counters, cumulative counters, and the possibility
 * for user-defined statistics routines to be invoked at exit 
 */
#if defined(KAFFE_STATS) && !defined(KAFFEH)

/* various flags */
#define	STAT_TIMING	1
#define STAT_COUNT	2
#define STAT_CUMULATE	4
#define STAT_MINMAX	8
#define STAT_USER	16

/*
 * We use the same struct for all kinds of timers and counters
 * We a bunch of aliases to refer to it
 */
typedef struct _statobject timespent;
typedef struct _statobject counter;
typedef struct _statobject statobject;

/* This struct is used by all kinds of counters/timers */
struct _statobject {
	char *name;
	int flags;	/* describes kind of counter/timer, see STAT_ flags */
	struct _statobject *next;
	int calls;
	struct timeval total;
	struct timeval stotal;
	struct timeval current;
	struct timeval scurrent;
	jlong cumtotal;
	jlong max;
	void	(*userfunc)(void);
};

extern void registerUserCounter(counter *counter, char *name, 
	void (*userfunc)(void));
extern void hitCounter(counter *counter, char *name);
extern void addToCounter(counter *counter, char *name, int n, jlong inc);
extern void startTiming(timespent *counter, char *name);
extern void stopTiming(timespent *counter);
#else
/* We either can't or wont perform timing:  The first macro suppresses
   unused variable warnings. */
/* NB: you can avoid getting these definitions by enclosing
 * the timer invocations in #ifdef KAFFE_STATS
 */
typedef char timespent;
typedef char statobject;
typedef char counter;

#define startTiming(C,N)
#define hitCounter(C,N) 
#define addToCounter(C,N,I0,I1)
#define stopTiming(C)
#define registerUserCounter(C,N,F)
#endif

/*
 * Declarations for actual counters.
 */
extern counter jitmem;
extern counter jitcodeblock;
extern timespent jit_time;
extern counter utf8new;
extern counter utf8newalloc;
extern counter utf8release;
extern counter ltmem;
extern counter jarmem;
extern counter cpemem;
extern counter fulljit;

extern void statsComputeSumMedian(int *v, int n, int *sum, int *med);
extern void statsSetMaskStr(char *);
#define GCSIZEOF(x)	KGC_getObjectSize(main_collector, (x))

#endif /* __kaffevm_stats_h */

