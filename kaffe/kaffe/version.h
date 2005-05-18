/*
 * version.h
 *
 * Copyright (c) 2000  The University of Utah.  All rights Reserved.
 *
 * This file is distributed as is under the terms of the GNU General
 * Public License.
 */

#ifndef KAFFE_KAFFE_VERSION_H
#define KAFFE_KAFFE_VERSION_H

#define JAVA_VERSION_STRING	"1.4"

/*
 * Print copyright notice and simple version info (Java version, 
 * engine type, etc).  Prints to stderr.
 */
void printShortVersion(void);

/*
 * In addition to the short version, print ludicrous amounts of
 * information about the compile environment, configuration 
 * options, etc.
 */
void printFullVersion(void);

#endif /* KAFFE_KAFFE_VERSION_H */
