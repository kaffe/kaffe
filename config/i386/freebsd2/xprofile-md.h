/*
 * xprofile-md.h
 * Interface functions to the profiling code
 *
 * Copyright (c) 2001 University of Utah and the Flux Group.
 * All rights reserved.
 *
 * This file is licensed under the terms of the GNU Public License.  
 * See the file "license.terms" for restrictions on redistribution 
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */

#ifndef __i386_freebsd2_xprofile_md_h
#define __i386_freebsd2_xprofile_md_h

int kaffeStdProfRate(void);
#define KAFFE_STD_PROF_RATE \
int kaffeStdProfRate(void) \
{ \
	struct clockinfo clockinfo; \
	int retval = 0; \
	size_t size; \
	int mib[2]; \
	\
	size = sizeof(clockinfo); \
	mib[0] = CTL_KERN; \
	mib[1] = KERN_CLOCKRATE; \
	if (sysctl(mib, 2, &clockinfo, &size, NULL, 0) < 0) { \
	} else if (clockinfo.profhz == 0) { \
		retval = clockinfo.hz; \
	} else { \
		retval = clockinfo.profhz; \
	} \
	return( retval ); \
}

struct gmonparam;
extern struct gmonparam _gmonparam;

/* Return a pointer to the gmonparam structure defined by the compiler */
static inline struct gmonparam *getGmonParam(void)
{
	return &_gmonparam;
}

/* Override the standard mcount with the xprofiling one */
#include <machine/profile.h>

#define _KAFFE_OVERRIDE_MCOUNT_DEF _MCOUNT_DECL __P((uintfptr_t frompc, uintfptr_t selfpc))
#define _KAFFE_OVERRIDE_MCOUNT MCOUNT

#endif
