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

#include "jni_i.h"
#include "constpool.h"
#include "seq.h"
#include "slots.h"
#include "registers.h"
#include "labels.h"
#include "codeproto.h"
#include "basecode.h"
#include "icode.h"
#include "machine.h"
#include "../jit/native-wrapper.c"
