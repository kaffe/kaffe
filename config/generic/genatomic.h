/*
 * config/genatomic.h
 * Generic code for atomic operations.
 *
 * Copyright (c) 2005
 *       Kaffe.org contributors. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

/* This file included by XXXX/common.h if necessary. */

/*
 * Do an atomic compare and exchange.  The address 'A' is checked against
 * value 'O' and if they match it's exchanged with value 'N'.
 * We return '1' if the exchange is sucessful, otherwise 0.
 */
#define atomic_compare_and_exchange_val_acq(A, N, O) \
({						\
    typeof(*A) ret = 0;				\
    KTHREAD(suspendall)();			\
						\
    ret = *(A) ;				\
    if (ret == (O)) {				\
	*(A) = (N);				\
    }						\
    KTHREAD(unsuspendall)();			\
    ret;					\
})
