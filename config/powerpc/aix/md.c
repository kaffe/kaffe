/*
 * powerpc/aix/md.c
 * AIX powerpc specific functions.
 *
 * Copyright (c) 1996 Sandpiper Software Consulting, LLC
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * Written by Rick Greer <rick@sandpiper.com>, 1996.
 * Used by Brad Harvey <b_harvey@hitachi.com>, 1997 for AIX port.
 */

void
thread_dummy(char *s, ...)
{   /*
     *  Dummy volatile function ...
     */

    volatile int n;
    n = 0;
}
