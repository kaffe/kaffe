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

#define	BUFSZ	100
#define	PATHSZ	1024

#define	PATH_SEP_CHAR	'/'

FILE* include;
char className[BUFSZ];
char pathName[BUFSZ];
char includeName[BUFSZ];
char realClassPath[PATHSZ];
char tmpName[BUFSZ];

int flag_shrt = 0;
char* outputName = 0;
char* directoryName = 0;

static void usage(void);
static int options(char**);

/*
 * MAIN
 */
int
main(int argc, char* argv[])
{
	char* nm;
	int i;
	int j;
	int farg;

	/* Process arguments */
	farg = options(argv);

	if (argv[farg] == 0) {
		usage();
		exit(1);
	}

	/* If we specify an output file, use that instead */
	if (outputName != 0) {
		FILE* file = fopen(outputName, "w");;
		if (file == 0) {
			fprintf(stderr, "Failed to create '%s'.\n", outputName);
			exit(1);
		}
		include = file;
		/* Setup the include ifdef name */
		strcpy(className, outputName);
	}

	for (nm = argv[farg]; nm != 0; nm = argv[++farg]) {
		j = 0;
		for (i = 0; nm[i] != 0; i++) {
			switch (nm[i]) {
			case '/':
			case '.':
				className[i] = '_';
				pathName[i] = '/';
				includeName[j+i] = '_';
				break;
			default:
				className[i] = nm[i];
				pathName[i] = nm[i];
				includeName[j+i] = nm[i];
				break;
			}
		}
		className[i] = 0;
		pathName[i] = 0;
		includeName[j+i] = 0;

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

		strcat(includeName, ".h");

		if (outputName == 0) {
			include = fopen(includeName, "w");
			if (include == 0) {
				fprintf(stderr, "Failed to create '%s'.\n", includeName);
				exit(1);
			}
		}
		initInclude();
		startInclude();
		findClass(pathName);

		if (outputName == 0) {
			if (include != 0) {
				endInclude();
				fclose(include);
			}
		}
	}

	if (outputName != 0) {
		if (include != 0) {
			endInclude();
			fclose(include);
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
	fprintf(stderr, "	-o <file>		Generate all output to the given file\n");
	fprintf(stderr, "	-d <directory>		Directory for the output\n");
}


void*
gc_malloc_fixed(size_t sz)
{
	void* mem;

	mem = malloc(sz);
	if (mem == NULL) {
		fprintf (stderr, "(Insufficient memory)\n");
		exit (-1);
	}
	return (mem);
}
