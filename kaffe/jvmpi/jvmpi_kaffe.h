
#ifndef _JVMPI_KAFFE_H
#define _JVMPI_KAFFE_H

#include <jvmpi.h>

#include "lock-impl.h"
#include "support.h"
#include "code.h"

/*
 * Implementation of a JVMPI_RawMonitor.  Just uses jmutex/jcondvar to do the
 * work.
 */
struct _JVMPI_RawMonitor {
	jmutex mux;
	jcondvar cv;
	char *lock_name;
};

/*
 * Global data structure 
 */
typedef struct jvmpi_kaffe {
	int jk_EventMask[BITMAP_BYTE_SIZE(JVMPI_EVENT_COUNT)];
	JVMPI_Interface jk_Interface;
} jvmpi_kaffe_t;

#if defined(ENABLE_JVMPI)

extern jvmpi_kaffe_t jvmpi_data;

#define JVMPI_EVENT_ISENABLED(type) \
	BITMAP_ISSET(jvmpi_data.jk_EventMask, (type))

JVMPI_Interface *jvmpiCreateInterface(jint version);
void jvmpiPostEvent(JVMPI_Event *ev);

void jvmpiConvertField(JVMPI_Field *dst, fields *src);
void jvmpiConvertMethod(JVMPI_Method *dst, methods *src);
void jvmpiConvertLineno(JVMPI_Lineno *dst,
			lineNumberEntry *src,
			void *start_pc);

#else

#define JVMPI_EVENT_ISENABLED(type) 0
#define jvmpiCreateInterface(version) NULL
#define jvmpiPostEvent(ev)
#define jvmpiConvertField(dst, src)
#define jvmpiConvertMethod(dst, src)
#define jvmpiConvertLineno(dst, src, start_pc)

#endif

#endif
