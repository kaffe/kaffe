/*
 * i386/oskit/md.c
 * i386 OSKit specific startup and intialization.
 * 
 * Copyright (c) 1998, 1999 The University of Utah. All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Contributed by the Flux Research Group at the University of Utah.
 * Authors: Godmar Back, Leigh Stoller
 */

/* XXX use the "config-foo.h" header files instead? */
#include <unistd.h>
#include <syslog.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <signal.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <oskit/threads/pthread.h>

/* why is fs.h part of the minimal c library? */
#include <oskit/c/fs.h>

#include <oskit/fs/dir.h>
#include <oskit/dev/freebsd.h>
#include <oskit/net/freebsd.h>
#include <oskit/net/socket.h>
#include <oskit/dev/dev.h>
#include <oskit/dev/freebsd.h>
#include <oskit/dev/net.h>
#include <oskit/dev/linux.h>

#include <oskit/dev/osenv.h>
#include <oskit/com/services.h>

/* For vmargs */
#include "jni.h"

#include <gtypes.h>
#include "external.h"

void get_fake_commandline(int *pargc, char ***pargv);

char *default_classpath = ".";

#if defined(OSKIT_UNIX) && defined(OSKIT_CONFIGURED)
#include "machine.h"

/*
 * Since the oskit bootstrapping procedure is infinitely less stable
 * than the basic com interfaces, it seems wise to use libstartup
 * as much as possible.  Since start_network and start_fs both call
 * start_devices, the easiest way to avoid pulling in all drivers is
 * to replace start_devices.
 *
 * Only done for Unix emulation OSKIT.
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


#ifdef OSKIT_UNIX
static void start_unix_oskit(void)
{
	char *cp;

	startup_atexit_init();
	start_clock();	

	pthread_init(1);
	
	start_fs_native_pthreads("/");
	// printf("  Native FS hack started.\n");

	start_network_native_pthreads();
	// printf("  Native Network hack started.\n");

	/*
	 * I guess I could reimplement getcwd here using native_
	 * system calls, but why?
	 */
	cp = getenv("PWD");
	if (cp)
		chdir(cp);
}
#endif

#ifndef OSKIT_UNIX
static void start_real_oskit(int *pargc, char ***pargv)
{
	startup_atexit_init();
	start_clock();

	start_pthreads();

	/*
	 * Initialize and probe device drivers.
	 */
	osenv_process_lock();
	start_devices();
	osenv_process_unlock();
	
	/*
	 * Set up the bmod filesystem.
	 */
	start_fs_bmod_pthreads();

	/*
	 * This is a local hack to read a "command-line" for
	 * Kaffe on systems with broken-ass boot loaders.
	 *
	 * XXX Do *after* the FS initialization (so we can
	 * read paramters off it!) and before the network
	 * initialization (so we can configure it!).
	 *
	 * I know its gross.  Bite me.
	 */
	if (*pargc == 1)
		get_fake_commandline(pargc, pargv);
	
/* libstartup was changed a lot post 990722. */
#ifdef newer_than_990722
	/*
	 * Start network router stuff
	 */
	start_network_router_pthreads();

	/* XXX do I still need this?? */
	oskit_freebsd_net_ipfw_init();
#else
	start_network_pthreads();
#endif	

}
#endif


/*
 * The entrypoint for OSKit Kaffe.  This is called by Kaffe's main
 * (via the MAIN_MD macro).  This is invoked before *anything* else is
 * done by Kaffe's main.
 */
void 
oskit_kaffe_main(int *pargc, char ***pargv)
{
	char		*cp, buf[BUFSIZ];
	FILE		*fp;
	int		c;

	oskit_clientos_init_pthreads();
	start_osenv();

#if defined(OSKIT_UNIX)
	start_unix_oskit();
#else
	start_real_oskit(pargc, pargv);
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

		if ((default_classpath = (char *) malloc(c + 1)) == NULL)
			panic("oskit_kaffe_main: No memory for default_classpath.\n");

		strcpy(default_classpath, buf);
		fclose(fp);

		/*
		 * Kaffe will get the classpath because we define
		 * DEFAULT_CLASSPATH to be default_classpath.
		 * See kaffe/kaffe/main.c
		 */
	}

	/*
	 * We need to be able to find .la files.  Provide a default
	 * LIBRARYPATH.  LIBRARYPATH is Kaffe #define that is the name
	 * of the library search path environment variable.
	 */
	setenv(LIBRARYPATH, "/lib", 0);

#ifdef GPROF
	/*
	 * Start gprof'ing after most setup is completed...
	 */
	start_gprof((*pargv)[0]);  /* from liboskit_startup.a */
#endif
}

static void
abort_with_panic(void)
{
	panic("abort_with_panic");
}

/*
 * Invoked via the INIT_MD macro  (XXX from where??)
 */
void
oskit_kaffe_init_md(void)
{
	extern JavaVMInitArgs vmargs;

	vmargs.abort = (void*)&abort_with_panic;
}

/*
 * Get a line of input from the console device. Includes 
 * XXX This does cooking, echo, backspace, linekill.
 *
 * XXX stolen from the all-powerful NetBooty!  If you
 * want to write a kernel, NetBooty's got some code for you!
 */
char *
getline(char *str, int size)
{
	int i;
	int c;

	if (size <= 0)
		return NULL;

	i = 0;
	while (i < size) {
		c = getchar();
		if (c == EOF) {
			if (i == 0)
				return NULL;
			break;
		}
		if (c == '\r')
			c = '\n';
		else if (c == '\b') {
			if (i > 0) {
				putchar(c);
				putchar(' ');
				putchar(c);
				i--;
			}
			continue;
		}
		else if (c == '\025') {		/* ^U -- kill line */
			while (i) {
				putchar('\b');
				putchar(' ');
				putchar('\b');
				i--;
			}
			str[0] = '\0';
			continue;
		}
		putchar(c);
		str[i++] = c;
		if (c == '\n')
			break;
	}
	if (i < size)
		str[i] = '\0';
	return str;
}


void get_fake_commandline(int *pargc, char ***pargv)
{
	extern char **environ;  /* global environment ptr. */
	char *buf;
	char *cmdlinef;
	FILE *cmdfp;

	/* malloc (and don't free it) because argv will point into it. */
	buf = (char*)malloc((sizeof(char)) * 512);
	assert(buf);

	assert(environ != NULL);
	assert(environ[0] == NULL);

	/* This function is only invoked if no real args were given to Kaffe. */
	assert(*pargc == 1);
	assert((*pargv)[0] != NULL);
	

	/*
	 * Get the command line.  First, look for a file.
	 */
	if ((cmdlinef = getenv("KAFFE_CMDLINE")) == NULL)
		cmdlinef = "/etc/kaffe_cmdline";

	if ((cmdfp = fopen(cmdlinef, "r")) != NULL) {
		/* XXX verbosity. */
		printf("No command line supplied.  Using contents of %s for command line.\n",
		       cmdlinef);

		fgets(buf, 512, cmdfp);
	} else {
		/* XXX verbosity. */
		printf("No command line supplied.\n"
		       "No command line hack file (%s).\n"
		       "Syntax of command line is similar as for NetBoot:\n"
		       "   Arguments before a '--' are interpreted as environment settings.\n"
		       "   Arguments after a '--' (or all if no '--') are command-line arguments.\n",
		       cmdlinef);
		
		printf("> ");

		/*
		 * XXX stole this from NetBoot!
		 */
		getline(buf, 512);
	}

	{
		char *toks[strlen(buf)];
		char *tok;
		char **argv;
		int ntoks;
		int sepIndex = -1; /* index of separator, -1 if none */
		int i;
		
		/* Tokenize the input. */
		ntoks = 0;
		for (tok = strtok(buf, " \t\n"); tok; tok = strtok(NULL, " \t\n"))
		{
			assert(tok);

			/* Keep track of the first '--' token. */
			if ((strcmp(tok, "--") == 0)
			    && (sepIndex == -1))
				sepIndex = ntoks;

			/* Only add interesting tokens. */
			if (tok[0] != '\0')
				toks[ntoks++] = tok;
		}

		/*
		 * Handle environment variables, if any
		 */

		if ((sepIndex != -1) && (sepIndex != 0))
		{
			int i;
			int envct;
			
			assert(sepIndex > 0);
			envct = sepIndex;

			/* Set the global environment pointer */
			environ = (char**)malloc(sizeof(char*)*(envct+1));
			assert(environ);

			for (i = 0; i < envct; i++)
			{
				/* printf("\tadding %s to env\n", toks[i]); */
				environ[i] = toks[i];
			}
			
			environ[envct] = NULL;
		}

		/*
		 * Handle command line args, if any
		 */

		/* sepIndex + 1 is zero (no sep) or just after sep */
		assert(sepIndex+1 >= 0);
		assert(sepIndex+1 <= ntoks);

		/*
		 * *pargc is number of tokens after the sep, plus
		 * one for the kernel name (argv[0]).
		 */
		*pargc = ntoks - (sepIndex + 1) + 1;
		/* printf("\tadding %d arguments\n", *pargc); */

		argv = (char**)malloc(sizeof(char*) * (1 + (*pargc)));
		assert(argv);

		argv[0] = (*pargv)[0];  /* Keep kernel name */

		/* Copy args into argv array. */
		for (i = (sepIndex + 1); i < ntoks; i++)
		{
			int idx = i - (sepIndex + 1) + 1;
			/* printf("\tadding %s to command line @%d.\n", toks[i], idx); */
			argv[idx] = toks[i];
		}

		*pargv = argv;
	}
}

