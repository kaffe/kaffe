/* Definitions for Motorola 68k running Linux-based GNU systems with
   ELF format.
   Copyright (C) 1995, 1996, 1997, 1998, 1999, 2000, 2002, 2003, 2004
   Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING.  If not, write to
the Free Software Foundation, 51 Franklin Street, Fifth Floor,
Boston, MA 02110-1301, USA.  */

#if !defined(CLEAR_CACHE_H)
#define CLEAR_CACHE_H

/* Clear the instruction cache from `beg' to `end'.  This makes an
   inline system call to SYS_cacheflush.  The arguments are as
   follows:

	cacheflush (addr, scope, cache, len)

   addr	  - the start address for the flush
   scope  - the scope of the flush (see the cpush insn)
   cache  - which cache to flush (see the cpush insn)
   len    - a factor relating to the number of flushes to perform:
	    len/16 lines, or len/4096 pages.  */

#define CLEAR_INSN_CACHE(BEG, END)					\
{									\
  register unsigned long _beg __asm ("%d1") = (unsigned long) (BEG);	\
  unsigned long _end = (unsigned long) (END);				\
  register unsigned long _len __asm ("%d4") = (_end - _beg + 32);	\
  __asm __volatile							\
    ("move%.l #123, %/d0\n\t"	/* system call nr */			\
     "move%.l #1, %/d2\n\t"	/* clear lines */			\
     "move%.l #3, %/d3\n\t"	/* insn+data caches */			\
     "trap #0"								\
     : /* no outputs */							\
     : "d" (_beg), "d" (_len)						\
     : "%d0", "%d2", "%d3");						\
}

#endif /* !defined(CLEAR_CACHE_H) */
