/*
 * main.c
 * Kick off program.
 *
 * Copyright (c) 1996-2000
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2006
 *      Kaffe.org contributors. See ChangeLog for details.
 *
 * Cross-language profiling changes contributed by
 * the Flux Research Group, Department of Computer Science,
 * University of Utah, http://www.cs.utah.edu/flux/
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#include <stdlib.h>

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "debug.h"
#include "classMethod.h"
#include "jni_md.h"
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
#if defined(KAFFE_FEEDBACK)
#include "feedback.h"
#endif
#include "external.h"
#include "kaffe_jni.h"

#if defined(HAVE_GETTEXT)
#include <libintl.h>
#define _(T) gettext(T)
#else
#define _(T) (T)
#endif
#if defined(HAVE_LC_MESSAGES)
#include <locale.h>
#endif

#if defined(KAFFE_PROFILER)
extern int profFlag;
#endif

#include "jni.h"

KaffeVM_Arguments vmargs;
JavaVM* global_vm;
static int isJar = 0;

static int options(char**, int);
static void usage(void);
static size_t parseSize(char*);
static int checkException(JNIEnv* env);
static int main2(JNIEnv* env, char *argv[], int farg, int argc);

#define	CLASSPATH1	"KAFFECLASSPATH"
#define	LIBRARYPATH1	"KAFFELIBRARYPATH"

#define	CLASSPATH2	"CLASSPATH"
#define	LIBRARYPATH2	"LD_LIBRARY_PATH"

#define BOOTCLASSPATH 	"BOOTCLASSPATH"

/*
 * MAIN
 */
int
main(int argc, char* argv[])
{
	int farg;
	const char* cp;
	void* env;

#if defined(MAIN_MD)
	MAIN_MD;
#endif

#if defined(HAVE_LC_MESSAGES)
	setlocale(LC_MESSAGES, "");
	setlocale(LC_CTYPE, "");
#endif
#if defined(HAVE_GETTEXT)
	bindtextdomain(PACKAGE, KAFFE_LOCALEDIR);
	textdomain(PACKAGE);
#endif

	vmargs.version = JNI_VERSION_1_1;

#if defined(KAFFE_PROFILER)
	profFlag = 0;
#endif

	JNI_GetDefaultJavaVMInitArgs(&vmargs);

	/* set up libtool/libltdl dlopen emulation */
	LTDL_SET_PRELOADED_SYMBOLS();

#if defined(KAFFE_VMDEBUG)
	cp = getenv("KAFFE_VMDEBUG");
	if (cp != 0)
		dbgSetMaskStr(cp);
#endif

	cp = getenv(BOOTCLASSPATH);
	vmargs.bootClasspath = cp;

	cp = getenv(CLASSPATH1);
	if (cp == 0) {
		cp = getenv(CLASSPATH2);
#if defined(DEFAULT_CLASSPATH)
		if (cp == 0) {
			cp = DEFAULT_CLASSPATH;
		}
#endif
	}
	vmargs.classpath = (cp == NULL? NULL :strdup(cp));

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
	farg = options(argv, argc);
	argc = argc - farg;

#if defined(KAFFE_XPROFILER)
	if( xProfFlag )
	{
		if( !enableXCallGraph() && !enableXProfiling() )
		{
			fprintf(stderr, 
				"Unable to initialize cross "
				"language profiling\n");
			xProfFlag = 0;
		}
	}
#endif

	/* Get the class name to start with */
	if (argv[farg] == 0) {
		usage();
		exit(EXIT_FAILURE);
	}

	if (strcmp(argv[farg] + strlen(argv[farg]) - strlen(".class"),
		   ".class") == 0) {
		fprintf(stderr,
			"Please do not specify the .class extension\n");
		exit(EXIT_FAILURE);
	}

	/* Initialise */
	if (JNI_CreateJavaVM(&global_vm, 
			     &env, 
			     &vmargs) 
	    < 0)
	  {
	    fprintf(stderr, "Cannot create the Java VM\n");
	    exit(EXIT_FAILURE);
	  }

	return main2(env, argv, farg, argc);
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
	int ret_code;
	const char* exec;

	/* make sure no compiler optimizes this away */
	gc_safe_zone[0] = gc_safe_zone[sizeof gc_safe_zone - 1] = 0;

	/* Executable is a JAR?  Use the JAR launcher */
	if (isJar != 0) {
		exec = "org.kaffe.jar.ExecJar";
	}
	else {
		exec = argv[farg];
		farg++;
		argc--;
	}
	
	mcls = (*env)->FindClass(env, exec);
	if (checkException(env))
		goto exception_happened;
	
	/* ... and run main. */
	mmth = (*env)->GetStaticMethodID(env,
	    mcls, "main", "([Ljava/lang/String;)V");
	if (checkException(env))
		goto exception_happened;

	/* Build an array of strings as the arguments */
	cls = (*env)->FindClass(env, "java/lang/String");
	if (checkException(env))
		goto exception_happened;
	args = (*env)->NewObjectArray(env, argc, cls, NULL);
	if (checkException(env))
		goto exception_happened;
	for (i = 0; i < argc; i++) {
		str = (*env)->NewStringUTF(env, argv[farg+i]);
		if (checkException(env))
			goto exception_happened;
		(*env)->SetObjectArrayElement(env, args, i, str);
		if (checkException(env))
			goto exception_happened;
	}

	/* Call method, check for errors and then exit */
	(*env)->CallStaticVoidMethod(env, mcls, mmth, args);
	if (checkException(env))
	  goto exception_happened;

	ret_code = 0;
	goto done;

exception_happened:
	ret_code = 1;
done:
	/* We're done. We are the "main thread" and so are required to call
	   (*vm)->DestroyJavaVM() instead of (*vm)->DetachCurrentThread() */
	(*global_vm)->DestroyJavaVM(global_vm);
	return ret_code;
}

static int
checkException(JNIEnv* env)
{
	jobject e;
	jclass eiic;

	/* Display exception stack trace */
	if ((e = (*env)->ExceptionOccurred(env)) == NULL)
		return (0);
	(*env)->DeleteLocalRef(env, e);
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
			return (checkException(env));
		}
	}
	return (1);
}

/*
 * Set a user property.
 *
 * @param propStr property to set
 * @return updated user properties
 */
static 
userProperty*
setUserProperty(char * propStr)
{
        size_t sz;
	userProperty * prop;

        prop = malloc(sizeof(userProperty));
        assert(prop != 0);
        prop->next = userProperties;
        userProperties = prop;
        for (sz = 0; propStr[sz] != 0; sz++) {
        	if (propStr[sz] == '=') {
                	propStr[sz] = 0;
                        sz++;
                        break;
         	}
    	}
       	prop->key = propStr;
        prop->value = &propStr[sz];

	return prop;
}

/*
 * Set AWT backend to use in Kaffe's AWT
 *
 * @param propStr AWT backend
 * @return updated user properties
 */

#if defined (KAFFE_X_AWT_INCLUDED) || defined (KAFFE_QT_AWT_INCLUDED)	\
  || defined (KAFFE_NANOX_AWT_INCLUDED) || defined(KAFFE_XYNTH_AWT_INCLUDED)
static 
userProperty*
setKaffeAWT(const char * propStr)
{
	char    *newbootcpath;
	unsigned int      bootcpathlength;
	char    *prefix =  DEFAULT_KAFFEHOME;

	const char *suffix = file_separator "kaffeawt.jar";
	char *backend_property = strdup(propStr);
        userProperty* prop;

	bootcpathlength = strlen(prefix)
        		+ strlen(suffix)
        		+ strlen(path_separator)
                        + ((vmargs.bootClasspath != NULL) ?
                           strlen(vmargs.bootClasspath) : 0)
                        + 1;

      	/* Get longer buffer FIXME:  free the old one */
       	if ((newbootcpath = malloc(bootcpathlength)) == NULL) 
	{
           	fprintf(stderr, "%s", _("Error: out of memory.\n"));
                exit(EXIT_FAILURE);
      	}

        /* Construct new boot classpath */
       	strcpy(newbootcpath, prefix);
        strcat(newbootcpath, suffix);
        strcat(newbootcpath, path_separator);
        if( vmargs.bootClasspath != 0 ) 
	{
        	strcat(newbootcpath, vmargs.bootClasspath);
	}

        /* set the new boot classpath */
        vmargs.bootClasspath = newbootcpath;

        /* select Xlib backend */
        prop = setUserProperty(backend_property);

	return prop;
}
#endif
/*
 * Process program's flags.
 */
static
int
options(char** argv, int argc)
{
	int i;
	unsigned int j;
	size_t sz;
	userProperty* prop;

	for (i = 1; i < argc; i++) {
		if (argv[i][0] != '-') {
			break;
		}

		if (strcmp(argv[i], "-help") == 0) {
			usage();
			exit(EXIT_SUCCESS);
		}
		else if (strcmp(argv[i], "-version") == 0) {
			printShortVersion();
			exit(EXIT_SUCCESS);
		}
		else if (strcmp(argv[i], "-fullversion") == 0) {
			printFullVersion();
			exit(EXIT_SUCCESS);
		}
#if defined(__ia64__)
		else if (strcmp(argv[i], "-ia32") == 0) {
			i++;
			/* FIXME: skip, case handled by the calle script */
		}
#endif
		else if (strcmp(argv[i], "-classpath") == 0
			 || strcmp(argv[i], "-cp") == 0) {

			i++;
			if (argv[i] == 0) {
				fprintf(stderr, 
				    "Error: No path found for %s option.\n",
				    argv[i - 1]);
				exit(EXIT_FAILURE);
			}

			/* set the new classpath */
			vmargs.classpath = strdup(argv[i]);
		}
		else if (strcmp(argv[i], "-addclasspath") == 0) {
			char	*newcpath;
			unsigned int      cpathlength;

			i++;
			if (argv[i] == 0) {
				fprintf(stderr, 
				    "Error: No path found for %s option.\n",
				    argv[i - 1]);
				exit(EXIT_FAILURE);
			}

			cpathlength = ((vmargs.classpath != NULL) ? strlen(vmargs.classpath) : 0)
				+ strlen(path_separator)
				+ strlen(argv[i])
				+ 1;

			/* Get longer buffer FIXME:  free the old one */
			if ((newcpath = malloc(cpathlength)) == NULL) {
				fprintf(stderr,  "%s", _("Error: out of memory.\n"));
				exit(EXIT_FAILURE);
			}

			/* Construct new classpath */
			if( vmargs.classpath != 0 )
			  	strcpy(newcpath, vmargs.classpath);
			else
				newcpath[0] = '\0';
			strcat(newcpath, path_separator);
			strcat(newcpath, argv[i]);

			/* set the new classpath */
			vmargs.classpath = newcpath;
		}
#ifdef KAFFE_X_AWT_INCLUDED
                /* Extra option to use kaffe's Xlib AWT backend.
                 */
                else if (strncmp(argv[i], "-Xkaffe-xlib-awt", (j=16)) == 0) {
			prop = setKaffeAWT("kaffe.awt.nativelib=xawt");
                }
#endif
#ifdef KAFFE_QT_AWT_INCLUDED
                /* Extra option to use kaffe's Qt/Embedded AWT backend.
                 */
                else if (strncmp(argv[i], "-Xkaffe-qt-awt", (j=15)) == 0) {
                        prop = setKaffeAWT("kaffe.awt.nativelib=qtawt");
                }
#endif
#ifdef KAFFE_NANOX_AWT_INCLUDED
		/* Extra option to use kaffe's Nano-X AWT backend.
		 */
		else if (strncmp(argv[i], "-Xkaffe-nanox-awt", (j=17)) == 0) {
			prop = setKaffeAWT("kaffe.awt.nativelib=nanoxawt");
		}
#endif
#ifdef KAFFE_XYNTH_AWT_INCLUDED
		/* Extra option to use kaffe's Xynth AWT backend.
		 */
		else if (strncmp(argv[i], "-Xkaffe-xynth-awt", (j=17)) == 0) {
			prop = setKaffeAWT("kaffe.awt.nativelib=xynthawt");
		}
#endif
#if defined(USE_GMP)
		/* Extra option to use gmp for native, fast bignums.
		 * Only available with binreloc, since binreloc is used to
		 * find the gmpjavamath.jar file.
		 */
                else if (strncmp(argv[i], "-Xnative-big-math", (j=17)) == 0) {
                        char    *newbootcpath;
                        unsigned int      bootcpathlength;
			const char *prefix = DEFAULT_KAFFEHOME;

 			const char *suffix = file_separator "gmpjavamath.jar";

                        bootcpathlength = strlen(prefix)
				+ strlen(suffix)
				+ strlen(path_separator)
                                + ((vmargs.bootClasspath != NULL) ?
                                        strlen(vmargs.bootClasspath) : 0)
                                + 1;

                        /* Get longer buffer FIXME:  free the old one */
                        if ((newbootcpath = malloc(bootcpathlength)) == NULL) {
                                fprintf(stderr,  "%s", _("Error: out of memory.\n"));
                                exit(EXIT_FAILURE);
                        }

                        /* Construct new boot classpath */
                        strcpy(newbootcpath, prefix);
			strcat(newbootcpath, suffix);
                        strcat(newbootcpath, path_separator);
                        if( vmargs.bootClasspath != 0 )
                                strcat(newbootcpath, vmargs.bootClasspath);

                        /* set the new boot classpath */
                        vmargs.bootClasspath = newbootcpath;
                }
#endif /* defined(USE_GMP) */
		else if (strncmp(argv[i], "-Xbootclasspath/p:", (j=18)) == 0) {
			char	*newbootcpath;
			unsigned int      bootcpathlength;

			bootcpathlength = strlen(&argv[i][j])
				+ strlen(path_separator)
				+ ((vmargs.bootClasspath != NULL) ?
					strlen(vmargs.bootClasspath) : 0)
				+ 1;

			/* Get longer buffer FIXME:  free the old one */
			if ((newbootcpath = malloc(bootcpathlength)) == NULL) {
				fprintf(stderr,  "%s", _("Error: out of memory.\n"));
				exit(EXIT_FAILURE);
			}

			/* Construct new boot classpath */
			strcpy(newbootcpath, &argv[i][j]);
			strcat(newbootcpath, path_separator);
			if( vmargs.bootClasspath != 0 )
			  	strcat(newbootcpath, vmargs.bootClasspath);

			/* set the new boot classpath */
			vmargs.bootClasspath = newbootcpath;
		}
		else if (strncmp(argv[i], "-Xbootclasspath/a:", (j=18)) == 0) {
			char	*newbootcpath;
			unsigned int      bootcpathlength;

			bootcpathlength = strlen(&argv[i][j])
				+ strlen(path_separator)
				+ ((vmargs.bootClasspath != NULL) ?
					strlen(vmargs.bootClasspath) : 0)
				+ 1;

			/* Get longer buffer FIXME:  free the old one */
			if ((newbootcpath = malloc(bootcpathlength)) == NULL) {
				fprintf(stderr,  "%s", _("Error: out of memory.\n"));
				exit(EXIT_FAILURE);
			}

			/* Construct new boot classpath */
			if( vmargs.bootClasspath != 0 ) {
			  	strcpy(newbootcpath, vmargs.bootClasspath);
				strcat(newbootcpath, path_separator);
			}
			else
				newbootcpath[0]='\0';

			strcat(newbootcpath, &argv[i][j]);

			/* set the new boot classpath */
			vmargs.bootClasspath = newbootcpath;
		}
		else if (strncmp(argv[i], "-Xbootclasspath:", (j=16)) == 0) {
			char	*newbootcpath;
			unsigned int      bootcpathlength;

			bootcpathlength = strlen(&argv[i][j]) + 1;

			/* Get longer buffer FIXME:  free the old one */
			if ((newbootcpath = malloc(bootcpathlength)) == NULL) {
				fprintf(stderr,  "%s", _("Error: out of memory.\n"));
				exit(EXIT_FAILURE);
			}

			/* Construct new boot classpath */
			strcpy(newbootcpath, &argv[i][j]);

			/* set the new boot classpath */
			vmargs.bootClasspath = newbootcpath;
		}
		else if ((strncmp(argv[i], "-ss", (j=3)) == 0) 
			 || (strncmp(argv[i], "-Xss", (j=4)) == 0)) {
			if (argv[i][j] == 0) {
				i++;
				if (argv[i] == 0) {
					fprintf(stderr, "%s", _("Error: No stack size found for -ss option.\n"));
					exit(EXIT_FAILURE);
				}
				sz = parseSize(argv[i]);
			} else {
				sz = parseSize(&argv[i][j]);
			}
			if (sz < THREADSTACKSIZE) {
				fprintf(stderr, _("Warning: Attempt to set stack size smaller than %d - ignored.\n"), THREADSTACKSIZE);
			}
			else {
				vmargs.nativeStackSize = sz;
			}
		}
		else if ((strncmp(argv[i], "-mx", (j=3)) == 0)
			 || (strncmp(argv[i], "-Xmx", (j=4)) == 0)) {
			if (argv[i][j] == 0) {
				i++;
				if (argv[i] == 0) {
					fprintf(stderr,  "%s", _("Error: No heap size found for -mx option.\n"));
					exit(EXIT_FAILURE);
				}
				if (strcmp(argv[i], "unlimited") == 0)
					vmargs.maxHeapSize = UNLIMITED_HEAP;
				else
					vmargs.maxHeapSize = parseSize(argv[i]);
			} else {
				if (strcmp(&argv[i][j], "unlimited") == 0)
					vmargs.maxHeapSize = UNLIMITED_HEAP;
				else
					vmargs.maxHeapSize = parseSize(&argv[i][j]);
			}
		}
		else if ((strncmp(argv[i], "-ms", (j=3)) == 0)
			 || (strncmp(argv[i], "-Xms", (j=4)) == 0)) {
			if (argv[i][j] == 0) {
				i++;
				if (argv[i] == 0) {
					fprintf(stderr,  "%s", _("Error: No heap size found for -ms option.\n"));
					exit(EXIT_FAILURE);
				}
				vmargs.minHeapSize = parseSize(argv[i]);
			} else {
				vmargs.minHeapSize = parseSize(&argv[i][j]);
			}
		}
		else if (strncmp(argv[i], "-as", 3) == 0) {
			if (argv[i][3] == 0) {
				i++;
				if (argv[i] == 0) {
					fprintf(stderr,  "%s", _("Error: No heap size found for -as option.\n"));
					exit(EXIT_FAILURE);
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
			char	*newcpath;
			unsigned int      cpathlength;

			cpathlength = strlen(argv[i+1]) + 
				strlen(path_separator) +
				((vmargs.classpath!=NULL) ? strlen(vmargs.classpath) : 0) +
				1;

			newcpath = (char *)malloc (cpathlength);
			if (newcpath == NULL) {
				fprintf(stderr,  "%s", _("Error: out of memory.\n"));
				exit(EXIT_FAILURE);
			}

			strcpy (newcpath, argv[i+1]);

			if (vmargs.classpath != NULL) {
				strcat (newcpath, path_separator);
				strcat (newcpath, vmargs.classpath);
				free ((void*)vmargs.classpath);
			}

			/* set the new classpath */
			vmargs.classpath = newcpath;

                        isJar = 1;
                }
		else if (strncmp(argv[i], "-Xrun", 5) == 0) {
		  char *argPos;
		  char *libName;
		  int libnameLen;

		  argPos = strchr(argv[i], ':');
		  if (argPos != NULL)
		    {
		      libnameLen = argPos - &argv[i][5];
		      vmargs.profilerArguments = strdup(argPos+1);
		    }
		  else
		    libnameLen = strlen(argv[i]) - 1;

		  libName = malloc(libnameLen+4);
		  strcpy(libName, "lib");
		  strncat(libName, &argv[i][5], libnameLen);

		  vmargs.profilerLibname = libName;
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
				fprintf(stderr, 
					"%s", _("Error: -Xxprof_syms option requires "
					"a file name.\n"));
			}
			else if( !profileSymbolFile(argv[i]) )
			{
				fprintf(stderr, 
					 _("Unable to create profiler symbol "
					"file %s.\n"),
					argv[i]);
			}
		}
		else if (strcmp(argv[i], "-Xxprof_gmon") == 0) {
			i++;
			if (argv[i] == 0) {
				fprintf(stderr, 
					"%s", _("Error: -Xxprof_gmon option requires "
					"a file name.\n"));
			}
			else if (!profileGmonFile(argv[i]))
			{
				fprintf(stderr, 
					_("Unable to create gmon file %s.\n"),
					argv[i]);
			}
		}
#endif
#if defined(KAFFE_XDEBUGGING)
		else if (strcmp(argv[i], "-Xxdebug") == 0) {
			/* Use a default name */
			machine_debug_filename = "xdb.as";
		}
		else if (strcmp(argv[i], "-Xxdebug_file") == 0) {
			i++;
			if (argv[i] == 0) {
				fprintf(stderr, 
					"%s", _("Error: -Xxdebug_file option requires "
					"a file name.\n"));
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
				fprintf(stderr, 
					"%s", _("Error: -Xfeedback option requires a "
					"file name.\n"));
			}
			else
			{
				feedback_filename = argv[i];
			}
		}
#endif
		else if (strcmp(argv[i], "-nodeadlock") == 0) {
			KaffeVM_setDeadlockDetection(0);
		}
#if defined(KAFFE_STATS)
                else if (strcmp(argv[i], "-vmstats") == 0) {
			extern void statsSetMaskStr(char *);
                        i++;
                        if (argv[i] == 0) { /* forgot second arg */
                                fprintf(stderr, 
					"%s", _("Error: -vmstats option requires a "
					"second arg.\n"));
                                exit(EXIT_FAILURE);
                        }
                        statsSetMaskStr(argv[i]);
                }
#endif
#if defined(KAFFE_VMDEBUG)
                else if (strcmp(argv[i], "-vmdebug") == 0) {
                        i++;
                        if (argv[i] == 0) { /* forgot second arg */
                                fprintf(stderr, 
					"%s", _("Error: -vmdebug option requires a "
					"debug flag. Use `list' for a list.\n"));
                                exit(EXIT_FAILURE);
                        }
                        if (!dbgSetMaskStr(argv[i]))
				exit(EXIT_FAILURE);
                }
#endif
                else if (strcmp(argv[i], "-debug-fd") == 0) {
			char *end;
                        i++;
                        if (argv[i] == 0) { /* forgot second arg */
                                fprintf(stderr, 
					"%s", _("Error: -debug-fd an open descriptor.\n"));
                                exit(EXIT_FAILURE);
                        }
			dbgSetDprintfFD(strtol(argv[i], &end, 10));
			if (end != 0 && *end != '\0') {
				fprintf(stderr,
					"%s", _("Error: -debug-fd requires an integer.\n"));
				exit(EXIT_FAILURE);
			}
                }
		else if (argv[i][1] ==  'D') {
			/* Set a property */
			char *propStr = strdup(&argv[i][2]);

			prop = setUserProperty(propStr);
		}
		else if (argv[i][1] == 'X') {
			fprintf(stderr, 
				_("Error: Unrecognized JVM specific option "
				"`%s'.\n"),
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
			fprintf(stderr, _("Unknown flag: %s\n"), argv[i]);
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
	fprintf(stderr,
		"%s", _("usage: kaffe [-options] class\n"
		  "Options are:\n"
		  "	-help			 Print this message\n"
		  "	-version		 Print version number\n"
		  "	-fullversion		 Print verbose version info\n"));
#if defined(__ia64__)
	fprintf(stderr, "%s", _("	-ia32			 Execute the ia32 version of Kaffe\n"));
#endif
	fprintf(stderr, "%s", _("	-ss <size>		 Maximum native stack size\n"
			  "	-mx <size> 		 Maximum heap size\n"
			  "	-ms <size> 		 Initial heap size\n"
			  "	-as <size> 		 Heap increment\n"
			  "	-classpath <path>        Set classpath\n"
			  "	-Xbootclasspath:<path>   Set bootclasspath\n"
			  "	-Xbootclasspath/a:<path> Append path to bootclasspath\n"
			  "	-Xbootclasspath/p:<path> Prepend path to bootclasspath\n"
			  "	-D<property>=<value>     Set a property\n"
			  "	-verify *		 Verify all bytecode\n"
			  "	-verifyremote *		 Verify bytecode loaded from network\n"
			  "	-noverify		 Do not verify any bytecode\n"
			  "	-noclassgc		 Disable class garbage collection\n"
			  "	-verbosegc		 Print message during garbage collection\n"
			  "	-v, -verbose		 Be verbose\n"
			  "	-verbosejit		 Print message during JIT code generation\n"
			  "	-verbosemem		 Print detailed memory allocation statistics\n"
			  "	-verbosecall		 Print detailed call flow information\n"
			  "	-nodeadlock		 Disable deadlock detection\n"));
#if defined(KAFFE_PROFILER)
	fprintf(stderr, "%s", _("	-prof			 Enable profiling of Java methods\n"));
#endif
#if defined(KAFFE_XPROFILER)
	fprintf(stderr, "%s", _("	-Xxprof			 Enable cross language profiling\n"
			  "	-Xxprof_syms <file>	 Name of the profiling symbols file [Default: kaffe-jit-symbols.s]\n"
			  "	-Xxprof_gmon <file>	 Base name for gmon files [Default: xgmon.out]\n"));
#endif
#if defined(KAFFE_XDEBUGGING)
	fprintf(stderr, "%s", _("	-Xxdebug_file <file>	 Name of the debugging symbols file\n"));
#endif
#if defined(KAFFE_FEEDBACK)
	fprintf(stderr, "%s", _("	-Xfeedback <file>	 The file name to write feedback data to\n"));
#endif
	fprintf(stderr, "%s", _("	-debug * 		 Trace method calls\n"
			  "	-noasyncgc *		 Do not garbage collect asynchronously\n"
			  "	-cs, -checksource *	 Check source against class files\n"
			  "	-oss <size> *		 Maximum java stack size\n"
			  "	-jar                     Executable is a JAR\n"));
#ifdef KAFFE_VMDEBUG
        fprintf(stderr, "%s", _("	-vmdebug <flag{,flag}>	 Internal VM debugging.  Set flag=list for a list\n"));
#endif
        fprintf(stderr, "%s", _("	-debug-fd <descriptor>	 Descriptor to send debug info to\n"));
#ifdef KAFFE_STATS
        fprintf(stderr, "%s", _("	-vmstats <flag{,flag}>	 Print VM statistics.  Set flag=all for all\n"));
#endif
#if defined(USE_GMP)
        fprintf(stderr, "%s", _("	-Xnative-big-math	 Use GMP for faster, native bignum calculations\n"));
#endif /* defined(USE_GMP) */
#ifdef KAFFE_X_AWT_INCLUDED
	fprintf(stderr, "%s", _("	-Xkaffe-xlib-awt	 Use Kaffe's Xlib AWT backend\n"));
#endif
#ifdef KAFFE_QT_AWT_INCLUDED
	fprintf(stderr, "%s", _("	-Xkaffe-qt-awt		 Use Kaffe's Qt2/3/Embedded AWT backend\n"));
#endif
#ifdef KAFFE_NANOX_AWT_INCLUDED
	fprintf(stderr, "%s", _("	-Xkaffe-nanox-awt	 Use Kaffe's Nano-X AWT backend\n"));
#endif
#ifdef KAFFE_XYNTH_AWT_INCLUDED
	fprintf(stderr, _("	-Xkaffe-xynth-awt	 Use Kaffe's Xynth AWT backend\n"));
#endif

	fprintf(stderr, "%s", _("  * Option currently ignored.\n"
			  "\n"
			  "Compatibility options:\n"
			  "	-Xss <size>		 Maximum native stack size\n"
			  "	-Xmx <size> 		 Maximum heap size\n"
			  "	-Xms <size> 		 Initial heap size\n"
			  "	-cp <path> 		 Set classpath\n"));
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
	default:
		break;
	}

	return (sz);
}
