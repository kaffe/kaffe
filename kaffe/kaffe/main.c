/*
 * main.c
 * Kick off program.
 *
 * Copyright (c) 1996-2000
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Cross-language profiling changes contributed by
 * the Flux Research Group, Department of Computer Science,
 * University of Utah, http://www.cs.utah.edu/flux/
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "debug.h"
#include "classMethod.h"
#include "jtypes.h"
#include "native.h"
#include "constants.h"
#include "support.h"
#include "errors.h"
#include "thread.h"
#include "system.h"
#include "md.h"
#include "ltdl.h"
#include "version.h"
#include "debugFile.h"
#include "xprofiler.h"
#include "fileSections.h"
#include "feedback.h"
#include "methodCache.h"

#if defined(KAFFE_PROFILER)
extern int profFlag;
#endif

extern char* engine_name;
extern char* engine_version;

#include "jni.h"

JavaVMInitArgs vmargs;
JNIEnv* env;
JavaVM* vm;
static int isJar = 0;

static int options(char**);
static void usage(void);
static size_t parseSize(char*);
static int checkException(void);
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

#if defined(MAIN_MD)
	/* Machine specific main first */
	MAIN_MD;
#endif
	vmargs.version = JAVA_VERSION_HEX;

#if defined(KAFFE_PROFILER)
	profFlag = 0;
#endif

	JNI_GetDefaultJavaVMInitArgs(&vmargs);

	/* set up libtool/libltdl dlopen emulation */
	LTDL_SET_PRELOADED_SYMBOLS();

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
        vmargs.libraryhome = cp;

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

#if defined(KAFFE_XPROFILER)
	if( xProfFlag )
	{
		if( !enableXCallGraph() )
		{
			dprintf(
				"Unable to initialize cross "
				"language profiling\n");
			xProfFlag = 0;
		}
	}
#endif
	
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
	if (checkException())
		goto done;

	mmth = (*env)->GetStaticMethodID(env,
	    mcls, "main", "([Ljava/lang/String;)V");
	if (checkException())
		goto done;

	/* Build an array of strings as the arguments */
	cls = (*env)->FindClass(env, "java/lang/String");
	if (checkException())
		goto done;
	args = (*env)->NewObjectArray(env, argc, cls, 0);
	if (checkException())
		goto done;
	for (i = 0; i < argc; i++) {
		str = (*env)->NewStringUTF(env, argv[farg+i]);
		if (checkException())
			goto done;
		(*env)->SetObjectArrayElement(env, args, i, str);
		if (checkException())
			goto done;
	}

	/* Call method, check for errors and then exit */
	(*env)->CallStaticVoidMethod(env, mcls, mmth, args);
	(void)checkException();

done:
	/* We're done. We are the "main thread" and so are required to call
	   (*vm)->DestroyJavaVM() instead of (*vm)->DetachCurrentThread() */
	(*vm)->DestroyJavaVM(vm);
	return (0);
}

static int
checkException(void)
{
	jobject e;
	jclass eiic;

	/* Display exception stack trace */
	if ((e = (*env)->ExceptionOccurred(env)) == NULL)
		return (0);
	(*env)->ExceptionDescribe(env);
	(*env)->ExceptionClear(env);

	/* Display inner exception in ExceptionInInitializerError case */
	eiic = (*env)->FindClass(env, "java/lang/ExceptionInInitializerError");
	if ((*env)->ExceptionOccurred(env) != NULL) {
		(*env)->ExceptionClear(env);
		return (1);
	}
	if ((*env)->IsInstanceOf(env, e, eiic)) {
		e = (*env)->CallObjectMethod(env, e,
		    (*env)->GetMethodID(env, (*env)->GetObjectClass(env, e),
			"getException", "()Ljava/lang/Throwable;"));
		if ((*env)->ExceptionOccurred(env) != NULL) {
			(*env)->ExceptionClear(env);
			return (1);
		}
		if (e != NULL) {
			(*env)->Throw(env, e);
			return (checkException());
		}
	}
	return (1);
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
			printShortVersion();
			exit(0);
		}
		else if (strcmp(argv[i], "-fullversion") == 0) {
			printFullVersion();
			exit(0);
		}
		else if (strcmp(argv[i], "-classpath") == 0) {
			i++;
			if (argv[i] == 0) {
				dprintf(
				    "Error: No path found for %s option.\n",
				    "-classpath");
				exit(1);
			}
			vmargs.classpath = argv[i];
		}
		else if (strcmp(argv[i], "-addclasspath") == 0) {
			char	*newcpath;

			i++;
			if (argv[i] == 0) {
				dprintf(
				    "Error: No path found for %s option.\n",
				    "-addclasspath");
				exit(1);
			}

			/* Get longer buffer FIXME: free old one */
			if ((newcpath = malloc(strlen(vmargs.classpath)
			    + strlen(path_separator)
			    + strlen(argv[i]) + 1)) == NULL) {
				dprintf( "Error: out of memory.\n");
				exit(1);
			}

			/* Construct new classpath */
			strcpy(newcpath, vmargs.classpath);
			strcat(newcpath, path_separator);
			strcat(newcpath, argv[i]);
			vmargs.classpath = newcpath;
		}
		else if (strncmp(argv[i], "-ss", 3) == 0) {
			if (argv[i][3] == 0) {
				i++;
				if (argv[i] == 0) {
					dprintf( "Error: No stack size found for -ss option.\n");
					exit(1);
				}
				sz = parseSize(argv[i]);
			} else {
				sz = parseSize(&argv[i][3]);
			}
			if (sz < THREADSTACKSIZE) {
				dprintf( "Warning: Attempt to set stack size smaller than %d - ignored.\n", THREADSTACKSIZE);
			}
			else {
				vmargs.nativeStackSize = sz;
			}
		}
		else if (strncmp(argv[i], "-mx", 3) == 0) {
			if (argv[i][3] == 0) {
				i++;
				if (argv[i] == 0) {
					dprintf( "Error: No heap size found for -mx option.\n");
					exit(1);
				}
				vmargs.maxHeapSize = parseSize(argv[i]);
			} else {
				vmargs.maxHeapSize = parseSize(&argv[i][3]);
			}
		}
		else if (strncmp(argv[i], "-ms", 3) == 0) {
			if (argv[i][3] == 0) {
				i++;
				if (argv[i] == 0) {
					dprintf( "Error: No heap size found for -ms option.\n");
					exit(1);
				}
				vmargs.minHeapSize = parseSize(argv[i]);
			} else {
				vmargs.minHeapSize = parseSize(&argv[i][3]);
			}
		}
		else if (strncmp(argv[i], "-as", 3) == 0) {
			if (argv[i][3] == 0) {
				i++;
				if (argv[i] == 0) {
					dprintf( "Error: No heap size found for -as option.\n");
					exit(1);
				}
				vmargs.allocHeapSize = parseSize(argv[i]);
			} else {
				vmargs.allocHeapSize = parseSize(&argv[i][3]);
			}
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
		else if (strcmp(argv[i], "-noclassgc") == 0) {
			vmargs.enableClassGC = 0;
		}
		else if (strcmp(argv[i], "-verbosejit") == 0) {
			vmargs.enableVerboseJIT = 1;
		}
		else if (strcmp(argv[i], "-verbosemem") == 0) {
			vmargs.enableVerboseGC = 2;
		}
		else if (strcmp(argv[i], "-verbosecall") == 0) {
			vmargs.enableVerboseCall = 1;
		}
		else if (strcmp(argv[i], "-verbose") == 0 || strcmp(argv[i], "-v") == 0) {
			vmargs.enableVerboseClassloading = 1;
		}
                else if (strcmp(argv[i], "-jar") == 0) {
                        isJar = 1;
                }
#if defined(KAFFE_PROFILER)
		else if (strcmp(argv[i], "-prof") == 0) {
			profFlag = 1;
			vmargs.enableClassGC = 0;
		}
#endif
#if defined(KAFFE_XPROFILER)
		else if (strcmp(argv[i], "-Xxprof") == 0) {
			xProfFlag = 1;
			vmargs.enableClassGC = 0;
		}
		else if (strcmp(argv[i], "-Xxprof_syms") == 0) {
			i++;
			if (argv[i] == 0) {
				dprintf(
					"Error: -Xxprof_syms option requires "
					"a file name.\n");
			}
			else if( !profileSymbolFile(argv[i]) )
			{
				dprintf(
					"Unable to create profiler symbol "
					"file %s.\n",
					argv[i]);
			}
		}
		else if (strcmp(argv[i], "-Xxprof_gmon") == 0) {
			i++;
			if (argv[i] == 0) {
				dprintf(
					"Error: -Xxprof_gmon option requires "
					"a file name.\n");
			}
			else if (!profileGmonFile(argv[i]))
			{
				dprintf(
					"Unable to create gmon file %s.\n",
					argv[i]);
			}
		}
#endif
#if defined(KAFFE_XDEBUGGING)
		else if (strcmp(argv[i], "-Xxdebug_file") == 0) {
			i++;
			if (argv[i] == 0) {
				dprintf(
					"Error: -Xxdebug_file option requires "
					"a file name.\n");
			}
			else
			{
				machine_debug_filename = argv[i];
			}
		}
#endif
#if defined(KAFFE_FEEDBACK)
		else if (strcmp(argv[i], "-Xfeedback") == 0) {
			i++;
			if (argv[i] == 0) {
				dprintf(
					"Error: -Xfeedback option requires a "
					"file name.\n");
			}
			else
			{
				feedback_filename = argv[i];
			}
		}
#endif
		else if (strcmp(argv[i], "-nodeadlock") == 0) {
			deadlockDetection = 0;
		}
#if defined(KAFFE_STATS)
                else if (strcmp(argv[i], "-vmstats") == 0) {
			extern void statsSetMaskStr(char *);
                        i++;
                        if (argv[i] == 0) { /* forgot second arg */
                                dprintf( 
					"Error: -vmstats option requires a "
					"second arg.\n");
                                exit(1);
                        }
                        statsSetMaskStr(argv[i]);
                }
#endif
#if defined(DEBUG)
                else if (strcmp(argv[i], "-vmdebug") == 0) {
			extern int dbgSetMaskStr(char *);
                        i++;
                        if (argv[i] == 0) { /* forgot second arg */
                                dprintf( 
					"Error: -vmdebug option requires a "
					"debug flag. Use `list' for a list.\n");
                                exit(1);
                        }
                        if (!dbgSetMaskStr(argv[i]))
				exit(1);
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
		else if (argv[i][1] == 'X') {
			dprintf(
				"Error: Unrecognized JVM specific option "
				"`%s'.\n",
				argv[i]);
		}
		/* The following options are not supported and will be
		 * ignored for compatibility purposes.
		 */
		else if (strcmp(argv[i], "-noasyncgc") == 0 ||
		   strcmp(argv[i], "-cs") == 0 ||
		   strcmp(argv[i], "-checksource")) {
		}
		else if (strcmp(argv[i], "-oss") == 0) {
			i++;
		}
		else {
			dprintf( "Unknown flag: %s\n", argv[i]);
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
	dprintf("usage: kaffe [-options] class\n");
	dprintf("Options are:\n");
	dprintf("	-help			Print this message\n");
	dprintf("	-version		Print version number\n");
	dprintf("	-fullversion		Print verbose version info\n");
	dprintf("	-ss <size>		Maximum native stack size\n");
	dprintf("	-mx <size> 		Maximum heap size\n");
	dprintf("	-ms <size> 		Initial heap size\n");
	dprintf("	-as <size> 		Heap increment\n");
	dprintf("	-classpath <path>	Set classpath\n");
	dprintf("	-verify *		Verify all bytecode\n");
	dprintf("	-verifyremote *		Verify bytecode loaded from network\n");
	dprintf("	-noverify		Do not verify any bytecode\n");
	dprintf("	-D<property>=<value>	Set a property\n");
	dprintf("	-verbosegc		Print message during garbage collection\n");
	dprintf("	-noclassgc		Disable class garbage collection\n");
	dprintf("	-v, -verbose		Be verbose\n");
	dprintf("	-verbosejit		Print message during JIT code generation\n");
	dprintf("	-verbosemem		Print detailed memory allocation statistics\n");
	dprintf("	-verbosecall		Print detailed call flow information\n");
	dprintf("	-nodeadlock		Disable deadlock detection\n");
#if defined(KAFFE_PROFILER)
	dprintf("	-prof			Enable profiling of Java methods\n");
#endif
#if defined(KAFFE_XPROFILER)
	dprintf("	-Xxprof			Enable cross language profiling\n");
	dprintf("	-Xxprof_syms <file>	Name of the profiling symbols file [Default: kaffe-jit-symbols.s]\n");
	dprintf("	-Xxprof_gmon <file>	Base name for gmon files [Default: xgmon.out]\n");
#endif
#if defined(KAFFE_XDEBUGGING)
	dprintf("	-Xxdebug_file <file>	Name of the debugging symbols file\n");
#endif
#if defined(KAFFE_FEEDBACK)
	dprintf("	-Xfeedback <file>	The file name to write feedback data too\n");
#endif
	dprintf("	-debug * 		Trace method calls\n");
	dprintf("	-noasyncgc *		Do not garbage collect asynchronously\n");
	dprintf("	-cs, -checksource *	Check source against class files\n");
	dprintf("	-oss <size> *		Maximum java stack size\n");
        dprintf("	-jar                    Executable is a JAR\n");
#ifdef DEBUG
        dprintf("	-vmdebug <flag{,flag}>	Internal VM debugging.  Set flag=list for a list\n");                     
#endif
#ifdef KAFFE_STATS
        dprintf("	-vmstats <flag{,flag}>	Print VM statistics.  Set flag=all for all\n");                     
#endif
	dprintf("  * Option currently ignored.\n");
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
