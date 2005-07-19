/*
 * stats.c
 * Getrusage based execution timing and other statistics gathering.
 *
 * Copyright (c) 1999
 *	Archie Cobbs <archie@whistle.com>
 *
 * Copyright (c) 2004
 *      Kaffe.org contributors. See ChangeLog for details. All rights reserved.
 *   
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "jni_md.h"
#if !defined(KAFFEH)
#include "jthread.h"
#endif
#include "support.h"
#include "debug.h"
#include "stats.h"

#if defined(KAFFE_STATS)

timespent fulljit;
counter jitmem;
counter jitcodeblock;
counter utf8new;
counter utf8newalloc;
counter utf8release;
counter ltmem;
counter jarmem;
counter cpemem;
timespent jit_time;

/*
 * The format for statMask (set by statsSetMaskStr) is like so:
 *	mask = element (',' element)*
 * 	element = groupname | itemname
 * 	itemname = groupname '-' item
 *
 * For example: "gc,jitmem-pusharg" will print all counters/timers belonging
 * to group "gc", and the "pusharg" counter in group "jitmem".
 */
static char *statMask = "none";

static
int cmp_int(const void *_a, const void *_b)
{
        int a = *(int*)_a;
        int b = *(int*)_b;
        return (a < b ? -1 : (a == b) ? 0 : 1);
}

void
statsComputeSumMedian(int *v, int n, int *sum, int *med)
{
        int i = 0;
        qsort(v, n, sizeof(int), cmp_int);
        *sum = 0;
        while (i < n) {
                *sum += v[i++];
        }
        *med = v[n/2];
}

static timespent *counters;	/* all timers that have been run */

/*
 * determine whether counter with a given name should be reported
 * under the current statMask
 */
static int
reportCounter(timespent *counter, int type)
{
	char *g, buf[256];

	if (!(counter->flags & type)) {
		return (0);
	}

	if (!strcmp(statMask, "all")) {
		return (1);
	}

	if (strstr(statMask, counter->name)) {
		return (1);
	}

	/* find group */
	strncpy(buf, counter->name, sizeof(buf));
	buf[sizeof(buf) - 1] = '\0';
	g = strchr(buf, '-');
	if (g != 0) {
		g[0] = '\0';
		if (strstr(statMask, buf)) {
			return (1);
		}
	}
	return (0);
}

/* Print total user time, user time spent by each defined counter, along
 * with the number of passes each counter took.
 */
static void
statsReport(void)
{
	timespent *p;
	struct rusage tot_usage;
	int ntimers = 0, ncounters = 0, ncumcounters = 0, nusercounters = 0;

	getrusage(RUSAGE_SELF, &tot_usage);

	for (p = counters; p; p = p->next) {
		if (reportCounter(p, STAT_TIMING)) {
			ntimers++;
		}
		if (reportCounter(p, STAT_COUNT)) {
			ncounters++;
		}
		if (reportCounter(p, STAT_CUMULATE)) {
			ncumcounters++;
		}
		if (reportCounter(p, STAT_USER)) {
			nusercounters++;
		}
	}
	if (ntimers + ncounters + ncumcounters + nusercounters == 0) {
		return;	/* user doesn't want to see any counters */
	}

	if (ntimers > 0) {
		dprintf("#FACILITY\tUSER TIME\tSYSTEM TIME\tCALLS\n"
			"#--------------- --------------- "
			"--------------- ---------------\n");
	}
	for (p = counters; p; p = p->next) {
		if (reportCounter(p, STAT_TIMING)) {
			dprintf("%-15s %8d.%06d %8d.%06d %15d\n",
				p->name, p->total.tv_sec, p->total.tv_usec,
				p->stotal.tv_sec, p->stotal.tv_usec,
				p->calls);
		}
	}
	if (ntimers > 0) {
		dprintf("%-15s %8d.%06d %8d.%06d\n",
			"TOTAL",
			tot_usage.ru_utime.tv_sec, tot_usage.ru_utime.tv_usec,
			tot_usage.ru_stime.tv_sec, tot_usage.ru_stime.tv_usec);
	}

	if (ncounters > 0) {
		dprintf("#\n#HIT COUNTERS\n");
		dprintf("%-30s %8s\n", "#FACILITY", "HITS");
		dprintf("%-30s %8s\n", 
			"#-----------------------------", "--------");
	}
	for (p = counters; p; p = p->next) {
		if (reportCounter(p, STAT_COUNT)) {
			dprintf("%-30s %8d\n",
				p->name, p->calls);
		}
	}

	if (ncumcounters > 0) {
		dprintf("#\n#CUMULATIVE COUNTERS\n");
		dprintf("%-25s %-11s %-14s %-14s %-11s\n",
			"#FACILITY", "VISITS", "FINAL", "MAX", "AVG PER HIT");
		dprintf("%-25s %-11s %14s %14s %11s\n",
			"#------------------------", "-----------", 
			"--------------", "--------------", 
			"-----------");
	}
	for (p = counters; p; p = p->next) {
		if (reportCounter(p, STAT_CUMULATE)) {
			dprintf("%-25s %11d %14qd %14qd %11.2f\n",
				p->name, p->calls, p->cumtotal, p->max,
				p->cumtotal/(double)p->calls);
		}
	}

	if (nusercounters > 0) {
		dprintf("#\n#USER-DEFINED COUNTERS\n");
	}
	for (p = counters; p; p = p->next) {
		if (reportCounter(p, STAT_USER)) {
			dprintf("#<------ BEGIN `%s' ------>\n", p->name);
			p->userfunc();
			dprintf("#<------ END `%s' ------>\n", p->name);
		}
	}
}

static void
registerCounter(timespent *counter, char *name, int type)
{
	if (!counter->name) {
		counter->name = name;
		counter->next = counters;
		counter->flags = type;
		counters = counter;
	}
}

/* Start timing some portion of JVM activity.
 * If we are called for the first time, install the exit function
 * to print timings.  If we are called for the first time on a counter,
 * initialize it and add it to the list of all counters.
 * Caller is responsible for synchronization.
 */
void
startTiming(timespent *counter, char *name)
{
	struct rusage ru;
	
	if (!counter->name) {
		registerCounter(counter, name, STAT_TIMING);
	}

	counter->calls++;
	getrusage(RUSAGE_SELF, &ru);
	counter->current = ru.ru_utime;
	counter->scurrent = ru.ru_stime;
}

void 
registerUserCounter(timespent *counter, char *name,
        void (*userfunc)(void))
{
	registerCounter(counter, name, STAT_USER);
	counter->userfunc = userfunc;
}

void 
hitCounter(timespent *counter, char *name)
{
	if (!counter->name) {
		registerCounter(counter, name, STAT_COUNT);
	}
	counter->calls++;
}

void 
addToCounter(timespent *counter, char *name, int n, jlong increment)
{
	if (!counter->name) {
		registerCounter(counter, name, STAT_CUMULATE);
	}
	counter->calls += n;
	counter->cumtotal += increment;
	if (counter->max < counter->cumtotal) {
		counter->max = counter->cumtotal;
	}
}

/* timeval arithmetic macros */
#define DECF(D, S)				\
{						\
	(D).tv_usec -= (S).tv_usec;		\
	if ((D).tv_usec < 0) {			\
		(D).tv_usec += 1000000;		\
		(D).tv_sec -= 1;		\
	}					\
	(D).tv_sec -= (S).tv_sec;		\
}

#define INCF(D,S)				\
{						\
	(D).tv_usec += (S).tv_usec;		\
	if ((D).tv_usec > 1000000) {		\
		(D).tv_usec -= 1000000;		\
		(D).tv_sec += 1;		\
	}					\
	(D).tv_sec += (S).tv_sec;		\
}

/*
 * End a timing run.  Adjust total time for this counter.
 */
void
stopTiming(timespent *counter)
{
	struct rusage ru;

	getrusage(RUSAGE_SELF, &ru);
	DECF(ru.ru_utime, counter->current);
	INCF(counter->total, ru.ru_utime);
	DECF(ru.ru_stime, counter->scurrent);
	INCF(counter->stotal, ru.ru_stime);
}

void 
statsSetMaskStr(char *mask)
{
	static int once = 0;

	if (!once) atexit(statsReport);
	once = 1;
	statMask = mask;
}

#endif /* KAFFE_STATS */
