/*
 * flags.c
 * Various configuration flags.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "flags.h"

/*
 * Specify which version of Java we can use.
 */
char* java_version = "1.1.5";
int java_major_version = 1;
int java_minor_verison = 1;

/*
 * Specify what and how bytecode should be verified.
 */
int flag_verify = 0;

/*
 * Specify whether the garbage collector is noisy.
 */
int flag_gc = 0;
/*
 * Specify where class loading in noisy.
 */
int flag_classload;

/*
 * Specify whether the JIT system (if in use) is noisy.
 */
int flag_jit = 0;
