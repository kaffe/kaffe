/*
 * system.h
 * Defines for this system.
 *
 * Copyright (c) 1996, 1997, 1998, 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#ifndef __system_h
#define __system_h

#define	kaffe_version			"1.2"
#define	kaffe_vendor			"Kaffe.org project"
#define	kaffe_vendor_url		"http://www.kaffe.org"
#define	kaffe_vendor_url_bug		"http://www.kaffe.org"

#define	kaffe_vm_specification_version	"1.0"
#define	kaffe_vm_specification_vendor	"Sun Microsystems Inc."
#define	kaffe_vm_specification_name	"Java Virtual Machine Specification"
#define	kaffe_vm_version		PACKAGE_VERSION
#define	kaffe_vm_vendor			kaffe_vendor
#define	kaffe_vm_name			"Kaffe"
#define	kaffe_specification_version	"1.2"
#define	kaffe_specification_vendor	"Sun Microsystems Inc."
#define	kaffe_specification_name	"Java Platform API Specification"
#define	kaffe_class_version		"46.0"

#if defined(unix) || defined(__unix__) || defined(__BEOS__) || defined(_AIX) || defined(__NetBSD__) || defined(__APPLE__)
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
#define	KAFFECLASSPATH		"CLASSPATH"

#endif
