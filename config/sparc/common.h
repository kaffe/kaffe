/*
 * sparc/common.h
 * Common SPARC configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __sparc_common_h
#define __sparc_common_h

#define	sysdepCallMethod(CALL)						\
	asm volatile ("							\n\
		cmp %2,0						\n\
		be 1f							\n\
		nop							\n\
		ld [%2],%%o0						\n\
		ld [%2+8],%%o1						\n\
		ld [%2+16],%%o2						\n\
		ld [%2+24],%%o3						\n\
		ld [%2+32],%%o4						\n\
		ld [%2+40],%%o5						\n\
	1:								\n\
		call %1,0						\n\
		nop							\n\
		st %%o0,[%0]						\n\
	" :								\
	  : "r" ((CALL)->ret),						\
	    "r" ((CALL)->function),					\
	    "r" ((CALL)->args)						\
	  : "g1" , "g2", "g3", "g4", "o0", "o1", "o2", "o3", "o4", "o5", "o7" )

#endif
