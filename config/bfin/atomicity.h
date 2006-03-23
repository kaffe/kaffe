/* Low-level functions for atomic operations.  ARM version.
   Copyright (C) 1997, 1998, 1999, 2000 Free Software Foundation, Inc.
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
   Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301 USA.  */

#ifndef _ATOMICITY_H
#define _ATOMICITY_H    1


/* Kaffe: Modified by Guilhem Lavaux to return the oldval */

static inline long int
__attribute__ ((unused))
compare_and_swap (volatile long int *p, long int oldval, long int newval)
{
  long int result, tmp;
  __asm__ ("\n"
           "0:\tldr\t%1,[%2]\n\t"
           "mov\t%0,#0\n\t"
           "cmp\t%1,%4\n\t"
           "bne\t1f\n\t"
           "swp\t%0,%3,[%2]\n\t"
           "cmp\t%1,%0\n\t"
           "swpne\t%1,%0,[%2]\n\t"
           "bne\t0b\n\t"
           "mov\t%0,%4\n"
	   "jmp\t2f\n\t"
           "1:"
	   "mov\t%0,%1\n\t"
	   "2:"
           : "=&r" (result), "=&r" (tmp)
           : "r" (p), "r" (newval), "r" (oldval)
           : "cc", "memory");
  return result;
}

#endif /* atomicity.h */

