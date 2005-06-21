/*
 * Copyright (c) 1998, 1999, 2000, 2001
 *     The University of Utah. All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Contributed by the Flux Research Group at the University of Utah.
 * Authors: Godmar Back, Patrick Tullmann, Edouard G. Parmelan
 */
/*
 * debug.c
 *
 * A dynamic debugging framework for Kaffe.  Through the magic
 * of hideous macros, we can control the debugging output of
 * Kaffe in a couple of ways.  First, it can be completely
 * disabled in which case all of the code "just goes away",
 * or, if its enabled, the area of code to debug is dynamically
 * chosen at run time.
 */


#if defined(HAVE_STDARG_H)
#include <stdarg.h>
#endif /* defined(HAVE_STDARG_H) */

#include <stdio.h>

#include "lerrno.h"
#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "config-io.h"
#include "config-signal.h"
#include "jni_md.h"
#include "gtypes.h"
#include "gc.h"
#include "debug.h"
#include "jsyscall.h"

/* Default debugging mask to use (if debug is enabled) */
#define DEFAULT_DEBUG_MASK	DBG_NONE

#if defined(TRANSLATOR) && defined(KAFFE_VMDEBUG)
extern int jit_debug;
#endif /* defined(TRANSLATOR) && defined(KAFFE_VMDEBUG) */

static char *debugBuffer;
static size_t bufferBegin = 0;
static size_t bufferSz = 16 * 1024;
static int bufferOutput = 0;
static int kaffe_dprintf_fd = 2;

/**
 * Set the file descriptor used for debug output.
 *
 * @param fd new file descriptor
 */
void
dbgSetDprintfFD(int fd)
{
	kaffe_dprintf_fd = fd;
}

#if defined(NDEBUG) || !defined(KAFFE_VMDEBUG)
/* --- Debugging is NOT enabled --- */

/* Don't waste space with the debugging functions */

void dbgSetMask(debugmask_t UNUSED m) { }
int dbgSetMaskStr(const char UNUSED *s) { return 0; }

#else /* Actually define the functions */
/* --- Debugging is enabled --- */

/* Defines what debugging output is seen. Needs to be 64-bit. */
static debugmask_t kaffevmDebugMask = DEFAULT_DEBUG_MASK;

/**
 * Get the debug mask. 
 *
 * The debug mask defines what kind of
 * debugging output is enabled or disabled.
 *
 * @return current debug mask
 */
debugmask_t
dbgGetMask(void)
{
	return kaffevmDebugMask;
}

/**
 * Set the debug mask. 
 *
 * The debug mask defines what kind of
 * debugging output is enabled or disabled.
 *
 * @param mask new debug mask
 */
void
dbgSetMask(debugmask_t mask)
{
	kaffevmDebugMask = mask;
}

/*
 * Associate strings with an option or set of
 * options.
 */
static struct debug_opts
{
	const char *name;
	debugmask_t mask;
	const char *desc;
} debug_opts[] =
{
	/* XXX Clean these names up, re-order them and make them consistent. */
#define D(name, str) { #name, DBG_ ## name, str }
	D(NONE, "Nothing"),
	D(SLOWLOCKS,  "Heavy lock/unlock operations"),
	D(VMCONDS,  "Show condition variable operations called by VM"),
	D(NEWINSTR, "Show softnew_ instructions (NEW, NEWARRAY, ANEWARRAY)"),
	D(VMTHREAD, "Show some java.lang.Thread operations called by VM"),
	D(JTHREAD,  "Show jthread operations--jthreads only"),
	D(JTHREADDETAIL,  "Show jthread operations (more detail)"),
	D(JTHREADNOPREEMPT,  "Disable preemption in--jthreads only"),
	D(EXCEPTION, "Debug exceptions, don't catch traps"),
	D(STACKTRACE, "Debug stack trace inspection"),
	D(INIT,     "Show initialization steps."),
	D(INITCLASSPATH,     "Show classpath initialization."),
	D(BREAKONEXIT, "Cause an exception before exiting. (Useful under GDB)"),
	D(GCPRIM,   "Debug gc_primitive_*"),
	D(GCALLOC,   "Debug gc_heap_alloc*"),
	D(GCFREE,   "Debug gc_heap_free*"),
	D(GCSYSALLOC,   "Show allocations of system memory"),
	D(GCWALK,   "Show gc walking"),
	D(GCPRECISE, "Debug precise collection of the heap."),
	D(GCSTAT,   "count instances of class and array types"),
	D(GCDIAG,   "Perform diagnostic checks on heap" ),
	{ "GCMEM", DBG_GCPRIM|DBG_GCALLOC|DBG_GCFREE|DBG_GCSYSALLOC|DBG_GCSTAT,
			"All allocation and free operations in gc-mem" },
	D(SLACKANAL,   "Print internal fragmentation statistics."),
	D(SYSDEPCALLMETHOD,	"Debug system-dependent call method."),

	D(ASYNCSTDIO, "Make stdio fds asynchronous despite debugging."),
	D(CATCHOUTOFMEM, "Catch recursive out of memory exceptions."),
	D(JARFILES, "Debug reading JAR files in jar.c."),
	D(CODEATTR, "Show code attributes during class file parsing."),
	D(INT_INSTR, "Show instructions. (interpreter)"),
	D(INT_NATIVE, "Show call to native methods. (interpreter)"),
	D(INT_RETURN, "Show return from function. (interpreter)"),
	D(INT_VMCALL, "Show call to virtualMachine. (interpreter)"),
	D(INT_CHECKS, "Show various checks. (interpreter)"),
	D(ELOOKUP, "Debug exception lookup."),
	D(FLOOKUP, "Debug field lookup."),
	D(MLOOKUP, "Debug method lookup."),
	D(CLASSLOOKUP, "Debug class lookup."),
	D(JIT, 	"Debug JIT compiler--show emitted instructions."),
	D(MOREJIT, 	"Debug JIT compiler--show callinfo."),
	D(NOGC,	"Turn garbage collection off."),
	D(STATICINIT,	"Announce when static initializers are called."),
	D(CLASSFILE,	"Show when methods and fields are added."),
	D(RESERROR,	"Show error in class file resolution."),
	D(VMCLASSLOADER,"Announce when VM calls class loaders.."),
	D(LOCKCONTENTION,"Show when a lock is contended."),
	D(CODEANALYSE, "Debug code analysis/verification."),
	D(CLASSGC, 	"Debug class garbage collection."),
	D(NEWOBJECT, 	"Show when new objects are allocated."),
	D(FINALIZE, 	"Debug finalization."),
	D(NATIVELIB, 	"Debug native library operations."),
	D(NATIVENET,	"Show network accesses from the native code."),

	D(AWT_MEM,	"AWT subsystem."),
	D(AWT_CLR,	"AWT subsystem."),
	D(AWT_EVT,	"AWT subsystem."),
	D(AWT_IMG,	"AWT subsystem."),
	D(AWT_WND,	"AWT subsystem."),
	D(AWT_GRA,	"AWT subsystem."),
	D(AWT_FNT,	"AWT subsystem."),
	D(AWT,		"Debug all of AWT."),

	D(GCJ,		"Debug GCJ support."),
	D(GCJMORE,	"Debug GCJ support (additional msg)."),

	D(REGFORCE,	"Debug forced registers framework."),
	D(READCLASS,	"Trace readClass() parsing of .class files."),
	
	D(VERIFY2,      "Debug verifier, pass 2...outputs constant pool."),
	D(VERIFY3,      "Debug verifier, pass 3...outputs walkthrough of data-flow analysis."),
	D(VERIFY,       "Debug verifier, passes 2 and 3...produces a TON of output."),

	D(KSEM,		"Debug semaphores."),
	D(REFERENCE,	"Debug references."),

	/* you can define combinations too */
	{ "lookup", DBG_MLOOKUP|DBG_ELOOKUP|DBG_FLOOKUP,
			"Various lookup operations" },

	{ "gcj", DBG_GCJ|DBG_GCJMORE,
			"Debug GCJ support in detailed" },
	{ "thread", DBG_JTHREAD|DBG_SLOWLOCKS|DBG_VMCONDS,
			"Thread operations and locking operations" },

	{ "intrp", DBG_INT_NATIVE|DBG_INT_RETURN|DBG_INT_VMCALL,
			"Calls of interpreter (without instructions)" },
	{ "deadlock", DBG_LOCKCONTENTION, "Debug deadlocks" },
	{ "classgc", DBG_CLASSGC|DBG_FINALIZE,
			"Debug finalization (show finalization)" },
	{ "intrpA",
		DBG_INT_CHECKS|DBG_INT_INSTR|DBG_INT_NATIVE|
		DBG_INT_RETURN|DBG_INT_VMCALL,
			"Complete interpreter trace" },

	/* special options */
	{ "buffer", 0, "Log output to an internal buffer instead of stderr" },
	{ "dump", 0, "Print the buffer log when the program exits" },
	D(ALL, "Everything under the sun...."),
	D(ANY, "Ibid.")
};

static void debugToBuffer(int size);
static void debugSysInit(void);
static void printDebugBuffer(void);

#define NELEMS(a)	(sizeof (a) / sizeof(a[0]))

int
dbgSetMaskStr(const char *orig_mask_str)
{
	unsigned int i;
	char *mask_str;
	const char *separators = "|,";
	char *opt;

	debugSysInit();

	kaffevmDebugMask = DEFAULT_DEBUG_MASK;

	if (orig_mask_str == NULL) {
		return 1;
	}

	/* Duplicate in case orig_mask_str is read-only. */
	mask_str = strdup(orig_mask_str);
	if (mask_str == NULL)
	{
		dprintf("debug.c: Failed to allocate duplicate for %s. Debugging disabled.\n", orig_mask_str);
		return 0;
	}

	opt = strtok(mask_str, separators);

	if (!opt) {
		free(mask_str);
		return 1;
	}

	/* Special target 'list' lists all the defined options */
	if (!strcasecmp(opt, "list")) {
		dprintf("Available debug opts: \n");
		dprintf("  %-15s\t%16s  %s\n", "Option", "Mask", "Description");
		for (i = 0; i < NELEMS(debug_opts); i++)
			if (debug_opts[i].mask>>32)
			{
				dprintf("  %-15s\t%8X%08X  %s\n",
					debug_opts[i].name,
				       (int)(debug_opts[i].mask>>32),
				       (int)(debug_opts[i].mask),
				       debug_opts[i].desc);
			}
			else
			{
				dprintf("  %-15s\t        %8X  %s\n",
					debug_opts[i].name,
				       (int)(debug_opts[i].mask),
				       debug_opts[i].desc);
			}
		free(mask_str);
		return 0;
	}


	while (opt) {
		if (!strcasecmp(opt, "buffer"))
			debugToBuffer(64 * 1024);
		else if (!strcasecmp(opt, "dump"))
			atexit(printDebugBuffer);
		else
		{
			int set;

			switch(opt[0]) {
			case '-':
				opt++;
				set = 0;
				break;

			case '+':
				opt++;
				/* FALLTHRU */
			default:
				set = 1;
			}

			for (i = 0; i < NELEMS(debug_opts); i++)
				if (!strcasecmp(opt, debug_opts[i].name))
				{
					if (set)
						kaffevmDebugMask |= debug_opts[i].mask;
					else
						kaffevmDebugMask &= ~debug_opts[i].mask;
					break;
				}

			/* Allow bit shifts. */
			if (i == (sizeof debug_opts)/(sizeof(debug_opts[0]))) {
				char *endp;
				int bit;

				bit = strtoul(opt, &endp, 0);
				/* Be polite. */
				if (*endp != '\0') {
					dprintf("Unknown flag (%s) passed to -vmdebug\n",
						opt);
					free(mask_str);
					return 0;
				}
				if (set)
					kaffevmDebugMask |= DBG_BIT(bit);
				else
					kaffevmDebugMask &= ~DBG_BIT(bit);
			}
		}

		/* Get next opt */
		opt = strtok(NULL, separators);
	}

	if (kaffevmDebugMask & DBG_JIT) {
#if defined(TRANSLATOR) && defined(KAFFE_VMDEBUG)
		jit_debug = 1;
#else /* !(defined(TRANSLATOR) && defined(KAFFE_VMDEBUG)) */
		dprintf(
			"You cannot debug the JIT in interpreter mode \n");
#endif /* defined(TRANSLATOR) && defined(KAFFE_VMDEBUG) */
	}

	free(mask_str);
	return 1;
}

/*
 * create a buffer in which debugging output is written
 */
static void
debugToBuffer(int size)
{
	assert(size > 0);
	bufferSz = size;
	debugBuffer = malloc(bufferSz);
	bufferOutput = 1;
	assert(debugBuffer != NULL);
}

/*
 * The following function is invoked at exit time.
 * It deliberately caused a trap, to give control to gdb.
 */
static void
debugExitHook(void)
{
	/*
	 * this is a hook for catching exits from GDB.
	 * make this dependent on the selection of this option
	 */
	DBG(BREAKONEXIT, DBGGDBBREAK());
}

/*
 * initialize debugging system
 */
static void
debugSysInit(void)
{
	static int once = 0;

	if (once) return;
	once = 1;
#if defined(TRANSLATOR) && defined(KAFFE_VMDEBUG)
	{
		if (getenv("JIT_DEBUG"))
			jit_debug = 1;
	}
#endif /*  defined(TRANSLATOR) && defined(KAFFE_VMDEBUG) */
	atexit(debugExitHook);
}

static void
printDebugBuffer(void)
{
	unsigned int i = 0;
	unsigned int end = bufferBegin;

	i = bufferBegin;
	assert(i != 0);

	while(i < bufferSz)
		putc(debugBuffer[i++], stdout);
	i = 0;
	while(i < end)
		putc(debugBuffer[i++], stdout);
}

#endif /* KAFFE_VMDEBUG */


/*
 * When debugging, printf should use fprintf() to avoid
 * threading/blocking problems.
 */

int
kaffe_dprintf(const char *fmt, ...)
{

	int n;
	int max;
	va_list args;
	ssize_t w = 0;
	int rc;

	va_start(args, fmt);
	if (!debugBuffer)
		debugBuffer = malloc(bufferSz);

#ifdef HAVE_VSNPRINTF
	max = bufferSz - bufferBegin - 1;
	assert(max > 0);
	n = vsnprintf(debugBuffer + bufferBegin, (unsigned int)max, fmt, args);

	/* The return value is bytes *needed* not bytes *used* */
	if (n > max)
		n = max;
#else
	n = vsprintf(debugBuffer + bufferBegin, fmt, args);
#endif
	bufferBegin += n;
	assert(bufferBegin < bufferSz);/* XXX */

	if (bufferOutput) {
		if (bufferBegin >= (bufferSz - 60))
			bufferBegin = 0;
	} else {
		/* Keep trying to write.  Should we pause(), or
		 * sigsuspend(), or do something based on config
		 * defintions?
		 */
		max = 0;
		while (max < n) {
                        w = write(kaffe_dprintf_fd,
                                       debugBuffer + max,
                                       (size_t)n - max);
			rc = errno;

			if (w >= 0)
				/* ignore errors */
				max += w;
			else if (rc != SIGINT)
			  {
		            /* Stderr should have been closed by another thread.
			     * We may only exit without printing anything.
			     */
			    break;
			  }
			  
		}
		bufferBegin = 0;
	}
	va_end(args);

	return n;
}

