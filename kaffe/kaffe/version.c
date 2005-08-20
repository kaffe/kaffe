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

#if defined(HAVE_GETTEXT)
#include <libintl.h>
#define _(T) gettext(T)
#else
#define _(T) (T)
#endif
#if defined(HAVE_LC_MESSAGES)
#include <locale.h>
#endif


extern char* engine_name;	/* defined in the engine's library */

/* Must not be initialized in place, because stderr is not always a
 * compile-time constant. */
static FILE* versionfd /* = stderr */;

void
printShortVersion(void)
{
	if (!versionfd)
		versionfd = stderr;
	fprintf(versionfd, "java full version \"kaffe-1.4.2\"\n\n");
	fprintf(versionfd, "%s VM \"%s\"\n\n", PACKAGE, PACKAGE_VERSION);
	fprintf(versionfd,
		       _("Copyright (c) 1996-2005 Kaffe.org project contributors (please see\n"
			"  the source code for a full list of contributors).  All rights reserved.\n"
			"Portions Copyright (c) 1996-2002 Transvirtual Technologies, Inc.\n\n"
			"The Kaffe virtual machine is free software, licensed under the terms of\n"
			"the GNU General Public License.  Kaffe.org is a an independent, free software\n"
			"community project, not directly affiliated with Transvirtual Technologies,\n"
			"Inc.  Kaffe is a Trademark of Transvirtual Technologies, Inc.  Kaffe comes\n"
			"with ABSOLUTELY NO WARRANTY.\n\n"));

	fprintf(versionfd, _("Engine: %s   Version: %s   Java Version: %s\n"),
		engine_name, PACKAGE_VERSION, JAVA_VERSION_STRING);
	if (MAX_HEAPSIZE == UNLIMITED_HEAP) {
		fprintf(versionfd, _("Heap defaults: minimum size: %d MB, maximum size: unlimited\n"),
			MIN_HEAPSIZE / (1024*1024)); 
	} else {
		fprintf(versionfd, _("Heap defaults: minimum size: %d MB, maximum size: %lu MB\n"),
			MIN_HEAPSIZE / (1024*1024), MAX_HEAPSIZE / (1024*1024));
	}
	fprintf(versionfd, _("Stack default size: %d KB\n"), THREADSTACKSIZE / 1024);
}

void
printFullVersion(void)
{
	printShortVersion();
	fprintf(versionfd, "Configuration/Compilation options:\n)");
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

