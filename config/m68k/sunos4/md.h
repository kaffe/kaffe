/*
 * m68k/sunos4/md.h
 * SunOS4 specific functions.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * Written by Kiyo Inaba <inaba@src.ricoh.co.jp>, 1998;
 */

#ifndef __m68k_sunos4_md_h
#define __m68k_sunos4_md_h

#include "m68k/common.h"
#include "m68k/threads.h"
#include "m68k/sysdepCallMethod.h"

/*
 * Redefine stack pointer offset.
 */
#undef  SP_OFFSET
#define SP_OFFSET       2

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

struct sockaddr;
extern int rename(const char*, const char*);
extern int remove(const char*);
extern int socket(int, int, int);
extern ssize_t sendto(int, const void*, size_t, int, const struct sockaddr*, int);
extern int setsockopt(int, int, int, const void*, int);
extern int getsockopt(int, int, int, void*, int*);
extern int getsockname(int, struct sockaddr*, int*);
extern int getpeername(int, struct sockaddr*, int*);
extern int select(int, fd_set*, fd_set*, fd_set*, struct timeval*);
extern int vfprintf(FILE *, char *, va_list);

#endif
