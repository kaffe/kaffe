/*
 * i386/freebsd2/md.c
 * FreeBSD i386 specific functions.
 *
 * 
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"

#if defined(HAVE_SYS_SYSCTL_H)
#include <sys/sysctl.h>

void *mdGetStackBase(void)
{
    int control_name[2] =
      { CTL_KERN, KERN_USRSTACK};
    ptr_t stackbase;
    size_t len = sizeof(ptr_t);
    
    if (sysctl(nm, 2, &base, &len, NULL, 0))
      {
	fprintf(stderr, 
		"Kaffe VM has not been able to retrieve the pointer to the stack base\n"
		"Aborting...\n");
	exit(1);
      }
   return base;
}

#endif
