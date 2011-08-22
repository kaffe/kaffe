/*
* tile/atomictile.h
* Some necessary macros and atomic.h
*
* Copyright (c) 2011 Francisco Javier Guzmán Jiménez <dejavih@gmail.com>. All rights reserved.
* See the file "license.terms" for information on usage and redistribution
* of this file.
*
* written by Francisco Javier Guzmán Jiménez
* <dejavih@gmail.com><dejavih@hotmail.es><javierguzman@alu.uma.es>
*
*/
#ifndef __tile_atomictile_h

#define __tile_atomictile_h
#define COMPARE_AND_EXCHANGE(A,O,N) (atomic_cmpxchg(A,N,O)==O)
#define atomic_decrement(mem) atomic_dec(mem)
#define atomic_increment(mem) atomic_inc(mem)

#include "atomic.h" /* atomic.h is a confidential file of Tilera, you must buy TileraMDE*/

#endif
