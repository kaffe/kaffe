/*
 * debug.h
 *
 * A dynamic debugging framework for Kaffe.  Through the magic
 * of hideous macros, we can control the debugging output of
 * Kaffe in a couple of ways.  First, it can be completely
 * disabled in which case all of the code "just goes away",
 * or, if it's enabled, specific debug statements can be dynamically
 * enabled at run time by setting the vmdebugmask, which is done
 * with the -vmdebug command line switch.
 *
 * INSERT COPYRIGHT HERE
 *
 * Written by Patrick Tullmann <tullmann@cs.utah.edu>, 1998
 *	   and Godmar Back <gback@cs.utah.edu>
 */
#ifndef __kaffevm_debug_h
#define __kaffevm_debug_h

/* Pascal Bourguignon <pjb@imaginet.fr> writes:
 * We include stdio here because on Linux, stdio defines dprintf.
 * Hence, we can override it with a macro defined here. (stdio.h
 * is include-once).
 */
#include <stdio.h>

/*
 * Available debugging options.
 * For an explanation, refer to the table in debug.c
 *
 * Adding an option is simple: pick a free number and add a define,
 * then #include "debug.h" in every file where you want to use the
 * option, and then use either DBG or DBGEXPR!
 *
 * If you add option, try to invent sensible categories.
 */
/* Debug Masks: (1 bit per option) */
# define DBG_BIT(x) (((jlong)1)<<x)
# define DBG_NONE		(0)
# define DBG_VMLOCKS            DBG_BIT(0)
# define DBG_VMCONDS		DBG_BIT(1)
# define DBG_NEWINSTR		DBG_BIT(2)
# define DBG_VMTHREAD		DBG_BIT(3) 
# define DBG_EXCEPTION		DBG_BIT(4) 
# define DBG_JTHREAD		DBG_BIT(5) 
# define DBG_JTHREADDETAIL	DBG_BIT(6) 
# define DBG_DETECTDEADLOCK	DBG_BIT(7) 
# define DBG_BREAKONEXIT	DBG_BIT(8) 
# define DBG_INIT		DBG_BIT(9) 
# define DBG_GCPRIM		DBG_BIT(10) 
# define DBG_GCSYSALLOC		DBG_BIT(11) 
# define DBG_GCALLOC		DBG_BIT(12) 
# define DBG_GCFREE		DBG_BIT(13) 
# define DBG_GCSTAT		DBG_BIT(14) 
# define DBG_ASYNCSTDIO		DBG_BIT(15) 
# define DBG_CATCHOUTOFMEM	DBG_BIT(16) 
# define DBG_JARFILES		DBG_BIT(17) 
# define DBG_INT_INSTR		DBG_BIT(18) 
# define DBG_INT_NATIVE		DBG_BIT(19) 
# define DBG_INT_RETURN		DBG_BIT(20) 
# define DBG_INT_VMCALL		DBG_BIT(21) 
# define DBG_INT_CHECKS		DBG_BIT(22) 
# define DBG_JTHREADNOPREEMPT	DBG_BIT(23) 
# define DBG_CODEATTR		DBG_BIT(24) 
# define DBG_ELOOKUP		DBG_BIT(25) 
# define DBG_FLOOKUP		DBG_BIT(26) 
# define DBG_MLOOKUP		DBG_BIT(27) 
# define DBG_NOGC		DBG_BIT(28) 
# define DBG_JIT		DBG_BIT(29) 
# define DBG_MOREJIT		DBG_BIT(30) 

/* let's reserve 50-63 for temporary uses */
# define DBG_SLACKANAL		DBG_BIT(50) 

# define DBG_ALL		((jlong)(-1))
# define DBG_ANY                DBG_ALL


#if defined(NDEBUG) || !defined(DEBUG)
/* --- Debugging is NOT enabled --- */

/*
 * DBGIF:
 * 	if debugging is disabled, resolves to nothing.
 *	if debugging is enabled, resolve to statement.
 *	shortcut for #if DEBUG ... #endif
 */
# define DBGIF(statement)  { ((void)0); }

/*
 * DBG:
 * 	if debugging is disabled, resolves to nothing.
 *	if debugging is enabled, statement is executed if mask matches.
 */
# define DBG(mask, statement)  { ((void)0); }

/*
 * DBGEXPR:
 * 	if debugging is disabled, resolve to default.
 *	if debugging is enabled, evaluates to expr if mask matches, else 
 *		to the default value.
 *
 *	Example:  if (DBGEXPR(EXCEPTION, false, true))
 *			catchSignal(SIGSEGV);
 *
 *	does not catch SIGSEGV if you're debugging exceptions, but in all
 *	other cases.
 */
# define DBGEXPR(mask, expr, default) (default)

/*
 * DBGGDBBREAK:
 * 	insert if you want gdb to gain control.
 * 	causes an exception in debugging mode.
 */
# define DBGGDBBREAK() { ((void)0); }

#elif !defined(KAFFEH) 
/* --- Debugging is enabled --- */

#include "jtypes.h"

/* Defines what debugging output is seen. Needs to be 64-bit. */
extern jlong kaffevmDebugMask;

/* Set the debugging mask to use. (give the mask) */
void dbgSetMask(jlong mask);

/* 
 * Set the debugging mask to use. (give a string, useful for
 * parsing a command line option.  The string is a comma-separated
 * list of options which combined form the debugging mask.
 */
void dbgSetMaskStr(char *mask_str);

# define DBGIF(statement)  statement

/* Debug macros that are selected with the above flags. */

# define DBG(mask, statement) {			\
	if ((DBG_##mask)&(kaffevmDebugMask)) {	\
		statement;			\
	}					\
	}

# define DBGEXPR(mask, expr, default)			\
	(((DBG_##mask)&(kaffevmDebugMask))?(expr):(default))

/* Do something that would cause GDB to gain control. */
# define DBGGDBBREAK() { (*(int*)0) = 42; }

/* XXX: change dprintf to kaffe_dprintf and get rid of the macro */
# define dprintf       kaffe_dprintf

extern int kaffe_dprintf(const char *fmt, ...);

#else	/* !defined(KAFFEH) */

/* --- give some simple macros for debugging kaffeh */

# define DBG(mask, statement)		statement
# define DBGEXPR(mask, expr, default) 	expr
# define DBGIF(statement)  statement
/* we don't link with debug.o, so make dprintf a simple printf */
# define dprintf	printf

#endif /* defined(NDEBUG) || !defined(DEBUG) */

#endif /* __kaffevm_debug_h */
