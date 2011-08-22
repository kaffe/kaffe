/*
* tile/linux/md.h
* Linux Tile configuration information.
*
* Copyright (c) 2011 Francisco Javier Guzmán Jiménez <dejavih@gmail.com>. All rights reserved.
* See the file "license.terms" for information on usage and redistribution
* of this file.
*
* Copyright (c) 1996, 1997
*     Transvirtual Technologies, Inc.  All rights reserved.
*
* Copyright (c) 2003
*     Helmer Kraemer <hkraemer@kaffe.org>. All rights reserved.
*
* written by Francisco Javier Guzmán Jiménez
* <dejavih@gmail.com><dejavih@hotmail.es><javierguzman@alu.uma.es>
*/
#ifndef __tile_linux_md_h

#define __tile_linux_md_h
#include "tile/common.h"
#include "tile/threads.h"

/*We suppose that we use LibFFI, if we do not use it, we must include sysdepCallMethod.h*/
#include <sysdepCallMethod-ffi.h>

#include <signal.h>
#include <ucontext.h>

#include "asm/page.h"
#define KAFFEMD_PAGESIZE PAGE_SIZE


#define SP_OFFSET 24

#define SIGNAL_ARGS(sig, sc) int sig, siginfo_t *tile_info, ucontext_t *sc
#define SIGNAL_CONTEXT_POINTER(scp) ucontext_t *scp
#define GET_SIGNAL_CONTEXT_POINTER(sc) (sc)
#define SIGNAL_PC(scp) (scp->uc_mcontext.sc_iaoq[0])
#endif

