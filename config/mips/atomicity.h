/* Low-level functions for atomic operations. Mips version.
   Copyright (C) 2001, 2002 Free Software Foundation, Inc.
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

#ifndef _MIPS_ATOMICITY_H
#define _MIPS_ATOMICITY_H    1

#include <inttypes.h>

static inline int
__attribute__ ((unused))
compare_and_swap (volatile long int *p, long int oldval, long int newval)
{
  long int ret, temp;

  __asm__ __volatile__
    ("1:\n\t"
     ".set	push\n\t"
     ".set	mips2\n\t"
     "ll	%1,%5\n\t"
     "move	%0,$0\n\t"
     "bne	%1,%3,2f\n\t"
     "move	%0,%4\n\t"
     "sc	%0,%2\n\t"
     ".set	pop\n\t"
     "beqz	%0,1b\n"
     "2:\n\t"
     : "=&r" (ret), "=&r" (temp), "=m" (*p)
     : "r" (oldval), "r" (newval), "m" (*p)
     : "memory");

  return ret;
}

#endif /* atomicity.h */
