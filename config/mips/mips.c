/*
 * mips/mips.c
 * MIPS specific functions.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef inline
# define inline
#endif

inline int
mipsCountFrames (void** pc, void** sp)
{
  printf ("Count frames from $pc=%p $sp=%p\n", pc, sp);
  return 2;
}
