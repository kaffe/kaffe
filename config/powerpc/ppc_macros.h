/*
 * ppc_macros.h
 *
 * Copyright (c) 2002, 2004 The University of Utah and the Flux Group.
 * All rights reserved.
 *
 * @JANOSVM_KAFFE_JANOSVM_LICENSE@
 */
#ifndef _ppc_macros_h
#define _ppc_macros_h

/* Get the lower 16 bits of a 32 bit value. */
#define ppc_lo16(x) (((unsigned long)(x)) & 0xffff)
/*
 * Get the high 16 bits of a 32 bit value that is suitable for an unsigned
 * immediate value.  In another words, just the raw higher order bits.
 */
#define ppc_hi16(x) (((unsigned long)(x)) >> 16)
/*
 * Get the high 16 bits of a 32 bit value that is suitable for a signed
 * immediate.  In other words, if bit 15 of the lower order bits is set the
 * macro will return the higher order bits plus one to counteract the sign
 * extension done by an instruction.
 */
#define ppc_ha16(x) (ppc_lo16(x) & 0x8000 ? ppc_hi16(x) + 1 : ppc_hi16(x))
/* Convert a value to something usable for a branch displacement. */
#define ppc_bd16(x) ((x) >> 2)
#define ppc_b26(x) ((x) >> 2)

#endif
