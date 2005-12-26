/* Wrapper file to include the right atomic.h file for ppc depending on
   the bit size.  */

/* this is imperfect possibly. On an old 64bit POWER/AIX system what happens ? */

#if defined(__ppc__) || defined(__powerpc__) || defined(_POWER)
#include "powerpc32/atomic.h"
#elif defined(__ppc64__) || defined(__powerpc64__)
#include "powerpc64/atomic.h"
#else
#error Huh, what is defined ?
#endif
