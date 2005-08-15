/* Wrapper file to include the right atomic.h file for ppc depending on
   the bit size.  */

#if defined(__ppc__) || defined(__powerpc__)
#include "powerpc32/atomic.h"
#elif defined(__ppc64__) || defined(__powerpc64__)
#include "powerpc64/atomic.h"
#else
#error Huh, what is defined ?
#endif
