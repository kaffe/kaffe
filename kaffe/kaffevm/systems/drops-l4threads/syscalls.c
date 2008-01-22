/*
 * syscall.c
 * Definition of jsyscall interface for Kaffe on DROPS.
 * 
 * Copyright (c) 2004, 2005
 *	TU Dresden, Operating System Group.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * written by Alexander Boettcher <ab764283@os.inf.tu-dresden.de>
 */

#include "config.h"
#include "debug.h"
#include "config-std.h"
#include "config-mem.h"
#include "config-net.h"
#include "config-io.h"
#include "config-signal.h"
#include "jthread.h"
#include "jsyscall.h"

#if defined(HAVE_SYS_TYPES_H)
#include <sys/types.h>
#endif /* defined(HAVE_SYS_TYPES_H) */

static int drops_open(const char *path,
                      int f, int m,
                      int *outfd) {

  *outfd = open(path, f, m);
  if (*outfd == -1) 
    return errno;

  return 0;
}

static int drops_read(int fd, void* buffer,
                      size_t len, ssize_t *out) {
  int nReturn = 0;

  *out = read(fd, buffer, len);

  if (*out == -1)
    nReturn = errno;

  return nReturn;
}

static int drops_close(int fd) {
  int nReturn = 0;

  if (close(fd) < 0)
    nReturn = errno;

  return nReturn;
}

static int drops_fstat(int fd, struct stat *st) {
  int nReturn = 0;

  if (fstat(fd, st) == -1)
    nReturn = errno;
  
  return nReturn;
}

static int drops_stat(const char *p, struct stat *st){
  int nReturn = 0;

  if (stat(p, st) == -1)
    nReturn = errno;
 
  return nReturn;
}

SystemCallInterface Kaffe_SystemCallInterface = {
	drops_open,
	drops_read,
	drops_close,
	drops_fstat,
	drops_stat,
};
