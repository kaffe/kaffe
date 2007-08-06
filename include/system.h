/*
 * system.h
 * Defines for this system.
 *
 * Copyright (c) 1996, 1997, 1998, 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2006
 *      Kaffe.org contributors. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#ifndef __system_h
#define __system_h

#if defined(unix) || defined(__unix__) || defined(__BEOS__) || defined(_AIX) || defined(__NetBSD__) || defined(__APPLE__) || defined(__uclinux__) || defined(__INTERIX)
#define	file_separator		"/"
#define	path_separator		":"
#define	line_separator		"\n"
#elif defined(_WIN32) || defined(__WIN32__) || defined(__OS2__)
#define	file_separator		"\\"
#define	path_separator		";"
#define	line_separator		"\r\n"
#elif defined(__amigaos__)
#define	file_separator		"/"
#define	path_separator		";"
#define	line_separator		"\n"
#elif defined(__riscos__)
#define	file_separator		"/"
#define	path_separator		","
#define	line_separator		"\n"
#else
#error "Separators undefined for this system"
#endif

#define	KAFFEHOME		"KAFFEHOME"

#endif
