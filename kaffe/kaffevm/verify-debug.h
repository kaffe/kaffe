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

#ifdef KAFFE_VMDEBUG
extern void printInstruction(const int opcode);
extern void printType(const Type*);
extern void printBlock(const Method* method, const BlockInfo* binfo, const char* indent);
#endif
