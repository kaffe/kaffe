/*
 * gcj.h
 *
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#ifndef __gcj_h
#define __gcj_h

/*
 * If we have the necessary infra-structure, then include GCJ support.
 */
#if defined(HAVE___THROW) && \
    defined(HAVE___TERMINATE_FUNC) && \
    defined(HAVE___GET_EH_INFO)

#define	HAVE_GCJ_SUPPORT 1

#endif

extern void gcjInit(void);

#endif
