/* #include "../jit/funcs.c" */
/* funcs.c
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "debug.h"
#include "config-std.h"
#include "gtypes.h"
#include "seq.h"
#include "slots.h"
#include "registers.h"
#include "labels.h"
#include "basecode.h"
#include "itypes.h"
#include "errors.h"
#include "machine.h"
#include "md.h"

/* XXX: HUGE name clash: machine.h defines a define_insn for use in 
 * kaffe.def.  The define_insn below, however, is used in jit.def
 *
 * We must rename one or the other!
 */
#undef define_insn

#define	define_insn(n, i) void i (sequence* s)

#define ALIGN(byte)							\
	(CODEPC = (CODEPC % (byte)					\
		   ? CODEPC + (byte) - (CODEPC % (byte))		\
		   : CODEPC))

#define	OUT	(codeblock[CODEPC++])
#define	BOUT	(*(uint8*)&codeblock[CODEPC++])
#define	WOUT	(*(uint16*)&codeblock[(CODEPC += 2) - 2])
#define	LOUT	(*(uint32*)&codeblock[(CODEPC += 4) - 4])
#define	QOUT	(*(uint64*)&codeblock[(CODEPC += 8) - 8])

#include "jit.def"
#include "trampolines.c"
