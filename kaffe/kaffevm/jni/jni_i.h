#ifndef _kaffe_jni_i_h_
#define _kaffe_jni_i_h_

#include "config.h"
#include "jni_md.h"
#include "itypes.h"
#include "threadData.h"
#include "thread.h"
#include "exception.h"
#include "md.h"

extern JavaVM* KaffeJNI_GetKaffeVM(void);

#define KAFFE_VM_ENV(env) ((KaffeVM_Environment *) (((JNIEnv *)(env)) + 1))

/*
 * If we must manage the JNI references for the native layer then we
 * add extra functions to the JNI calls and returns to manage the
 * referencing.
 */
void KaffeJNI_addJNIref(jref);
void KaffeJNI_removeJNIref(jref);
#define	ADD_REF(O)		KaffeJNI_addJNIref(O)
#define	REMOVE_REF(O)		KaffeJNI_removeJNIref(O)

static inline jobject
unveil(jref w)
{
  uintp wp = (uintp) w;

  return ( (wp & 1) ? *((jobject *)(wp & ~(uintp)1)) : w);
}

/*
 * Define how to set the frame pointer in a VmExceptHandler.
 */
#if defined(TRANSLATOR)
#define KAFFE_JNI_SETEXCEPTFP(ebufp) {				\
	struct _exceptionFrame frame;				\
	FIRSTFRAME (frame, NULL);				\
	vmExcept_setJNIFrame(ebufp, FPFRAME(&frame));		\
        }
#else
/*
 * Stack frame info isn't needed (and isn't available) in the
 * interpreter (see dispatchException/unwindStackFrame in exception.c)
 * However, we have to at least tag the VmExceptHandler as
 * a JNIFrame so the stack trace code can ignore it.
 */
#define KAFFE_JNI_SETEXCEPTFP(ebufp) {   \
	vmExcept_setJNIFrame(ebufp, ebufp); \
        }
#endif 


/*
 * Define how we handle exceptions in JNI.
 *
 * Each BEGIN_EXCEPTION_HANDLING macro must be matched by an
 * END_EXCEPTION_HANDLING macro call in the same scope.  Each should
 * be used only once in a given JNI entrypoint.
 */
#define	BEGIN_EXCEPTION_HANDLING(X)			\
	VmExceptHandler ebuf;				\
	threadData *thread_data = THREAD_DATA();	\
	KAFFE_JNI_SETEXCEPTFP(&ebuf); 			\
	ebuf.prev = thread_data->exceptPtr;\
	if (JTHREAD_SETJMP(ebuf.jbuf) != 0) {		\
		thread_data->exceptPtr = ebuf.prev;	\
		return X;				\
	}						\
	thread_data->exceptPtr = &ebuf

#define	BEGIN_EXCEPTION_HANDLING_VOID()			\
	VmExceptHandler ebuf; 				\
	threadData *thread_data = THREAD_DATA();	\
	KAFFE_JNI_SETEXCEPTFP(&ebuf); 			\
	ebuf.prev = thread_data->exceptPtr;	\
	if (JTHREAD_SETJMP(ebuf.jbuf) != 0) {		\
		thread_data->exceptPtr = ebuf.prev; \
		return;					\
	}						\
	thread_data->exceptPtr = &ebuf

#define	END_EXCEPTION_HANDLING()			\
	thread_data->exceptPtr = ebuf.prev


#define	KNI_GET_FIELD(T,O,F)	*(T*)((char*)(O) + FIELD_BOFFSET((Field*)(F)))
#define	KNI_SET_FIELD(T,O,F,V)	*(T*)((char*)(O) + FIELD_BOFFSET((Field*)(F))) = (V)
#define	KNI_GET_STATIC_FIELD(T,F)	*(T*)FIELD_ADDRESS((Field*)F)
#define	KNI_SET_STATIC_FIELD(T,F,V)	*(T*)FIELD_ADDRESS((Field*)F) = (V)


#endif

