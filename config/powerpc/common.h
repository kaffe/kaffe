/*
 * powerpc/common.h
 * Common PowerPC configuration information.
 *
 */

#ifndef __powerpc_common_h
#define __powerpc_common_h

#define NEED_STACK_ALIGN
#define STACK_ALIGN(p)  ((((unsigned long)(p)) & 15) ^ (unsigned long)(p))
 
#if defined (__linux__) && defined(__powerpc__)
#if NEED_sysdepCallMethod
#include "callmethod_ppc.h"
#define	sysdepCallMethod(CALL) sysdepCallMethod_ppc(CALL)
#endif

#endif

#endif




