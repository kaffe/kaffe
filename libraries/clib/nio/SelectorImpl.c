/*
 * SelectorImpl.c
 *
 * Copyright (c) 2003, 2004.
 *    The Kaffe.org's developers. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#include "config.h"
#include "config-std.h"
#include "config-io.h"
#include "files.h"
#include "gnu_java_nio_SelectorImpl.h"
#include "jsyscall.h"
#include "native.h"
#include "Arrays.h"

static void checkInSelectTable(HArrayOfInt* fdArray, fd_set *set, int *num_fd)
{
  jint *iarray;
  int i;

  FD_ZERO(set);
  
  iarray = unhand_int_array(fdArray);
  for (i=0;i<obj_length(fdArray);i++)
    {
      FD_SET(iarray[i], set);
      if (iarray[i] >= (*num_fd))
	*num_fd = iarray[i]+1;
    }
}

static void checkOutSelectTable(HArrayOfInt* fdArray, fd_set *set, int *num_fd)
{
  int i;
  jint *iarray;

  iarray = unhand_int_array(fdArray);
  for (i=0;i<obj_length(fdArray);i++)
    {
      if (!FD_ISSET(iarray[i], set))
	iarray[i] = 0;
      else
	(*num_fd)++;
    }
} 

jint
gnu_java_nio_VMSelector_select(HArrayOfInt* readA, HArrayOfInt* writeA,
			       HArrayOfInt* exceptA, jlong timeout)
{
  fd_set read_set, write_set, except_set;
  int num_fd = 0;
  int ret, r;
  struct timeval tval;

  checkInSelectTable(readA, &read_set, &num_fd);
  checkInSelectTable(writeA, &write_set, &num_fd);
  checkInSelectTable(exceptA, &except_set, &num_fd);
  
  if (timeout < 0)
    r = KSELECT(num_fd, &read_set, &write_set, &except_set, NULL, &ret);
  else
    {
      tval.tv_sec = timeout / 1000;
      tval.tv_usec = timeout * 1000;
      r = KSELECT(num_fd, &read_set, &write_set, &except_set, &tval, &ret);
    }

  if (r != 0)
    SignalError("java.io.IOException", SYS_ERROR(r));

  num_fd = 0;
  checkOutSelectTable(readA, &read_set, &num_fd);
  checkOutSelectTable(writeA, &write_set, &num_fd);
  checkOutSelectTable(exceptA, &except_set, &num_fd);

  return num_fd;
}
