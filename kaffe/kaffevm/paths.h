/*
 * path.h
 * Path support routines.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __paths_h
#define __paths_h

#if defined(__WIN32__) || defined(__OS2__)
#define	PATHSEP		';'
#define	DIRSEP		"\\"
#elif defined(__amigaos__)
#define PATHSEP		';'
#define DIRSEP		"/"
#else
#define	PATHSEP		':'
#define	DIRSEP		"/"
#endif

#endif
