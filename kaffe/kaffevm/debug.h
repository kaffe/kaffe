/*
 * Copyright (c) 1998, 1999, 2000 The University of Utah. All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Contributed by the Flux Research Group at the University of Utah.
 * Authors: Godmar Back, Patrick Tullmann 
 */
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
 */
#ifndef __kaffevm_debug_h
#define __kaffevm_debug_h

#include "config.h"

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
 * If you add options, try to invent sensible categories.
 */
/* Debug Masks: (1 bit per option) */
# define DBG_BIT(x) (((debugmask_t)1)<<x)
# define DBG_NONE		(0)
# define DBG_SLOWLOCKS          DBG_BIT(0)
# define DBG_VMCONDS		DBG_BIT(1)
# define DBG_NEWINSTR		DBG_BIT(2)
# define DBG_VMTHREAD		DBG_BIT(3) 
# define DBG_EXCEPTION		DBG_BIT(4) 
# define DBG_JTHREAD		DBG_BIT(5) 
# define DBG_JTHREADDETAIL	DBG_BIT(6) 
# define DBG_STACKTRACE		DBG_BIT(7)
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
# define DBG_STATICINIT		DBG_BIT(31) 
# define DBG_RESERROR		DBG_BIT(32)
# define DBG_GCPRECISE		DBG_BIT(33)
# define DBG_CLASSFILE		DBG_BIT(34)
# define DBG_VMCLASSLOADER	DBG_BIT(35)
# define DBG_GCWALK		DBG_BIT(36)
# define DBG_GCDIAG		DBG_BIT(37)
# define DBG_LOCKCONTENTION	DBG_BIT(38)
# define DBG_CODEANALYSE	DBG_BIT(39)
# define DBG_CLASSGC		DBG_BIT(40)
# define DBG_NEWOBJECT		DBG_BIT(41)
# define DBG_FINALIZE		DBG_BIT(42)
# define DBG_NATIVELIB		DBG_BIT(43)
# define DBG_NATIVENET		DBG_BIT(44)

# define DBG_AWT_MEM		DBG_BIT(45)
# define DBG_AWT_CLR		DBG_BIT(46)
# define DBG_AWT_EVT		DBG_BIT(47)
# define DBG_AWT_IMG		DBG_BIT(48)
# define DBG_AWT_WND		DBG_BIT(49)
# define DBG_AWT_GRA		DBG_BIT(50)
# define DBG_AWT_FNT		DBG_BIT(51)
# define DBG_AWT		(DBG_AWT_MEM | DBG_AWT_CLR | DBG_AWT_EVT    \
				 | DBG_AWT_IMG | DBG_AWT_WND | DBG_AWT_GRA  \
				 | DBG_AWT_FNT)

# define DBG_SLACKANAL		DBG_BIT(52) 
# define DBG_GCJ		DBG_BIT(53) 
# define DBG_GCJMORE		DBG_BIT(54) 
# define DBG_INITCLASSPATH      DBG_BIT(55)
# define DBG_CLASSLOOKUP        DBG_BIT(56)

# define DBG_REGFORCE		DBG_BIT(57)
# define DBG_SYSDEPCALLMETHOD   DBG_BIT(58)
# define DBG_READCLASS		DBG_BIT(59)

# define DBG_VERIFY2            DBG_BIT(60)
# define DBG_VERIFY3            DBG_BIT(61)
# define DBG_VERIFY             (DBG_VERIFY2 | DBG_VERIFY3)

# define DBG_KSEM               DBG_BIT(62)
# define DBG_REFERENCE          DBG_BIT(63)

# define DBG_ALL		((debugmask_t)(-1))
# define DBG_ANY                DBG_ALL

/*
 * Debugging mask type.  Must be at least 64 bits.
 *
 * Define the type in both debug and non-debug versions.
 */
#if SIZEOF_LONG >= 8
typedef long int        debugmask_t;
#elif SIZEOF___INT64 == 8
typedef __int64         debugmask_t;
#elif SIZEOF_LONG_LONG >= 8
typedef long long int   debugmask_t;
#else
#error "no known 64-bit type for debugmask_t"
#endif

#if defined(NDEBUG) || !defined(KAFFE_VMDEBUG)
/* --- Debugging is NOT enabled --- */

/*
 * DBGIF:
 * 	if debugging is disabled, resolves to nothing.
 *	if debugging is enabled, resolve to statement.
 *	shortcut for #if KAFFE_VMDEBUG ... #endif
 */
# define DBGIF(statement) do { ((void)0); } while (0)

/*
 * DBG:
 * 	if debugging is disabled, resolves to nothing.
 *	if debugging is enabled, statement is executed if mask matches.
 */
# define DBG(mask, statement) do { ((void)0); } while (0)

/*
 * DBGEXPR:
 * 	if debugging is disabled, resolve to default.
 *	if debugging is enabled, evaluates to expr if mask matches, else 
 *		to the default value.
 *
 *	Example:  if (DBGEXPR(EXCEPTION, false, true))
 *			restoreSynchSignalHandler(SIGSEGV);
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
# define DBGGDBBREAK() do { ((void)0); } while (0)

#else
/* --- Debugging is enabled --- */

extern debugmask_t dbgGetMask(void);

# define DBGIF(statement)  do { statement; } while (0)

/* Debug macros that are selected with the above flags. */

# define DBG(mask, statement) do {			\
	if ((DBG_##mask)&(dbgGetMask())) {	\
		statement;			\
	}					\
	} while (0)

# define DBGEXPR(mask, expr, default)			\
	(((DBG_##mask)&(dbgGetMask()))?(expr):(default))

/* Do something that would cause GDB to gain control. */
# define DBGGDBBREAK() do { (*(int*)0) = 42; } while (0)

#endif /* defined(NDEBUG) || !defined(KAFFE_VMDEBUG) */


#undef dprintf
#define dprintf       kaffe_dprintf

#if defined(__GNUC__)
#define KFUNC_FORMAT(archtype,fmt,arg) __attribute__((__format__ (archtype,fmt,arg)))
#else
#define KFUNC_FORMAT(archtype,fmt,arg)
#endif

extern void dbgSetDprintfFD(int fd);
int kaffe_dprintf(const char *fmt, ...) KFUNC_FORMAT(printf,1,2);

/* Set the debugging mask to use. (give the mask) */
void dbgSetMask(debugmask_t mask);

/* 
 * Set the debugging mask to use. (give a string, useful for
 * parsing a command line option.  The string is a comma-separated
 * list of options which combined form the debugging mask.
 *
 * Return false if nothing was set
 */
int dbgSetMaskStr(const char *mask_str);

#endif /* __kaffevm_debug_h */
