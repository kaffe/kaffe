/*
 * main.c
 * Kick off program.
 *
 * Copyright (c) 1996, 1997, 1998
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "classMethod.h"
#include "jtypes.h"
#include "native.h"
#include "constants.h"
#include "support.h"
#include "errors.h"
#include "thread.h"

/* HACK - we cannot use the internal VM malloc system at this point so
 * turn it off.  We should fix this properly.
 */
#undef	malloc

extern char* engine_name;
extern char* engine_version;
extern char* java_version;

extern void initialiseKaffe(void);

extern char* realClassPath;
extern int threadStackSize;
extern size_t gc_heap_limit;
extern int flag_verify;
extern int flag_gc;
extern int flag_classload;
extern int flag_jit;
extern size_t gc_heap_allocation_size;

static int options(char**);
static void usage(void);
static size_t parseSize(char*);
static void handleErrors(void);

#define	CLASSPATH1	"KAFFE_CLASSPATH"
#define	CLASSPATH2	"CLASSPATH"

/*
 * MAIN
 */
int
main(int argc, char* argv[])
{
	HArrayOfObject* args;
	Hjava_lang_String** str;
	int i;
	int farg;
	char* cp;

	/* Get classpath from environment */
	cp = getenv(CLASSPATH1);
	if (cp == 0) {
		cp = getenv(CLASSPATH2);
		if (cp == 0) {
#if defined(DEFAULT_CLASSPATH)
			cp = DEFAULT_CLASSPATH;
#else
			fprintf(stderr, "CLASSPATH is not set!\n");
			exit(1);
#endif
		}
	}
	realClassPath = cp;

	/* Process program options */
	farg = options(argv);

	/* Get the class name to start with */
	if (argv[farg] == 0) {
		usage();
		exit(1);
	}

	/* Initialise */
	initialiseKaffe();

	/* Build an array of strings as the arguments */
	args = (HArrayOfObject*)AllocObjectArray(argc-(farg+1), "Ljava/lang/String");

	/* Build each string and put into the array */
	str = (Hjava_lang_String**)unhand(args)->body;
	for (i = farg+1; i < argc; i++) {
		str[i-(farg+1)] = makeJavaString(argv[i], strlen(argv[i]));
	}

	/* Kick it */
	do_execute_java_class_method(argv[farg], "main", "([Ljava/lang/String;)V", args);
	exitThread();
	/* This should never return */
	exit(1);
	return(1);
}

/*
 * Process program's flags.
 */
static
int
options(char** argv)
{
	int i;
	int sz;
	userProperty* prop;

	for (i = 1; argv[i] != 0; i++) {
		if (argv[i][0] != '-') {
			break;
		}

		if (strcmp(argv[i], "-help") == 0) {
			usage();
			exit(0);
		}
		else if (strcmp(argv[i], "-version") == 0) {
			fprintf(stderr, "Kaffe Virtual Machine\n");
			fprintf(stderr, "Copyright (c) 1996, 1997\nTransvirtual Technologies, Inc.  All rights reserved\n");
			fprintf(stderr, "Engine: %s   Version: %s   Java Version: %s\n", engine_name, engine_version, java_version);
			exit(0);
		}
		else if (strcmp(argv[i], "-classpath") == 0) {
			i++;
			if (argv[i] == 0) {
				fprintf(stderr, "Error: No path found for -classpath option.\n");
				exit(1);
			}
			realClassPath = argv[i];
		}
		else if (strcmp(argv[i], "-ss") == 0) {
			i++;
			if (argv[i] == 0) {
				fprintf(stderr, "Error: No stack size found for -ss option.\n");
				exit(1);
			}
			sz = parseSize(argv[i]);
			if (sz < THREADSTACKSIZE) {
				fprintf(stderr, "Warning: Attempt to set stack size smaller than %d - ignored.\n", THREADSTACKSIZE);
			}
			else {
				threadStackSize = sz;
			}
		}
		else if (strcmp(argv[i], "-mx") == 0) {
			i++;
			if (argv[i] == 0) {
				fprintf(stderr, "Error: No heap size found for -mx option.\n");
				exit(1);
			}
			gc_heap_limit = parseSize(argv[i]);
		}
		else if (strcmp(argv[i], "-ms") == 0) {
			i++;
			if (argv[i] == 0) {
				fprintf(stderr, "Error: No heap size found for -ms option.\n");
				exit(1);
			}
			gc_heap_allocation_size = parseSize(argv[i]);
		}
		else if (strcmp(argv[i], "-verify") == 0) {
			flag_verify = 3;
		}
		else if (strcmp(argv[i], "-verifyremote") == 0) {
			flag_verify = 2;
		}
		else if (strcmp(argv[i], "-noverify") == 0) {
			flag_verify = 0;
		}
		else if (strcmp(argv[i], "-verbosegc") == 0) {
			flag_gc = 1;
		}
		else if (strcmp(argv[i], "-verbosejit") == 0) {
			flag_jit = 1;
		}
		else if (strcmp(argv[i], "-verbosemem") == 0) {
			flag_gc = 2;
		}
		else if (strcmp(argv[i], "-verbose") == 0 || strcmp(argv[i], "-v") == 0) {
			flag_classload = 1;
		}
		else if (argv[i][1] ==  'D') {
			/* Set a property */
			prop = malloc(sizeof(userProperty));
			assert(prop != 0);
			prop->next = userProperties;
			userProperties = prop;
			for (sz = 2; argv[i][sz] != 0; sz++) {
				if (argv[i][sz] == '=') {
					argv[i][sz] = 0;
					sz++;
					break;
				}
			}
			prop->key = &argv[i][2];
			prop->value = &argv[i][sz];
		}
		/* The following options are not supported and will be
		 * ignored for compatibility purposes.
		 */
		else if (strcmp(argv[i], "-noasyncgc") == 0 ||
		   strcmp(argv[i], "-cs") == 0 ||
		   strcmp(argv[i], "-debug") == 0 ||
		   strcmp(argv[i], "-checksource") == 0 ||
		   strcmp(argv[i], "-prof") == 0) {
		}
		else if (strcmp(argv[i], "-ms") == 0 ||
		   strcmp(argv[i], "-oss") == 0) {
			i++;
		}
		else {
			fprintf(stderr, "Unknown flag: %s\n", argv[i]);
		}
	}

	/* Return first no-flag argument */
	return (i);
}

/*
 * Print usage message.
 */
static
void
usage(void)
{
	fprintf(stderr, "usage: kaffe [-options] class\n");
	fprintf(stderr, "Options are:\n");
	fprintf(stderr, "	-help			Print this message\n");
	fprintf(stderr, "	-version		Print version number\n");
	fprintf(stderr, "	-ss <size>		Maximum native stack size\n");
	fprintf(stderr, "	-mx <size> 		Maximum heap size\n");
	fprintf(stderr, "	-ms <size> 		Initial heap size\n");
	fprintf(stderr, "	-classpath <path>	Set classpath\n");
	fprintf(stderr, "	-verify *		Verify all bytecode\n");
	fprintf(stderr, "	-verifyremote *		Verify bytecode loaded from network\n");
	fprintf(stderr, "	-noverify		Do not verify any bytecode\n");
	fprintf(stderr, "	-D<property>=<value>	Set a property\n");
	fprintf(stderr, "	-verbosegc		Print message during garbage collection\n");
	fprintf(stderr, "	-v, -verbose		Be verbose\n");
	fprintf(stderr, "	-verbosejit		Print message during JIT code generation\n");
	fprintf(stderr, "	-verbosemem		Print detailed memory allocation statistics\n");
	fprintf(stderr, "	-debug * 		Trace method calls\n");
	fprintf(stderr, "	-noasyncgc *		Do not garbage collect asynchronously\n");
	fprintf(stderr, "	-cs, -checksource *	Check source against class files\n");
	fprintf(stderr, "	-oss <size> *		Maximum java stack size\n");
	fprintf(stderr, "	-prof *			?\n");
	fprintf(stderr, "  * Option currently ignored.\n");
}

static
size_t
parseSize(char* arg)
{
	size_t sz;
	char* narg;

	sz = strtol(arg, &narg, 0);
	switch (narg[0]) {
	case 'b': case 'B':
		break;

	case 'k': case 'K':
		sz *= 1024;
		break;

	case 'm': case 'M':
		sz *= 1024 * 1024;
		break;
	}

	return (sz);
}
