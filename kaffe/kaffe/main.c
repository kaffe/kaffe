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
static char* java_version;

#include "jni.h"

JavaVMInitArgs vmargs;
JNIEnv* env;
JavaVM* vm;
static int isJar = 0;

static int options(char**);
static void usage(void);
static size_t parseSize(char*);
static void handleErrors(void);
static int main2(JNIEnv* env, char *argv[], int farg, int argc);

#define	KAFFEHOME	"KAFFEHOME"
#define	CLASSPATH1	"KAFFECLASSPATH"
#define	LIBRARYPATH1	"KAFFELIBRARYPATH"

#define	CLASSPATH2	"CLASSPATH"
#define	LIBRARYPATH2	"LD_LIBRARY_PATH"

/*
 * MAIN
 */
int
main(int argc, char* argv[])
{
	int farg;
	char* cp;

	java_version = "1.1";
	vmargs.version = 0x00010001;

	JNI_GetDefaultJavaVMInitArgs(&vmargs);

	cp = getenv(CLASSPATH1);
	if (cp == 0) {
		cp = getenv(CLASSPATH2);
#if defined(DEFAULT_CLASSPATH)
		if (cp == 0) {
			cp = DEFAULT_CLASSPATH;
		}
#endif
	}
	vmargs.classpath = cp;

        cp = getenv(LIBRARYPATH1);
	if (cp == 0) {
		cp = getenv(LIBRARYPATH2);
	}
        vmargs.librarypath = cp;

        cp = getenv(KAFFEHOME);
        if (cp == 0) {
#if defined(DEFAULT_KAFFEHOME)
                cp = DEFAULT_KAFFEHOME;
#endif
        }
        vmargs.classhome = cp;

	/* Process program options */
	farg = options(argv);
	argc = argc - farg;

	/* Get the class name to start with */
	if (argv[farg] == 0) {
		usage();
		exit(1);
	}

	/* Initialise */
	JNI_CreateJavaVM(&vm, &env, &vmargs);

	return (main2(env, argv, farg, argc));
}

/*
 * Note:
 * Why do we split main in two parts?
 *
 * During initialisation, which is invoked in JNI_CreateJavaVM, we will
 * estimate the upper end of the main stack by taking the address of a
 * local variable.  The upper end of the main stack is important since
 * it tells the conservative garbage collector the upper boundary up to 
 * which it must scan the stack (stackEnd in the thread context).
 * (Replace stackEnd with stackBase and upper with lower if your stack
 *  grows upward.)
 *
 * To ensure that no references will be stored on the stack above that
 * estimated point, we will allocate 1K on the stack as a safe zone.
 *
 * The old approach would have the initialisation code guess how much 
 * it must add to the address of the local variable to find the actual 
 * upper end up to which the gc must look for local variables.  
 * The problem with that approach is what if the code guesses wrong?  
 * If it guesses too low, we will lose objects.  If it guesses too
 * high, however, the gc might segfault when trying to scan the stack.
 *
 * With the new approach some guessing is still involved:
 * we guess how big the safe zone should be.  If we guess too small,
 * we will lose objects.  If we guess too big, however, all we do is to 
 * waste memory on the main stack.  
 * Weighing the consequences, the new approach seems better.
 * Does anybody have a better solution?
 */

/*
 * MAIN, part II
 */
static int
main2(JNIEnv* env, char *argv[], int farg, int argc)
{
	char gc_safe_zone[1024];
	jarray args;
	jclass cls;
	jclass mcls;
	jmethodID mmth;
	jobject str;
	int i;
	char* exec;

	/* make sure no compiler optimizes this away */
	gc_safe_zone[0] = gc_safe_zone[sizeof gc_safe_zone - 1] = 0;

	/* Executable is a JAR?  Use the JAR launcher */
	if (isJar != 0) {
		exec = "kaffe.jar.ExecJar";
	}
	else {
		exec = argv[farg];
		farg++;
		argc--;
	}

	mcls = (*env)->FindClass(env, exec);
	handleErrors();

	mmth = (*env)->GetStaticMethodID(env, mcls, "main", "([Ljava/lang/String;)V");
	handleErrors();

	/* Build an array of strings as the arguments */
	cls = (*env)->FindClass(env, "java/lang/String");
	handleErrors();
	args = (*env)->NewObjectArray(env, argc, cls, 0);
	handleErrors();
	for (i = 0; i < argc; i++) {
		str = (*env)->NewStringUTF(env, argv[farg+i]);
		handleErrors();
		(*env)->SetObjectArrayElement(env, args, i, str);
		handleErrors();
	}

	/* Call method, check for errors and then exit */
	(*env)->CallStaticVoidMethod(env, mcls, mmth, args);
	handleErrors();

	(*vm)->DetachCurrentThread(vm);
	return (0);
}

static
void
handleErrors(void)
{
	if ((*env)->ExceptionOccurred(env)) {
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
		(*vm)->DetachCurrentThread(vm);
	}
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
			fprintf(stderr, "Copyright (c) 1996, 1997, 1998\nTransvirtual Technologies, Inc.  All rights reserved\n");
			fprintf(stderr, "Engine: %s   Version: %s   Java Version: %s\n", engine_name, engine_version, java_version);
			exit(0);
		}
		else if (strcmp(argv[i], "-classpath") == 0) {
			i++;
			if (argv[i] == 0) {
				fprintf(stderr, "Error: No path found for -classpath option.\n");
				exit(1);
			}
			vmargs.classpath = argv[i];
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
				vmargs.nativeStackSize = sz;
			}
		}
		else if (strcmp(argv[i], "-mx") == 0) {
			i++;
			if (argv[i] == 0) {
				fprintf(stderr, "Error: No heap size found for -mx option.\n");
				exit(1);
			}
			vmargs.maxHeapSize = parseSize(argv[i]);
		}
		else if (strcmp(argv[i], "-ms") == 0) {
			i++;
			if (argv[i] == 0) {
				fprintf(stderr, "Error: No heap size found for -ms option.\n");
				exit(1);
			}
			vmargs.allocHeapSize = parseSize(argv[i]);
		}
		else if (strcmp(argv[i], "-verify") == 0) {
			vmargs.verifyMode = 3;
		}
		else if (strcmp(argv[i], "-verifyremote") == 0) {
			vmargs.verifyMode = 2;
		}
		else if (strcmp(argv[i], "-noverify") == 0) {
			vmargs.verifyMode = 0;
		}
		else if (strcmp(argv[i], "-verbosegc") == 0) {
			vmargs.enableVerboseGC = 1;
		}
		else if (strcmp(argv[i], "-verbosejit") == 0) {
			vmargs.enableVerboseJIT = 1;
		}
		else if (strcmp(argv[i], "-verbosemem") == 0) {
			vmargs.enableVerboseGC = 2;
		}
		else if (strcmp(argv[i], "-verbose") == 0 || strcmp(argv[i], "-v") == 0) {
			vmargs.enableVerboseClassloading = 1;
		}
                else if (strcmp(argv[i], "-jar") == 0) {
                        isJar = 1;
                }
#ifdef DEBUG
                else if (strcmp(argv[i], "-vmdebug") == 0) {
			extern void dbgSetMaskStr(char *);
                        i++;
                        if (argv[i] == 0) { /* forgot second arg */
                                fprintf(stderr, 
					"Error: -vmdebug option requires a "
					"debug flag. Use `list' for a list.\n");
                                exit(1);
                        }
                        dbgSetMaskStr(argv[i]);
                }
#endif
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
        fprintf(stderr, "	-jar                    Executable is a JAR\n");
#ifdef DEBUG
        fprintf(stderr, "	-vmdebug <flag{,flag}>	Internal VM debugging. Set flag=list for a list\n");                     
#endif
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
