
#ifndef __stackTrace_impl_h__
#define __stackTrace_impl_h__

#include "../exception.h"

typedef struct _stackTrace {
        VmExceptHandler* frame;
} stackTrace;

/* Dummy exceptionFrame */
struct _exceptionFrame {
        char    dummy;
};

#define STACKTRACEINIT(S,I,O,R) ((S).frame = KTHREAD(get_data)(KTHREAD(current)())->exceptPtr)
#define STACKTRACESTEP(S)       ((S).frame = (S).frame->prev)

#define STACKTRACEPC(S)         (vmExcept_isJNIFrame((S).frame)?0:vmExcept_getPC((S).frame))
#define STACKTRACEFP(S)         ((uintp)(S).frame)

#define STACKTRACEFRAME(S)	((struct _exceptionFrame *)(S).frame)

/* XXX Fix to work with jvmpi */
#define EXCEPTIONFRAME(f, c)
#define FIRSTFRAME(f, c)        /* Does nothing */

#endif /* __stackTrace_impl_h__ */
