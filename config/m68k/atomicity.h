/* Low-level functions for atomic operations.  m680x0 version, x >= 2.
   Copyright (C) 1997 Free Software Foundation, Inc.
   Contributed by Andreas Schwab <schwab@issan.informatik.uni-dortmund.de>.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

/* Taken from glibc-2.3.2, from sysdeps/m68k/m68020/atomicity.h */

#ifndef _ATOMICITY_H
#define _ATOMICITY_H    1

static inline int
__attribute__ ((unused))
compare_and_swap (volatile long int *p, long int oldval, long int newval)
{
  char ret;
  long int readval;

  __asm__ __volatile__ ("cas%.l %2,%3,%1; seq %0"
                        : "=dm" (ret), "=m" (*p), "=d" (readval)
                        : "d" (newval), "m" (*p), "2" (oldval));
  return ret;
}

#endif /* atomicity.h */
