/* 
 * m68k/nextstep3/md.h
 * NEXTSTEP m68k specific functions.   
 *
 * Copyright (c) 1996 Matthias Hopf <mshopf@informatik.uni-erlangen.de>
 * 
 * Permission granted for Tim Wilkinson to include this source in his
 * Kaffe system.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */
  
#ifndef __m68k_nextstep3_md_h
#define __m68k_nextstep3_md_h

#include "m68k/threads.h"
#include "m68k/sysdepCallMethod.h"

/*
 * Stack offset.
 * This is the offset into the setjmp buffer 
 * where the stack pointer is stored.
 */
#undef	SP_OFFSET
#define	SP_OFFSET		2

#if defined(TRANSLATOR)  
#include "jit-md.h"
#endif           

#endif  
