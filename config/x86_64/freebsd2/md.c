/*
 * x86_64/freebsd2/md.c
 * FreeBSD AMD64 specific functions.
 *
 * Copyright(c) 2004 The Kaffe.org's developers.
 *
 * Some parts are imported from the boehm-gc.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"

#if defined(HAVE_SYS_SYSCTL_H)
#include <sys/sysctl.h>

void *mdGetStackEnd(void)
{
    int control_name[2] =
      { CTL_KERN, KERN_USRSTACK};
    void *stackbase;
    size_t len = sizeof(void *);
    
    if (sysctl(control_name, 2, &stackbase, &len, NULL, 0))
      {
	fprintf(stderr, 
		"Kaffe VM has not been able to retrieve the pointer to the stack base\n"
		"Aborting...\n");
	exit(1);
      }
   return stackbase;
}

#endif
