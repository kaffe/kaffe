#ifndef __lock_impl_h
#define __lock_impl_h

#include "debug.h"

#define THREAD_SYSTEM_HAS_KSEM

typedef struct Ksem {
	sem_id sem;
} Ksem;

static inline void ksemInit(struct Ksem* sem)
{
	sem->sem = create_sem(0, "Ksem");
}

static inline void ksemPut(struct Ksem* sem)
{
	release_sem(sem->sem);
}

static inline jboolean ksemGet(struct Ksem* sem, jlong timeout)
{
	status_t rc = B_OK;
	bigtime_t timeoutAt = timeout*1000L + system_time();

	do {
		if (0 == timeout) {
			rc = acquire_sem(sem->sem);
		}
		else {
			rc = acquire_sem_etc(sem->sem, 1, B_ABSOLUTE_TIMEOUT, timeoutAt);
		}
	} while (B_INTERRUPTED == rc);
	return (B_OK == rc) ? true : false;
}

static inline void ksemDestroy(struct Ksem* sem)
{
	delete_sem(sem->sem);
}

#endif /* __lock_impl_h */
