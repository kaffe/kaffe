/*
 * Copyright (c) 1998, 1999 The University of Utah. All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Contributed by the Flux Research Group at the University of Utah.
 * Authors: Godmar Back, Leigh Stoller
 */

#ifndef __i386_oskit_md_h
#define __i386_oskit_md_h

#include "i386/common.h"
#include "i386/threads.h"

/*
 * Undefine stack pointer offset, we don't need it
 */
#undef	SP_OFFSET

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

/*
 * Main thread uses default stacksize.
 */
#define MAINSTACKSIZE	threadStackSize
#endif
