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

#include <oskit/threads/pthread.h>

/* why is fs.h part of the minimal c library? */
#include <oskit/c/fs.h>

#include <oskit/fs/dir.h>
/* #include <oskit/fs/bmodfs.h> */
#include <oskit/net/socket.h>
#include <oskit/startup.h>
#include <oskit/dev/dev.h>
#include <oskit/dev/freebsd.h>
#include <oskit/dev/net.h>
#include <oskit/dev/linux.h>

#include <oskit/dev/osenv.h>
#include <oskit/com/services.h>

/* For DBGEXPR */
#include "debug.h"

/* For vmargs */
#include "jni.h"

#include <gtypes.h>
#include "external.h"

char *default_classpath = ".";


#if defined(OSKIT_UNIX) && defined(OSKIT_CONFIGURED)
#include "machine.h"

/*
 * Since the oskit bootstrapping procedure is infinitely less stable
 * than the basic com interfaces, it seems wise to use libstartup
 * as much as possible.  Since start_network and start_fs both call
 * start_devices, the easiest way to avoid pulling in all drivers is
 * to replace start_devices.
 */
void
start_devices()
{
	static int once = 0;

	if (!once) return;

	oskit_osenv_t  *env = start_osenv();
	
	osenv_process_lock();
	oskit_dev_init(env);
	oskit_linux_init_osenv(env);

	/* now, initialize all configured devices */
	{
	    	void (**init)() = init_vector;

		while (*init) (*init++)();
	}
	oskit_dev_probe();
	osenv_process_unlock();

}
#endif
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

	oskit_clientos_init();
	start_osenv();
#if !defined(OSKIT_UNIX)
	/*
	 * Real OSKIT kernel.
	 */
	start_world_pthreads();
#else
	/*
	 * Unix mode.
	 *
	 * XXX: pthread_init() is called here instead of in the thread
	 * init code since it needs to be done before all this goop is.
	 */
	pthread_init(1);
	
	start_clock();	
	start_fs_native_pthreads("/");
	start_network_native_pthreads();

	/*
	 * I guess I could reimpliment getcwd here using native_
	 * system calls, but why?
	 */
	cp = getenv("PWD");
	if (cp) chdir(cp);
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
	/*
	 * We need to be able to find .la files.  Provide a default
	 * KAFFE_LIBDIR.
	 */
	setenv(LIBRARYPATH, "/lib", 0);
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
