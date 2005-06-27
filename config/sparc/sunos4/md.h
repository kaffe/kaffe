/*
 * sparc/sunos4/md.h
 * SunOS4 sparc configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __sparc_sunos_md_h
#define __sparc_sunos_md_h

#include "sparc/common.h"
#include "sparc/threads.h"
#if defined(HAVE_SYS_RESOURCE_H)
#include <sys/resource.h>
#endif
#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif

/*
 * Redefine stack pointer offset.
 */
#undef  SP_OFFSET
#define SP_OFFSET       2

#undef SA_SIGINFO
#define SIGNAL_ARGS(sig, sc) int sig, int code, struct sigcontext* sc, char* addr
#define SIGNAL_CONTEXT_POINTER(scp) struct sigcontext* scp
#define GET_SIGNAL_CONTEXT_POINTER(scp) (scp)
#define SIGNAL_PC(scp) ((scp)->sc_pc)
#define STACK_POINTER(scp) ((scp)->sc_sp)

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

#include "kaffe-unix-stack.h"

#endif
