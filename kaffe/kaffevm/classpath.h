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

#define CP_INVALID      0
#define CP_ZIPFILE      1
#define CP_DIR          2
#define CP_SOFILE       3

#define IS_ZIP(B)       ((B)[0] == 'P' && (B)[1] == 'K')

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
