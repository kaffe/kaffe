/*
 * verify-debug.h
 *
 * Copyright 2004
 *   Kaffe.org contributors. See ChangeLog for details. All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Interface to the debugging code for the verifier.
 */

#if !defined(VERIFY_DEBUG_H)
#define VERIFY_DEBUG_H

#include "classMethod.h"
#include "verify.h"
#include "verify-block.h"

#if defined(KAFFE_VMDEBUG)
extern const char* indent;
extern const char* indent2;

extern uint32 printConstantPoolEntry(const Hjava_lang_Class* class, uint32 idx);
extern void printConstantPool(const Hjava_lang_Class* class);
extern void printInstruction(const int opcode);
extern void printType(const Type*);
extern void printBlock(const Method* method, const BlockInfo* binfo, const char* id);
#endif /* defined(KAFFE_VMDEBUG) */

#endif /* !defined(VERIFY_DEBUG_H) */
