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
#include "funcs.h"
#include "kaffe_jni.h"

#undef	define_insn
#define	define_insn(n, i) void i (sequence* s)

nativecode* codeblock;
uintp CODEPC;

#if defined(KAFFE_VMDEBUG)
/*
 * Print any labels that refer to the current address.
 */
static
void printCodeLabels(void)
{
	label *il, *curr = NULL;

	while( (il = KaffeJIT3_getInternalLabel(&curr, CODEPC)) )
	{
		kaffe_dprintf("%s:\n", il->name);
	}
}
#undef OUT
#define	OUT(v)	do { DBGEXPR(JIT,(void)printCodeLabels(),(void)0),codeblock[CODEPC] = v; CODEPC++; } while (0)
#define	BOUT(v)	do { DBGEXPR(JIT,(void)printCodeLabels(),(void)0),*(uint8*)&codeblock[CODEPC] = v; CODEPC++; } while (0)
#define	WOUT(v)	do { DBGEXPR(JIT,(void)printCodeLabels(),(void)0),*(uint16*)&codeblock[CODEPC] = v; CODEPC += 2; } while (0)
#define	LOUT(v)	do { DBGEXPR(JIT,(void)printCodeLabels(),(void)0),*(uint32*)&codeblock[CODEPC] = v; CODEPC += 4; } while (0)
#define	QOUT(v)	do { DBGEXPR(JIT,(void)printCodeLabels(),(void)0),*(uint64*)&codeblock[CODEPC] = v; CODEPC += 8; } while (0)
#else /* !  defined(KAFFE_VMDEBUG) */
#undef OUT
#define	OUT(v)	do { codeblock[CODEPC] = v; CODEPC++; } while (0)
#define	BOUT(v)	do { *(uint8*)&codeblock[CODEPC] = v; CODEPC++; } while (0)
#define	WOUT(v)	do { *(uint16*)&codeblock[CODEPC] = v; CODEPC += 2; } while (0)
#define	LOUT(v)	do { *(uint32*)&codeblock[CODEPC] = v; CODEPC += 4; } while (0)
#define	QOUT(v)	do { *(uint64*)&codeblock[CODEPC] = v; CODEPC += 8; } while (0)
#endif /* defined(KAFFE_VMDEBUG) */

#include "jit.def"
