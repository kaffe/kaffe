/* funcs.c
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "gtypes.h"
#include "seq.h"
#include "slots.h"
#include "registers.h"
#include "labels.h"
#include "basecode.h"
#include "itypes.h"
#include "md.h"
#include "debug.h"
#include "classMethod.h"
#include "icode.h"
#include "machine.h"

#undef	define_insn
#define	define_insn(n, i) void i (sequence* s)

nativecode* codeblock;
uintp CODEPC;

#define ALIGN_CODEPC(byte)						\
	(CODEPC = (CODEPC % (byte)					\
		   ? CODEPC + (byte) - (CODEPC % (byte))		\
		   : CODEPC))

#if defined(KAFFE_VMDEBUG)
/*
 * Print any labels that refer to the current address.
 */
static
void printCodeLabels(void)
{
	label *il, *curr = 0;

	while( (il = getInternalLabel(&curr, CODEPC)) )
	{
		kaffe_dprintf("%s:\n", il->name);
	}
}
#undef OUT
#define	OUT	(DBGEXPR(JIT,(void)printCodeLabels(),0),codeblock[CODEPC++])
#define	BOUT	(DBGEXPR(JIT,(void)printCodeLabels(),0),*(uint8*)&codeblock[CODEPC++])
#define	WOUT	(DBGEXPR(JIT,(void)printCodeLabels(),0),*(uint16*)&codeblock[(CODEPC += 2) - 2])
#define	LOUT	(DBGEXPR(JIT,(void)printCodeLabels(),0),*(uint32*)&codeblock[(CODEPC += 4) - 4])
#define	QOUT	(DBGEXPR(JIT,(void)printCodeLabels(),0),*(uint64*)&codeblock[(CODEPC += 8) - 8])
#else
#undef OUT
#define	OUT	(codeblock[CODEPC++])
#define	BOUT	(*(uint8*)&codeblock[CODEPC++])
#define	WOUT	(*(uint16*)&codeblock[(CODEPC += 2) - 2])
#define	LOUT	(*(uint32*)&codeblock[(CODEPC += 4) - 4])
#define	QOUT	(*(uint64*)&codeblock[(CODEPC += 8) - 8])
#endif

#include "jit.def"
