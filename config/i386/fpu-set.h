/*
 * config/i386/fpu-set.h
 * Setup the CPU in double mode.
 *
 * Copyright(c) 2005
 *    The Kaffe.org's developers. All rights reserved.
 *    See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
#ifndef __FPU_SET_H
#define __FPU_SET_H

static inline void
set_fpu_precision(void)
{
  unsigned int fpu_control;

  __asm__ __volatile__
    (
     "fnstcw %0\n\t"           /* Get the FPU state */
     "mov %0, %%ax\n\t"
     "and $0xfc, %%ah\n\t"     /* Remove the extended mode flag */
     "or $0x2, %%ah\n\t"       /* Put the double mode flag */
     "mov %%ax, %0\n\t"        /* Put back the new flag in memory */
     "fldcw %0\n\t"            /* and setup the FPU */
     : "+m" (fpu_control)
     : 
     : "eax");
}

#endif
