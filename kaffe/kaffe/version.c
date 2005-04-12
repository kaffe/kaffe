/*
 * version.c
 *
 * Copyright (c) 2000  The University of Utah.  All rights Reserved.
 *
 * This file is distributed as is under the terms of the GNU General
 * Public License.
 */

/* Print out the Kaffe version information.
 * This is in a separate file because the version-info.h header file is
 * re-generated for each compile, and this minimizes the dependencies.
 */
#include "config.h"
#include "config-std.h"
#include "version.h"
#include "version-info.h" /* generated at compile time */
#include "gc.h"
#include "md.h"

extern char* engine_name;	/* defined in the engine's library */

/* Must not be initialized in place, because stderr is not always a
 * compile-time constant. */
static FILE* versionfd /* = stderr */;

void
printShortVersion(void)
{
	if (!versionfd)
		versionfd = stderr;
	fprintf(versionfd, "%s\n\n", PACKAGE_NAME);
	fprintf(versionfd, "Copyright (c) 1996-2004 Kaffe.org project contributors (please see\n");
	fprintf(versionfd, "  the source code for a full list of contributors).  All rights reserved.\n");
	fprintf(versionfd, "Portions Copyright (c) 1996-2002 Transvirtual Technologies, Inc.\n\n");

	fprintf(versionfd, "The Kaffe virtual machine is free software, licensed under the terms of\n");
	fprintf(versionfd, "the GNU General Public License.  Kaffe.org is a an independent, free software\n");
	fprintf(versionfd, "community project, not directly affiliated with Transvirtual Technologies,\n");
	fprintf(versionfd, "Inc.  Kaffe is a Trademark of Transvirtual Technologies, Inc.  Kaffe comes\n");
	fprintf(versionfd, "with ABSOLUTELY NO WARRANTY.\n\n");

	fprintf(versionfd, "Engine: %s   Version: %s   Java Version: %s\n",
		engine_name, PACKAGE_VERSION, JAVA_VERSION_STRING);
	fprintf(versionfd, "Heap defaults: minimum size: %d MB, maximum size: %d MB\n",
		MIN_HEAPSIZE / (1024*1024), MAX_HEAPSIZE / (1024*1024));
	fprintf(versionfd, "Stack default size: %d KB\n", THREADSTACKSIZE / 1024);
}

void
printFullVersion(void)
{
	printShortVersion();
	fprintf(versionfd, "Configuration/Compilation options:\n");
	fprintf(versionfd, "  Compile date     : %s\n", VER_COMPILE_DATE);
	fprintf(versionfd, "  Compile host     : %s\n", VER_COMPILE_HOST);
	fprintf(versionfd, "  Install prefix   : %s\n", VER_PREFIX);
	fprintf(versionfd, "  Thread system    : %s\n", VER_THREAD_SYSTEM);
	fprintf(versionfd, "  Garbage Collector: %s\n", VER_GARBAGE_COLLECTOR);
	fprintf(versionfd, "  CC               : %s\n", VER_CC);
	fprintf(versionfd, "  CFLAGS           : %s\n", VER_CFLAGS);
	fprintf(versionfd, "  LDFLAGS          : %s\n", VER_LDFLAGS);
	fprintf(versionfd, "  ChangeLog head   : %s\n", VER_CHANGELOG_HEAD);
	/* fprintf(versionfd, "  Libraries     : %s\n", VER_KAFFELIBS); */
}

