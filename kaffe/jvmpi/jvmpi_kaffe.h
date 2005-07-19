/*
 * jvmpi_kaffe.h
 * Routines for generating an assembly file with debugging information
 *
 * Copyright (c) 2003 University of Utah and the Flux Group.
 * All rights reserved.
 *
 * This file is licensed under the terms of the GNU Public License.
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * Contributed by the Flux Research Group, Department of Computer Science,
 * University of Utah, http://www.cs.utah.edu/flux/
 */

#ifndef _JVMPI_KAFFE_H
#define _JVMPI_KAFFE_H

#include <jvmpi.h>

#include "classMethod.h"
#include "lock-impl.h"
#include "support.h"
#include "code.h"
#include "locks.h"

/**
 * Implementation of a JVMPI_RawMonitor.  We use internal kaffe reentrant fast lock.
 */
struct _JVMPI_RawMonitor {
        iStaticLock monitor;
	char *lock_name;
};

/*
 * Global structure for JVMPI data.
 *
 * jk_EventMask - Bitmask that indicates which events are currently active.
 * jk_Interface - The interface to the JVMPI functions.
 */
typedef struct jvmpi_kaffe {
	int jk_EventMask[BITMAP_BYTE_SIZE(JVMPI_EVENT_COUNT)];
	JVMPI_Interface jk_Interface;
} jvmpi_kaffe_t;

#if defined(ENABLE_JVMPI)

/**
 * Global JVMPI data.
 */
extern jvmpi_kaffe_t jvmpi_data;

/**
 * Test if a JVMPI event is enabled.
 *
 * @param type The event identifier.
 * @return True if the event has been enabled using EnableEvent() function in
 * the interface, false otherwise.
 */
#define JVMPI_EVENT_ISENABLED(type) \
	BITMAP_ISSET(jvmpi_data.jk_EventMask, (type))

/**
 * Initialize and return the JVMPI interface.
 *
 * @param version The requested version of the interface.
 * @return An initialized JVMPI_Interface object of the corresponding version.
 */
JVMPI_Interface *jvmpiCreateInterface(jint version);

/**
 * Post a JVMPI event by filling in the 'env_id' field, optionally disabling
 * the GC, and calling the 'NotifyEvent' function in the JVMPI_Interface.
 *
 * @param ev The event to post, the event_type field and any event specific
 * data should already be filled out.
 */
void jvmpiPostEvent(JVMPI_Event *ev);

/**
 * Convert a Kaffe field structure to a JVMPI_Field.
 *
 * @param dst The destination object.
 * @param src The source object.
 */
void jvmpiConvertField(JVMPI_Field *dst, fields *src);

/**
 * Convert a Kaffe method structure to a JVMPI_Method.
 *
 * @param dst The destination object.
 * @param src The source object.
 */
void jvmpiConvertMethod(JVMPI_Method *dst, methods *src);

/**
 * Convert a Kaffe lineNumberEntry structure to a JVMPI_Lineno.
 *
 * @param dst The destination object.
 * @param src The source object.
 * @param start_pc The starting address of the method.
 */
void jvmpiConvertLineno(JVMPI_Lineno *dst,
			lineNumberEntry *src,
			void *start_pc);

/**
 * Fill in a JVMPI_Event structure with the data for a JVMPI_EVENT_OBJECT_ALLOC
 * event.
 *
 * @param ev The object to fill out.
 * @param obj The object to describe in the event.
 */
void jvmpiFillObjectAlloc(JVMPI_Event *ev, struct Hjava_lang_Object *obj);

/**
 * Fill in a JVMPI_Event structure with the data for a JVMPI_EVENT_THREAD_START
 * event.  Note:  This function will KMALLOC the thread_start.parent_name,
 * thread_start.group_name, and thread_start.thread_name fields.
 *
 * @param ev The object to fill out.
 * @param obj The object to describe in the event.
 */
void jvmpiFillThreadStart(JVMPI_Event *ev, struct Hjava_lang_VMThread *tid);

/**
 * Free all requested memory by jvmpiFillThreadStart. This must be called after the
 * event has been posted to the profiler.
 *
 * @param ev The event object to cleanup. 
 */
void jvmpiCleanupThreadStart(JVMPI_Event *ev);

/**
 * Fill in a JVMPI_Event structure with the data for a JVMPI_EVENT_CLASS_LOAD
 * event.  Note:  The class_load.methods, class_load.statics, and
 * class_load.instances arrays must be allocated before calling this method.
 *
 * @param ev The object to fill out.
 * @param obj The object to describe in the event.
 */
void jvmpiFillClassLoad(JVMPI_Event *ev, struct Hjava_lang_Class *cl);

/**
 * Fill in a JVMPI_Event structure with the data for a JVMPI_EVENT_METHOD_LOAD
 * event.
 *
 * @param ev The event data structure to be filled.
 * @param meth The method which will be used for filling.
 */
void jvmpiFillMethodLoad(JVMPI_Event *ev, Method *meth);

#else

#define JVMPI_EVENT_ISENABLED(type) 0
#define jvmpiCreateInterface(version) NULL
#define jvmpiPostEvent(ev)
#define jvmpiConvertField(dst, src)
#define jvmpiConvertMethod(dst, src)
#define jvmpiConvertLineno(dst, src, start_pc)
#define jvmpiFillObjectAlloc(ev, obj)
#define jvmpiFillThreadStart(ev, obj)
#define jvmpiFillClassLoad(ev, obj)

#endif

#endif
