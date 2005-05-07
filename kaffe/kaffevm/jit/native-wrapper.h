/*
 * native-wrapper.c
 * Java Native Interface - JNI wrapper builder.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2004
 *      The kaffe.org's developers. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */


#ifndef __NATIVE_WRAPPER_H__
#define __NATIVE_WRAPPER_H__

void
engine_create_wrapper (Method *method, void *func);

#endif /* __NATIVE_WRAPPER_H__ */
