#ifndef __stackTrace_impl_h__
#define __stackTrace_impl_h__

#include "jit-md.h"

typedef struct _stackTrace {
        struct _exceptionFrame  nframe;
        struct _exceptionFrame* frame;
} stackTrace;

#define STACKTRACEINIT(S, I, O, R)                      \
        {                                               \
                if ((I) == NULL) {                      \
                        FIRSTFRAME((S).nframe, O);      \
                        (S).frame = &((S).nframe);      \
                } else {                                \
                        (S).frame = (I);                \
                }                                       \
                (R) = *(S).frame;                       \
        }

#ifdef STACK_NEXT_FRAME
#define STACKTRACESTEP(S)       (STACK_NEXT_FRAME((S).frame))
#else
#define STACKTRACESTEP(S)	((S).frame = (struct _exceptionFrame *)NEXTFRAME((S).frame))
#endif

#define STACKTRACEPC(S)         (PCFRAME((S).frame))
#define STACKTRACEFP(S)         (FPFRAME((S).frame))
#define STACKTRACEFRAME(S)	((S).frame)

#endif /* __stackTrace_impl_h__ */
