/*
 * XProfiler.c
 * Java wrappers for the xprofiler functions
 *
 * Copyright (c) 2000 University of Utah and the Flux Group.
 * All rights reserved.
 *
 * This file is licensed under the terms of the GNU Public License.
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * Contributed by the Flux Research Group, Department of Computer Science,
 * University of Utah, http://www.cs.utah.edu/flux/
 */

#include "config.h"
#include "gtypes.h"
#include "../../../kaffe/xprof/xprofiler.h"
#include "kaffe_management_XProfiler.h"
#include "stringSupport.h"
#include <native.h>

void Java_kaffe_management_XProfiler_on(JNIEnv *env UNUSED, jclass clazz UNUSED)
{
#if defined(KAFFE_XPROFILER)
	xProfilingOn();
#endif
}

void Java_kaffe_management_XProfiler_off(JNIEnv *env UNUSED, jclass clazz UNUSED)
{
#if defined(KAFFE_XPROFILER)
	xProfilingOff();
#endif
}

#if defined(KAFFE_XPROFILER)
void Java_kaffe_management_XProfiler_stage(JNIEnv *env UNUSED, jclass clazz UNUSED,
					   jstring _stage_name)
{
	char *stage_name = stringJava2C(_stage_name);

	xProfileStage(stage_name);
	gc_free(stage_name);
}
#else
void Java_kaffe_management_XProfiler_stage(JNIEnv *env UNUSED, jclass clazz UNUSED,
					   jstring _stage_name UNUSED)
{
}
#endif
