/*
 * Copyright (c) 1998, 1999 The University of Utah. All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Copyright (c) 2003
 *      Kaffe.org contributors. See ChangeLog for details.
 *
 * Contributed by the Flux Research Group at the University of Utah.
 * Authors: Godmar Back, Leigh Stoller
 */

#ifndef __i386_oskit_md_h
#define __i386_oskit_md_h

#include "i386/common.h"
#include "i386/threads.h"

#if defined(HAVE_SYS_RESOURCE_H)
#include <sys/resource.h>
#endif
#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif

/*
 * Undefine stack pointer offset, we don't need it
 */
#undef	SP_OFFSET

#define SIGNAL_ARGS(sig, scp) int sig, int code, struct sigcontext* scp
#define SIGNAL_CONTEXT_POINTER(scp) struct sigcontext* scp
#define GET_SIGNAL_CONTEXT_POINTER(scp) (scp)
#define SIGNAL_PC(scp) ((scp)->sc_eip)
#define STACK_POINTER(scp) ((scp)->sc_sp)

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

void oskit_kaffe_main(int *pargc, char ***pargv);
void oskit_kaffe_init_md(void);

#define INIT_MD	oskit_kaffe_init_md

#define MAIN_MD oskit_kaffe_main(&argc, &argv)

/*
 * CLASSPATH
 */
extern char *default_classpath;
#define DEFAULT_CLASSPATH default_classpath

#include "kaffe-unix-stack.h"

/*
 * Main thread uses default stacksize.
 */
#define MAINSTACKSIZE	threadStackSize
#endif
