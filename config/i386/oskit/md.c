/*
 * Copyright (c) 1998 The University of Utah. All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Contributed by the Flux Research Group at the University of Utah.
 * Authors: Godmar Back, Leigh Stoller
 */

#include <unistd.h>
#include <syslog.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <signal.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>

#include <fs.h>
#include <oskit/fs/dir.h>
#include <oskit/fs/bmodfs.h>
#include <oskit/net/socket.h>
#include <oskit/startup.h>
#include <oskit/dev/dev.h>
#include <oskit/dev/freebsd.h>
#include <oskit/dev/net.h>
#include <oskit/dev/linux.h>

/* For DBGEXPR */
#include "debug.h"

/* For vmargs */
#include "jni.h"

char *default_classpath = ".";

/*
 * since the boot people can't manage to give me a clean command line,
 * I will look for these switches at the end of the line and
 * cut them off: -h -f -retaddr
 */
void 
oskit_kaffe_clean_cmdline(int *pargc, char ***pargv)
{
	char		*cp, buf[BUFSIZ];
	FILE		*fp;
	int		c;
	
	char **argv = *pargv;
	while (*argv && strcmp(*argv, "-h") && strcmp(*argv, "-retaddr") &&
		strcmp(*argv, "-f"))
			argv++;

	/* eradicate them */
	while (*argv && **argv) {
		(*pargc)--;
		*argv++ = 0;
	}

	/*
	 * XXX: pthread_init() is called here instead of in the thread
	 * init code since it needs to be done before all this goop is.
	 */
	pthread_init(1);
	
	start_clock();
#ifndef OSKIT_UNIX
	/*
	 * Real OSKIT kernel.
	 */
	fs_init(oskit_bmod_init());
	osenv_process_lock();
	oskit_linux_init_devs();
	oskit_dev_probe();
        oskit_dump_devices();
	osenv_process_unlock();
	start_network_pthreads();
#else
	/*
	 * Unix mode.
	 */
	start_fs_native_pthreads("/");
	start_network_native_pthreads();
#endif
	/*
	 * Read in the default classpath file.
	 */
        if ((cp = getenv("CLASSPATHFILE")) == NULL)
		cp = "/etc/kaffe_classpath";
	
	if ((fp = fopen(cp, "r")) != NULL) {
		fgets(buf, BUFSIZ, fp);
		c = strlen(buf);
		if (buf[c - 1] == '\n')
			buf[c - 1] = NULL;

		printf("CLASSPATH = %s\n", buf);

		if ((default_classpath = (char *) malloc(c + 1)) == NULL)
			panic("oskit_kaffe_clean_cmdline: No memory\n");

		strcpy(default_classpath, buf);
		fclose(fp);
	}
}

void
oskit_kaffe_init_md(void)
{
	extern JavaVMInitArgs vmargs;
	void abort_with_panic(void);

	vmargs.abort = (void*)&abort_with_panic;
}

void
abort_with_panic(void)
{
	panic("abort_with_panic");
}
