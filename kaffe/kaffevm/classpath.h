/*
 * classpath.h
 *
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#ifndef __classpath_h
#define __classpath_h

#define IS_ZIP(B) \
	((B)[0] == 'P' && (B)[1] == 'K')
#define	IS_SOFILE(B) \
	((B)[0] == 0x7f && (B)[1] == 'E' && (B)[2] == 'L' && (B)[3] == 'F')

typedef struct _classpathEntry {
        int     type;
        char*   path;
        union {
                jarFile*        jar;
                struct {
                        int     loaded;
                } sof;
        } u;
        struct _classpathEntry* next;
} classpathEntry;

extern classpathEntry* classpath;

#endif
