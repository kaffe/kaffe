/*
 * parisc/common.h
 * Common configuration information for PA-RISC based machines.
 *
 * Copyright (c) 1996, 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2003
 *      Kaffe.org contributors. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * sysdepCallMethod is written by Pavel Roskin <pavel_roskin@geocities.com>
 */

#ifndef __parisc_common_h
#define __parisc_common_h

#define NEED_STACK_ALIGN
#define STACK_ALIGN(p)  ((((unsigned long)(p)) & 15) ^ (unsigned long)(p))

/*
 * Do an atomic compare and exchange.  The address 'A' is checked against
 * value 'O' and if they match it's exchanged with value 'N'.
 * We return '1' if the exchange is sucessful, otherwise 0.
 *
 * pa doesn't have an atomic compare-and-exchange instruction. we use
 * a C version a la MIPS for now. Might not be SMP safe.
 */
#define COMPARE_AND_EXCHANGE(A,O,N)            \
({                                             \
    int ret = 0;                               \
    KTHREAD(suspendall)();                      \
                                               \
    if (*(A) == (O)) {                         \
       *(A) = (N);                             \
       ret = 1;                                \
    }                                          \
    KTHREAD(unsuspendall)();                    \
    ret;                                       \
})

#endif
