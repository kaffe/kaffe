/*
 * main.c
 * Generate native code stubs from .class files.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2004
 *	Kaffe.org contributors. See ChangeLog for details. All rights reserved.
 *   
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "kaffeh-support.h"
#include "utf8const.h"
#include "debug.h"

#define	BUFSZ	1024
#define	PATHSZ	1024

#define	PATH_SEP_CHAR	'/'

/*
 * Global symbols expected by the class parsing code.
 * XXX Move into classMethod.c and share between kaffeh and kaffevm.
 */
Utf8Const* init_name;
Utf8Const* final_name;
Utf8Const* void_signature;
Utf8Const* constructor_name;
Utf8Const* Code_name;
Utf8Const* LineNumberTable_name;
Utf8Const* LocalVariableTable_name;
Utf8Const* ConstantValue_name;
Utf8Const* Exceptions_name;
Utf8Const* SourceFile_name;
Utf8Const* InnerClasses_name;

FILE* include;
FILE* jni_include;
char className[BUFSZ];		/* XXX should allocate dynamicallly */
char pathName[BUFSZ];		/* XXX should allocate dynamicallly */
char includeName[BUFSZ];	/* XXX should allocate dynamicallly */
char realClassPath[PATHSZ];	/* XXX should allocate dynamicallly */
char tmpName[BUFSZ];		/* XXX should allocate dynamicallly */

int flag_shrt = 0;
int flag_jni = 0;
char* outputName = 0;
char* directoryName = 0;

static void usage(void);
static int options(int argc, char**);

/*
 * MAIN
 */
int
main(int argc, char* argv[])
{
	char* nm;
	int i, j, first = 1;
	int farg;

	/* Process arguments */
	farg = options(argc, argv);

	if (argv[farg] == 0) {
		usage();
		exit(1);
	}

	utf8ConstInit();
	init_name = utf8ConstNew("<clinit>", -1);
	final_name = utf8ConstNew("finalize", -1);
	void_signature = utf8ConstNew("()V", -1);
	constructor_name = utf8ConstNew("<init>", -1);
	Code_name = utf8ConstNew("Code", -1);
	LineNumberTable_name = utf8ConstNew("LineNumberTable", -1);
	LocalVariableTable_name = utf8ConstNew("LocalVariableTable", -1);
	ConstantValue_name = utf8ConstNew("ConstantValue", -1);
	Exceptions_name = utf8ConstNew("Exceptions", -1);
	SourceFile_name = utf8ConstNew("SourceFile", -1);
	InnerClasses_name = utf8ConstNew("InnerClasses", -1);

	/* Process each class */
	for (nm = argv[farg]; nm != 0; nm = argv[++farg]) {

		/* Derive various names from class name */
		for (i = j = 0; nm[i] != 0; i++, j++) {
			if (i >= BUFSZ - 100 || j >= BUFSZ - 105) {
				dprintf(
				    "kaffeh: class name too long\n");
				exit(1);
			}
			switch (nm[i]) {
			case '/':
			case '.':
				className[j] = '_';
				pathName[i] = '/';
				includeName[i] = '_';
				break;
			case '$':
				className[j++] = '_';
				className[j++] = '0';
				className[j++] = '0';
				className[j++] = '0';
				className[j++] = '2';
				className[j] = '4';
				pathName[i] = nm[i];
				includeName[i] = '_';
				break;
			default:
				className[j] = nm[i];
				pathName[i] = nm[i];
				includeName[i] = nm[i];
				break;
			}
		}
		className[j] = 0;
		pathName[i] = 0;
		includeName[i] = 0;

		/* If we are in 'base' mode, truncate the include name
		 * to just the basename.
		 */
		if (flag_shrt == 1) {
			for (i = strlen(includeName); i >= 0; i--) {
				if (includeName[i] == '_') {
					strcpy(includeName, &includeName[i+1]);
					break;
				}
			}
		}

		/* Prepend a directory name if given */
		if (directoryName != 0) { 

			int dirLen;

			dirLen = strlen(directoryName);
			assert(dirLen + strlen(includeName) < BUFSZ);

			strcpy(tmpName, includeName);
			strcpy(includeName, directoryName);
			includeName[dirLen] = PATH_SEP_CHAR;
			includeName[dirLen+1] = 0;
			strcat(includeName, tmpName);
		}

		/* Add suffix */
		strcat(includeName, ".h");

		/* Open output file */
		if (first || outputName == 0) {
			char *fileName = outputName ? outputName : includeName;
			FILE *fp;

			if ((fp = fopen(fileName, "w")) == NULL) {
				dprintf(
				    "Failed to create '%s'.\n", fileName);
				exit(1);
			}
			if (flag_jni) {
				jni_include = fp;
				initJniInclude();
			}
			else {
				include = fp;
				initInclude();
			}
			first = 0;
		}

		/* Preamble for this class */
		if (flag_jni) {
			startJniInclude();
		}
		else {
			startInclude();
		}

		/* Process class */
		kaffeh_findClass(pathName);

		/* Finalize */
		if (outputName == 0 || argv[farg + 1] == 0) {
			if (include != 0) {
				endInclude();
				fclose(include);
				include = 0;
			}
			if (jni_include != 0) {
				endJniInclude();
				fclose(jni_include);
				jni_include = 0;
			}
		}
	}

	exit(0);
}

/*
 * Process program's flags.
 */
static
int
options(int argc, char** argv)
{
	int i;

	for (i = 1; i < argc; i++) {
		if (argv[i][0] != '-') {
			break;
		}

		if (strcmp(argv[i], "-help") == 0) {
			usage();
			exit(0);
		}
		else if (strcmp(argv[i], "-version") == 0) {
			dprintf("Kaffeh Stub Generator\n");
			dprintf("Copyright (c) 1996, 1997\nTransvirtual Technologies, Inc.  All rights reserved\n");

			dprintf("Version: %s\n", PACKAGE_VERSION);
			exit(0);
		}
		else if (strcmp(argv[i], "-base") == 0) {
			flag_shrt = 1;
		}
		else if (strcmp(argv[i], "-jni") == 0) {
			flag_jni = 1;
		}
#ifdef KAFFE_VMDEBUG
		else if (strcmp(argv[i], "-Xdebug") == 0) {
			i++;
			dbgSetMaskStr(argv[i]);
		}
#endif /*KAFFE_VMDEBUG*/
		else if (strcmp(argv[i], "-classpath") == 0) {
			i++;
			strcpy(realClassPath, argv[i]);
		}
		else if (strcmp(argv[i], "-bootclasspath") == 0) {
			i++;
			strcpy(realClassPath, argv[i]);
		}
		else if (strcmp(argv[i], "-o") == 0) {
			i++;
			outputName = argv[i];
		}
		else if (strcmp(argv[i], "-d") == 0) {
			i++;
			directoryName = argv[i];
		}
		else {
			dprintf("Unknown flag: %s\n", argv[i]);
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
	dprintf("usage: kaffeh [-options] class ...\n");
	dprintf("Options are:\n");
	dprintf("	-help			Print this message\n");
	dprintf("	-version		Print version number\n");
	dprintf("	-classpath <path>	Set classpath\n");
	dprintf("	-bootclasspath <path>	Set classpath\n");
	dprintf("	-jni			Generate JNI interface\n");
#ifdef KAFFE_VMDEBUG
	dprintf("	-Xdebug <opts>		Kaffe debug options.\n");
#endif /*KAFFE_VMDEBUG*/
	dprintf("	-o <file>		Generate all output to the given file\n");
	dprintf("	-d <directory>		Directory for the output\n");
}
