/*
 * mips/ultrix4/md.h
 * Ultrix4.2 MIPS configuration information.
 *
 * Copyright (c) 1996, 1997, 1998
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2003
 *      Kaffe.org contributors. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __mips_ultrix4_md_h
#define __mips_ultrix4_md_h

#include "mips/sysdepCallMethod.h"

#define	THREADSTACKSIZE		(32 * 1024)

/*
 * Stack offset.
 * This is the offset into the setjmp buffer where the stack pointer is
 * stored.  This may be different with different OSes.
 */

#define	SP_OFFSET	32

/*
 * Force to use homebrew longjmp rather than system provided one.
 * Since we want to use system provided setjmp (which is in the
 * same object file with longjmp), we can not simply name our
 * version of longjmp as 'longjmp'.
 * The 'ultrixlongjmp' is in 'md.c'.
 */
#define longjmp         ultrixlongjmp
extern void ultrixlongjmp(int*, int);

extern int socket(int, int, int);
extern int sendto(int, char*, int, int, struct sockaddr*, int);
extern int setsockopt(int, int, int, char*, int);
extern int getsockopt(int, int, int, char*, int*);
extern int getsockname(int, struct sockaddr*, int*);
extern int getpeername(int, struct sockaddr*, int*);
extern int select(int, fd_set*, fd_set*, fd_set*, struct timeval*);
extern int kill(pid_t, int);

#endif
