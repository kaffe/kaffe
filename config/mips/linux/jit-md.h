/*
 * mips/linux/jit-md.h
 * MIPS Linux JIT configuration information.
 *
 * Copyright (c) 1996-1999 Transvirtual Technologies, Inc.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * Written by Tim Wilkinson <tim@transvirtual.com>
 */

#ifndef __mips_linux_jit_md_h
#define __mips_linux_jit_md_h

/**/
/* Include common information. */
/**/
#include "mips/jit.h"

/*
 * newer Linux kernel actually implement SA_SIGINFO.
 * But we don't need it, so let's turn it off
 */
#if defined(SA_SIGINFO)
#undef SA_SIGINFO
#endif

#endif
