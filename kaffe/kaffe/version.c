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

extern char* engine_name;	/* defined in the engine's library */
extern char* engine_version;	/* defined in the engine's library */

static FILE* versionfd = stderr;

void
printShortVersion(void)
{
	fprintf(versionfd, "Kaffe Virtual Machine\n");
	fprintf(versionfd, "Copyright (c) 1996-2000\nTransvirtual Technologies, Inc.  All rights reserved\n");
	fprintf(versionfd, "Engine: %s   Version: %s   Java Version: %s\n",
		engine_name, engine_version, JAVA_VERSION_STRING);
}

void
printFullVersion(void)
{
	printShortVersion();
	fprintf(versionfd, "Configuration/Compilation options:\n");
	fprintf(versionfd, "  Compile date  : %s\n", VER_COMPILE_DATE);
	fprintf(versionfd, "  Compile host  : %s\n", VER_COMPILE_HOST);
	fprintf(versionfd, "  Install prefix: %s\n", VER_PREFIX);
	fprintf(versionfd, "  Thread system : %s\n", VER_THREAD_SYSTEM);
	fprintf(versionfd, "  CC            : %s\n", VER_CC);
	fprintf(versionfd, "  CFLAGS        : %s\n", VER_CFLAGS);
	fprintf(versionfd, "  LDFLAGS       : %s\n", VER_LDFLAGS);
	fprintf(versionfd, "  ChangeLog head: %s\n", VER_CHANGELOG_HEAD);
	// fprintf(versionfd, "  Libraries     : %s\n", VER_KAFFELIBS);
}

