/*
 * xprofiler.c
 * Interface functions to the profiling code
 *
 * Copyright (c) 2000, 2001 University of Utah and the Flux Group.
 * All rights reserved.
 *
 * This file is licensed under the terms of the GNU Public License.
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * Contributed by the Flux Research Group, Department of Computer Science,
 * University of Utah, http://www.cs.utah.edu/flux/
 */

#include "config.h"

#if defined(KAFFE_XPROFILER)

#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/gmon.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/gmon.h>
#include <sys/sysctl.h>

#include "gtypes.h"
#include "md.h"
#include "xprofile-md.h"
#include "exception.h"
#include "kaffe/jmalloc.h"
#include "methodCache.h"

#include "memorySamples.h"
#include "feedback.h"
#include "debugFile.h"
#include "fileSections.h"
#include "gmonFile.h"
#include "callGraph.h"
#include "xprofiler.h"

/* Variables for figuring out the start and end of the program text */
extern char _start;
extern char etext;

int xProfFlag = 0;
/* Flag used to determine whether or not we should record `hits' */
static volatile int xProfRecord = 1;

/*
 * This variable is part of a nasty hack to keep the call graph accounting
 * code from being credited with time.  The accounting function should set
 * this variable while its working so that if a profiling interrupt happens
 * while its executing the appropriate function will be credited.
 */
static char * volatile profiler_sample_override_pc = NULL;
volatile int profiler_sample_overrides = 0;

/* Structures used to hold the profiling information */
struct memory_samples *kaffe_memory_samples = NULL;
struct call_graph *kaffe_call_graph = NULL;

/* The name of the output files */
const char *kaffe_gmon_filename = "xgmon.out";
const char *kaffe_syms_filename = "kaffe-jit-symbols.s";

/* Debugging file for profiler symbols */
struct debug_file *profiler_debug_file = NULL;
static int extraProfileCount = 0;

/* Number of call_arc structures to preallocate */
#define XPROFILE_ARCS (1024 * 64)

/*
 * The gutter threshold is used to determine if the gap between two chunks of
 * observed memory is too large to put into the file.  Since the resulting gap
 * would just be a bunch of zeros, we split them into different files.
 */
#define SAMPLE_GUTTER_THRESHOLD 1024 * 1024 * 5

/* Structure used to track the current gmon file for our walker */
struct profiler_gmon_file {
	char *pgf_stage;		/* The name of the current stage */
	struct gmon_file *pgf_file;	/* The active gmon file structure */
	long pgf_record;		/* The index of the hist record */
};

/*
 * A walker function for walkMemorySamples.  This does a little more than
 * the one provided by the gmon code since it will create a new gmon file
 * if theres a large gap in observed memory.
 */
static int profilerSampleWalker(void *handle, char *addr,
				short *bins, size_t size)
{
	struct profiler_gmon_file *pgf = handle;
	struct gmon_file *gf = pgf->pgf_file;
	int retval = 0;

	/* Check if we need to dump these samples in another file */
	if( (addr - gf->gf_addr) > SAMPLE_GUTTER_THRESHOLD )
	{
		char *filename, *old_high;
		size_t len;

		old_high = gf->gf_high;
		/* Rewrite the old record with the new high address */
		writeGmonRecord(gf,
				GRA_Rewrite, pgf->pgf_record,
				GRA_Type, GMON_TAG_TIME_HIST,
				GRA_LowPC, gf->gf_low,
				GRA_HighPC, gf->gf_addr,
				GRA_DONE);
		writeCallGraph(kaffe_call_graph, gf);
		/* Close down the file */
		deleteGmonFile(gf);
		pgf->pgf_file = 0;
		gf = 0;

		/*
		 * Make up the filename for the new file, we'll just use the
		 * starting address for this range to make it unique
		 */
		len = strlen(kaffe_gmon_filename) +
			1 + /* `.' */
			2 + (sizeof(void *) * 2) + /* `0x...' */
			(pgf->pgf_stage ? strlen(pgf->pgf_stage) + 1 : 0) +
			1;
		if( (filename = (char *)KMALLOC(len)) )
		{
			/* Construct the new file name */
			sprintf(filename,
				"%s.%p%s%s",
				kaffe_gmon_filename,
				addr,
				pgf->pgf_stage ? "." : "",
				pgf->pgf_stage ? pgf->pgf_stage : "");
			if( (pgf->pgf_file = createGmonFile(filename)) )
			{
				gf = pgf->pgf_file;
				/* Write out another hist record */
				pgf->pgf_record = writeGmonRecord(
					gf,
					GRA_Type, GMON_TAG_TIME_HIST,
					GRA_LowPC, addr,
					GRA_HighPC, old_high,
					GRA_DONE);
			}
		}
	}
	if( gf )
	{
		/* Let the gmon walker do its thing */
		retval = gmonSampleWalker(gf, addr, bins, size);
	}
	memset(bins, 0, size * sizeof(short));
	return( retval );
}

/*
 * Handles any post processing
 */
static void profilerAtExit(void)
{
	if( !xProfFlag )
		return;
	/* We don't care about profiling anymore */
	disableProfileTimer();
	/* This is the final stage, write out any data */
	xProfileStage(0);
	/* Release everything else */
	disableXProfiling();
}

int enableXCallGraph(void)
{
	int retval = false;

	if( (kaffe_call_graph = createCallGraph(XPROFILE_ARCS)) )
	{
		retval = true;
	}
	return( retval );
}

#if defined(KAFFE_CPROFILER)
/*
 * Return the minimum cycles per second for the machine
 */
static int hertz(void)
{
	struct itimerval value, ovalue;
	int retval = 0;
	
	timerclear(&value.it_interval);
	timerclear(&value.it_value);
	timerclear(&ovalue.it_interval);
	timerclear(&ovalue.it_value);
	value.it_interval.tv_usec = 1;
	setitimer(ITIMER_REAL, &value, 0);
	getitimer(ITIMER_REAL, &ovalue);
	timerclear(&value.it_interval);
	timerclear(&value.it_value);
	setitimer(ITIMER_REAL, &value, 0);
	if( ovalue.it_interval.tv_usec >= 2 )
		retval = 1000000 / ovalue.it_interval.tv_usec;
	return( retval );
}

#if defined(KAFFE_STD_PROF_RATE)
KAFFE_STD_PROF_RATE
#endif
#endif

int enableXProfiling(void)
{
	int retval = false;

	xProfilingOff();
	/* Start up our profiler and set it to observe the main executable */
	if( xProfFlag &&
	    enableProfileTimer() &&
	    (kaffe_memory_samples = createMemorySamples()) &&
	    observeMemory(kaffe_memory_samples, &_start, &etext - &_start) &&
	    (profiler_debug_file = createDebugFile(kaffe_syms_filename)) &&
	    !atexit(profilerAtExit) )
	{
#if defined(KAFFE_CPROFILER)
		struct gmonparam *gp = getGmonParam();
		int prof_rate;
#endif
		
#if defined(KAFFE_CPROFILER)
		/* Turn off any other profiling */
		profil(0, 0, 0, 0);
#if defined(KAFFE_STD_PROF_RATE)
		if( !(prof_rate = kaffeStdProfRate()) )
#endif
			prof_rate = hertz(); /* Just guess */
		if( !prof_rate )
			prof_rate = 100;
		/* Copy the hits leading up to now into our own counters */
		if( gp && gp->kcountsize > 0 )
		{
			int lpc, len = gp->kcountsize / sizeof(HISTCOUNTER);
			HISTCOUNTER *hist = gp->kcount;
			int scale;

			scale = (gp->highpc - gp->lowpc) / len;
			for( lpc = 0; lpc < len; lpc++ )
			{
				if( hist[lpc] )
				{
					char *pc = ((char *)gp->lowpc) +
						(lpc * scale);

					memoryHitCount(kaffe_memory_samples,
						       pc,
						       (100 * hist[lpc]) /
						       prof_rate);
				}
			}
		}
#endif
		retval = true;
	}
	else
	{
		xProfFlag = 0;
		disableXProfiling();
	}
	xProfilingOn();
	return( retval );
}

void disableXProfiling(void)
{
	/* Shutoff the timer and delete any structures */
	disableProfileTimer();
	xProfilingOff();
	deleteMemorySamples(kaffe_memory_samples);
	kaffe_memory_samples = 0;
	deleteCallGraph(kaffe_call_graph);
	kaffe_call_graph = 0;
	deleteDebugFile(profiler_debug_file);
	profiler_debug_file = 0;
}

void xProfilingOn(void)
{
	xProfRecord++;
#if defined(KAFFE_CPROFILER)
	{
		struct gmonparam *gp = getGmonParam();
		
		if( xProfRecord && gp )
			gp->state = GMON_PROF_ON;
	}
#endif
}

void xProfilingOff(void)
{
	xProfRecord--;
#if defined(KAFFE_CPROFILER)
	{
		struct gmonparam *gp = getGmonParam();
		
		if( !xProfRecord && gp )
			gp->state = GMON_PROF_OFF;
	}
#endif
}

void xProfileStage(char *stage_name)
{
	char *low, *high, *filename;
	struct gmon_file *gf;
	size_t len;

	if( !xProfFlag )
		return;
	xProfilingOff();
	low = kaffe_memory_samples->ms_low;
	high = kaffe_memory_samples->ms_high +
		(extraProfileCount * HISTFRACTION);
	KTHREAD(suspendall)();
	len = strlen(kaffe_gmon_filename) +
		(stage_name ? strlen(stage_name) + 1 : 0) +
		1;
	if( (filename = (char *)KMALLOC(len)) )
	{
		sprintf(filename,
			"%s%s%s",
			kaffe_gmon_filename,
			stage_name ? "." : "",
			stage_name ? stage_name : "");
		/* Create and write out the gmon file */
		if( (gf = createGmonFile(filename)) )
		{
			struct profiler_gmon_file pgf;
			
			/* Initialize the profiler_gmon_files state */
			pgf.pgf_stage = stage_name;
			pgf.pgf_file = gf;
			/* Write out the samples */
			pgf.pgf_record =
				writeGmonRecord(gf,
						GRA_Type, GMON_TAG_TIME_HIST,
						GRA_LowPC, low,
						GRA_HighPC, high,
						GRA_DONE);
			/* The low/high might be aligned by writeGmonRecord */
			low = gf->gf_low;
			high = gf->gf_high;
			walkMemorySamples(kaffe_memory_samples,
					  low,
					  high -
					  (extraProfileCount * HISTFRACTION),
					  &pgf,
					  profilerSampleWalker);
			if( pgf.pgf_file )
			{
				/* Dump out the call graph data */
				writeCallGraph(kaffe_call_graph, pgf.pgf_file);
				deleteGmonFile(pgf.pgf_file);
			}
			else
			{
				resetMemorySamples(kaffe_memory_samples);
			}
		}
		else
		{
			fprintf(stderr,
				"XProf Notice: Cannot create gmon file %s\n",
				filename);
			KFREE(filename);
			resetMemorySamples(kaffe_memory_samples);
		}
	}
	else
	{
		fprintf(stderr,
			"XProf Notice: Not enough memory to write "
			"profiling data\n");
		resetMemorySamples(kaffe_memory_samples);
	}
	resetCallGraph(kaffe_call_graph);
	xProfilingOn();
	KTHREAD(unsuspendall)();
}

int profileGmonFile(char *name)
{
	/* Just set the file for now, we'll make it at exit */
	kaffe_gmon_filename = name;
	return( true );
}

int profileSymbolFile(char *name)
{
	kaffe_syms_filename = name;
	return( true );
}

#if defined(KAFFE_XPROFILER)

struct sigaction oldSigAction;

static void profileTimerHandler(SIGNAL_ARGS(sig UNUSED, sc))
{
	SIGNAL_CONTEXT_POINTER(scp) = GET_SIGNAL_CONTEXT_POINTER(sc);
	char *addr = (char *)SIGNAL_PC(scp);

	if( kaffe_memory_samples && xProfRecord )
	{
		if( profiler_sample_override_pc )
		{
			profiler_sample_overrides++;
			/* Use the override address */
			addr = profiler_sample_override_pc;
			/* Null it out to stop false positives */
			profiler_sample_override_pc = 0;
		}
		memoryHit(kaffe_memory_samples, addr);
	}
}

int enableProfileTimer(void)
{
	struct sigaction sa;
	int retval = false;

	/* Setup our signal handler */
	sa.sa_handler = (SIG_T)profileTimerHandler;
	sigfillset(&sa.sa_mask);
	sa.sa_flags = 0;
#if 0
	if( sigaction(SIGALRM, &sa, &oldSigAction) >= 0 )
	{
		struct itimerval new_value;
		
		/* Setup a 10ms timer */
		new_value.it_interval.tv_sec = 0;
		new_value.it_interval.tv_usec = 10000;
		new_value.it_value.tv_sec = 0;
		new_value.it_value.tv_usec = 10000;
		if( setitimer(ITIMER_REAL, &new_value, 0) >= 0 )
		{
			retval = true;
		}
	}
#endif
	retval = 1;
	return( retval );
}

void disableProfileTimer(void)
{
	struct itimerval disable_value;

	timerclear(&disable_value.it_interval);
	timerclear(&disable_value.it_value);
	setitimer(ITIMER_REAL, &disable_value, 0);
}
#else
int enableProfileTimer(void)
{
	return( 1 );
}

void disableProfileTimer(void)
{
}
#endif

/* The rest of these are just wrappers for other code */

int profileFunction(struct mangled_method *mm, char *code, int codelen)
{
	int retval = false;

	if( xProfFlag && kaffe_memory_samples )
	{
		/*
		 * Add the function name to the symbol file and observe the
		 * memory
		 */
		xProfilingOff();
		if( observeMemory(kaffe_memory_samples, code, codelen) &&
		    addDebugInfo(profiler_debug_file,
				 DIA_FunctionSymbol, mm, code, codelen,
				 DIA_DONE) )
		{
			retval = true;
		}
		xProfilingOn();
	}
	else
	{
		retval = true;
	}
	return( retval );
}

int profileMemory(char *code, int codelen)
{
	int retval = false;
	
	if( xProfFlag )
	{
		xProfilingOff();
		if( observeMemory(kaffe_memory_samples, code, codelen) )
		{
			retval = true;
		}
		xProfilingOn();
	}
	else
	{
		retval = true;
	}
	return( retval );
}

int profileSymbol(struct mangled_method *mm, char *addr, int size)
{
	int retval = false;
	
	if( xProfFlag )
	{
		if( addDebugInfo(profiler_debug_file,
				 DIA_FunctionSymbol, mm, addr, size,
				 DIA_DONE) )
		{
			retval = true;
		}
	}
	else
	{
		retval = true;
	}
	return( retval );
}

void profileArcHit(char *frompc, char *selfpc)
{
	char *old_override = profiler_sample_override_pc;
	
	/*
	 * profiler_sample_override_pc is a nasty hack to keep the
	 * accounting function from being counted as a hit when it
	 * should really be the caller.
	 */
	profiler_sample_override_pc = selfpc;
	if( kaffe_call_graph && xProfRecord )
	{
		arcHit(kaffe_call_graph, frompc, selfpc);
	}
	profiler_sample_override_pc = old_override;
}

#if defined(KAFFE_XPROFILER) && defined(KAFFE_CPROFILER) && defined(_KAFFE_OVERRIDE_MCOUNT_DEF)
_KAFFE_OVERRIDE_MCOUNT_DEF;
_KAFFE_OVERRIDE_MCOUNT_DEF
{
	char *old_override = profiler_sample_override_pc;
	
	/*
	 * profiler_sample_override_pc is a nasty hack to keep the
	 * accounting function from being counted as a hit when it
	 * should really be the caller.
	 */
	profiler_sample_override_pc = (char *)selfpc;
	if( kaffe_call_graph )
	{
		register struct gmonparam *gp = getGmonParam();
		
		if( !xProfRecord || (gp && (gp->state != GMON_PROF_ON)) )
			goto fini;
		arcHit(kaffe_call_graph, (char *)frompc, (char *)selfpc);
	}
	else
	{
		register u_short *frompcindex;
		register struct tostruct *top, *prevtop;
		register struct gmonparam *p;
		register long toindex;

		p = getGmonParam();
		/*
		 * check that we are profiling
		 * and that we aren't recursively invoked.
		 */
		if( !p || (p->state != GMON_PROF_ON) )
			goto fini;
		p->state = GMON_PROF_BUSY;
		
		/*
		 * check that frompcindex is a reasonable pc value.
		 * for example:	signal catchers get called from the stack,
		 *		not from text space.  too bad.
		 */
		frompc -= p->lowpc;
		if (frompc > p->textsize)
			goto done;
		
		frompcindex = &(p->froms[frompc /
				       (p->hashfraction * sizeof(*p->froms))]);
		toindex = *frompcindex;
		if (toindex == 0) {
			/*
			 *	first time traversing this arc
			 */
			toindex = ++p->tos[0].link;
			if (toindex >= p->tolimit)
				/* halt further profiling */
				goto overflow;
			
			*frompcindex = toindex;
			top = &p->tos[toindex];
			top->selfpc = selfpc;
			top->count = 1;
			top->link = 0;
			goto done;
		}
		top = &p->tos[toindex];
		if (top->selfpc == selfpc) {
			/*
			 * arc at front of chain; usual case.
			 */
			top->count++;
			goto done;
		}
		/*
		 * have to go looking down chain for it.
		 * top points to what we are looking at,
		 * prevtop points to previous top.
		 * we know it is not at the head of the chain.
		 */
		for (; /* goto done */; ) {
			if (top->link == 0) {
				/*
				 * top is end of the chain and none of the
				 * chain had top->selfpc == selfpc.  so we
				 * allocate a new tostruct and link it to the
				 * head of the chain.
				 */
				toindex = ++p->tos[0].link;
				if (toindex >= p->tolimit)
					goto overflow;
				
				top = &p->tos[toindex];
				top->selfpc = selfpc;
				top->count = 1;
				top->link = *frompcindex;
				*frompcindex = toindex;
				goto done;
			}
			/*
			 * otherwise, check the next arc on the chain.
			 */
			prevtop = top;
			top = &p->tos[top->link];
			if (top->selfpc == selfpc) {
				/*
				 * there it is.
				 * increment its count
				 * move it to the head of the chain.
				 */
				top->count++;
				toindex = prevtop->link;
				prevtop->link = top->link;
				top->link = *frompcindex;
				*frompcindex = toindex;
				goto done;
			}
			
		}
	done:
		p->state = GMON_PROF_ON;
		goto fini;
	overflow:
		p->state = GMON_PROF_ERROR;
		goto fini;
	}
 fini:
	profiler_sample_override_pc = old_override;
}

_KAFFE_OVERRIDE_MCOUNT
#endif

void profileHit(char *addr)
{
	if( kaffe_memory_samples && xProfRecord )
	{
		if( profiler_sample_override_pc )
		{
			profiler_sample_overrides++;
			/* Use the override address */
			addr = profiler_sample_override_pc;
			/* Null it out to stop false positives */
			profiler_sample_override_pc = 0;
		}
		memoryHit(kaffe_memory_samples, addr);
	}
}

#endif /* KAFFE_XPROFILER */
