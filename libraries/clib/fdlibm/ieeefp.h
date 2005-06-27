#ifndef __CLASSPATH_IEEEFP_H__
#define __CLASSPATH_IEEEFP_H__

#ifndef __IEEE_BIG_ENDIAN
#ifndef __IEEE_LITTLE_ENDIAN

#include "config.h"

#if defined(WORDS_BIGENDIAN)
# define __IEEE_BIG_ENDIAN
#else
# define __IEEE_LITTLE_ENDIAN
#endif

#endif /* not __IEEE_LITTLE_ENDIAN */
#endif /* not __IEEE_BIG_ENDIAN */

#endif /* __CLASSPATH_IEEEFP_H__ */
