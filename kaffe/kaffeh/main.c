/*
 * main.c
 * Generate native code stubs from .class files.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "kaffeh-support.h"

#define	BUFSZ	1024
#define	PATHSZ	1024

#define	PATH_SEP_CHAR	'/'

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
static int options(char**);
extern void utf8ConstInit(void);

/*
 * MAIN
 */
int
main(int argc, char* argv[])
{
	char* nm;
	int i, first = 1;
	int farg;

	/* Process arguments */
	farg = options(argv);

	if (argv[farg] == 0) {
		usage();
		exit(1);
	}
	utf8ConstInit();

	/* Process each class */
	for (nm = argv[farg]; nm != 0; nm = argv[++farg]) {

		/* Derive various names from class name */
		for (i = 0; nm[i] != 0; i++) {
			if (i >= BUFSZ - 100) {
				fprintf(stderr,
				    "kaffeh: class name too long\n");
				exit(1);
			}
			switch (nm[i]) {
			case '/':
			case '$':
			case '.':
				className[i] = '_';
				pathName[i] = '/';
				includeName[i] = '_';
				break;
			default:
				className[i] = nm[i];
				pathName[i] = nm[i];
				includeName[i] = nm[i];
				break;
			}
		}
		className[i] = 0;
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
				fprintf(stderr,
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
		findClass(pathName);

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
options(char** argv)
{
	int i;

	for (i = 1; argv[i] != 0; i++) {
		if (argv[i][0] != '-') {
			break;
		}

		if (strcmp(argv[i], "-help") == 0) {
			usage();
			exit(0);
		}
		else if (strcmp(argv[i], "-version") == 0) {
			fprintf(stderr, "Kaffeh Stub Generator\n");
			fprintf(stderr, "Copyright (c) 1996, 1997\nTransvirtual Technologies, Inc.  All rights reserved\n");

			fprintf(stderr, "Version: %s\n", KVER);
			exit(0);
		}
		else if (strcmp(argv[i], "-base") == 0) {
			flag_shrt = 1;
		}
		else if (strcmp(argv[i], "-jni") == 0) {
			flag_jni = 1;
		}
		else if (strcmp(argv[i], "-classpath") == 0) {
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
	fprintf(stderr, "usage: kaffeh [-options] class ...\n");
	fprintf(stderr, "Options are:\n");
	fprintf(stderr, "	-help			Print this message\n");
	fprintf(stderr, "	-version		Print version number\n");
	fprintf(stderr, "	-classpath <path>	Set classpath\n");
	fprintf(stderr, "	-jni			Generate JNI interface\n");
	fprintf(stderr, "	-o <file>		Generate all output to the given file\n");
	fprintf(stderr, "	-d <directory>		Directory for the output\n");
}
