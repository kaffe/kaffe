/*
 * m68k/common.h
 * Common M68K configuration information.
 *
 * Copyright (c) 1996, 1997, 1998, 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2003
 *      Kaffe.org contributors. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * Written by Kiyo Inaba <k-inaba@mxb.mesh.ne.jp>, 1998;
 * Based on the ports
 *      by Remi Perrot <r_perrot@mail.club-internet.fr> to m68k/linux
 * and
 *      by Alexandre Oliva <oliva@dcc.unicamp.br> to sparc
 *
 * Modified by Kiyo Inaba <k-inaba@mxb.mesh.ne.jp>, 1999 to support
 *	both linux and netbsd1.
 */

#ifndef __m68k_common_h
#define __m68k_common_h

#include "atomicity.h"

/*
 * Do an atomic compare and exchange.  The address 'A' is checked against
 * value 'O' and if they match it's exchanged with value 'N'.
 * We return '1' if the exchange is successful, otherwise 0.
 */

#define COMPARE_AND_EXCHANGE(A, O, N)  (compare_and_swap((long int*) A, (long int) O, (long int) N)) 

#endif
