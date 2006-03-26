/*
 * sparc/openbsd2/md.c
 * OpenBSD Sparc specific functions.
 *
 * Copyright (c) 1996, 1997, 1998
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"

#if defined(KAFFE_SYSTEM_UNIX_PTHREADS)

#include <sys/signal.h>
#include <pthread.h>
#include <pthread_np.h>
#include <stdio.h>
#include <stdlib.h>

void *mdGetStackEnd(void)
{
    stack_t stack;
    if (pthread_stackseg_np(pthread_self(), &stack) != 0)
      {
        fprintf(stderr,
                "Kaffe VM has not been able to retrieve the pointer to the stack base\n"
                "Aborting...\n");
        exit(1);
      }
    return stack.ss_sp;
}

size_t mdGetStackSize(void)
{
    stack_t stack;
    if (pthread_stackseg_np(pthread_self(), &stack) != 0)
      {
        fprintf(stderr,
                "Kaffe VM has not been able to retrieve the stack size\n"
                "Aborting...\n");
        exit(1);
      }
    return stack.ss_size;
}
#endif
