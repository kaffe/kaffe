#ifndef __CLASSPATH_IEEEFP_H__
#define __CLASSPATH_IEEEFP_H__

#ifndef __IEEE_BIG_ENDIAN
#ifndef __IEEE_LITTLE_ENDIAN

#include "config.h"

#if defined(__arm__) || defined(__thumb__)
/* ARM traditionally used big-endian words; and within those words the
   byte ordering was big or little endian depending upon the target.
   Modern floating-point formats are naturally ordered; in this case
   __VFP_FP__ will be defined, even if soft-float.  */
#ifdef __VFP_FP__
#  ifdef __ARMEL__
#    define __IEEE_LITTLE_ENDIAN
#  else
#    define __IEEE_BIG_ENDIAN
#  endif
#else
#  define __IEEE_BIG_ENDIAN
#  ifdef __ARMEL__
#    define __IEEE_BYTES_LITTLE_ENDIAN
#  endif
#endif

#endif

#if !defined(__IEEE_BIG_ENDIAN) && !defined(__IEEE_LITTLE_ENDIAN)
#  if defined(WORDS_BIGENDIAN)
#   define __IEEE_BIG_ENDIAN
#  else
#   define __IEEE_LITTLE_ENDIAN
#  endif
#endif

#endif /* not __IEEE_LITTLE_ENDIAN */
#endif /* not __IEEE_BIG_ENDIAN */

#endif /* __CLASSPATH_IEEEFP_H__ */
