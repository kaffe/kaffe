/*
 * system.h
 * Defines for this system.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __system_h
#define __system_h

#define	kaffe_version		KAFFEVERSION
#define	kaffe_vendor		"Transvirtual Technologies, Inc."
#define	kaffe_vendor_url	"http://www.transvirtual.com"
#define	kaffe_class_version	"1"

#if defined(unix)
#define	file_seperator		"/"
#define	path_seperator		":"
#define	line_seperator		"\n"
#elif defined(__WIN32__) || defined(__OS2__)
#define	file_seperator		"\\"
#define	path_seperator		";"
#define	line_seperator		"\r\n"
#elif defined(__amigaos__)
#define	file_seperator		"/"
#define	path_seperator		";"
#define	line_seperator		"\n"
#else
#error "Seperators undefined for this system"
#endif

#define	KAFFEHOME		"KAFFEHOME"
#define	KAFFECLASSPATH		"CLASSPATH"

#endif
