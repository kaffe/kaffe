/*
 * mips/netbsd1/md.h
 * NetBSD1 MIPS configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __mips_netbsd1_md_h
#define __mips_netbsd1_md_h

/**/
/* Thread handling */
/**/
#define USE_INTERNAL_THREADS

#define THREADSTACKSIZE         (32 * 1024)

/*
 * Stack offset.
 * This is the offset into the setjmp buffer where the stack pointer is
 * stored.  This may be different with different OSes.
 * NetBSD/mips needs not to define FP_OFFSET.
 */
#define SP_OFFSET	32


/*
 * System dependent call method routine definition.
 * We use 'mips32CallMethod' (defined in mips.c) for NetBSD/mips.
 */
#define sysdepCallMethod(CALL) mips32CallMethod(CALL)

#endif
