/*
 * findInJar.c
 * Search the CLASSPATH for the given class or property name.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#define	FDBG(s)
#define ZDBG(s)
#define	PDBG(s)
#define	CDBG(s)

#include "config.h"
#include "config-std.h"
#include "config-io.h"
#include "config-mem.h"
#include "gtypes.h"
#include "file.h"
#include "exception.h"
#include "readClass.h"
#include "paths.h"
#include "flags.h"
#include "errors.h"
#include "lerrno.h"
#include "locks.h"
#include "baseClasses.h"
#include "external.h"
#include "jar.h"
#include "jsyscall.h"

#define	MAXBUF		256
#define	MAXPATHELEM	16

#define	CP_INVALID	0
#define	CP_ZIPFILE	1
#define	CP_DIR		2
#define	CP_SOFILE	3

#ifndef GXX_JOINER
#define GXX_JOINER '.'
#endif

#define	IS_ZIP(B)	((B)[0] == 'P' && (B)[1] == 'K')

static struct {
	int	type;
	char*	path;
	union {
		jarFile*	jar;
		struct { 
			int	loaded;
		} sof;
	} u;
} classpath[MAXPATHELEM+1];

static char* splitClassPath;
char* realClassPath;

void initClasspath(void);
classFile findInJar(char*);

static int getClasspathType(char*);
static void generateMangledName(char*, char*);

/*
 * Find the named class in a directory or JAR file.
 */
void
findClass(classEntry* centry)
{
	char buf[MAXBUF];
	classFile hand;
	char* cname;
	Hjava_lang_Class* class;

	cname = centry->name->data;
	class = centry->class;

	/* Look for the class */
CDBG(	printf("Scanning for class %s\n", cname);		)

	strcpy(buf, cname);
	strcat(buf, ".class");

	/* Find class in Jar file */
	hand = findInJar(buf);
	switch (hand.type) {
	case CP_DIR:
	case CP_ZIPFILE:
		if (class == 0) {
			class = newClass();
		}

		lockMutex(centry);

		class->name = centry->name;
		class->centry = centry;
		centry->class = class;
		readClass(class, &hand, NULL);
		class->state = CSTATE_LOADED;

		unlockMutex(centry);

		if (hand.base != 0) {
			gc_free_fixed(hand.base);
		}
		return;

	case CP_SOFILE:
		/* Note that if for an Elf shared library created using
		 * gcc -shared, the static constructors will be run by dlopen.
		 * This calls registerClass on each class, which means
		 * we actually never call findClass on such classes.
		 * However, this code is needed for shared libraries linked
		 * without automagic static constructor support.
		 */
		assert(class == NULL);
		class = (Hjava_lang_Class*)hand.base;
		centry->class = class;
		class->centry = centry;
		registerClass(centry);
		return;

	default:
		break;
	}

	/*
	 * Certain classes are essential.  If we don't find them then
	 * abort.
	 */
	if (strcmp(cname, "java/lang/ClassNotFoundException") == 0 ||
	    strcmp(cname, "java/lang/Object") == 0) {
		fprintf(stderr, "Cannot find essential class '%s' in class library ... aborting.\n", cname);
		ABORT();
	}
	return;
}

/*
 * Locate the given name in the CLASSPATH.
 */
classFile
findInJar(char* cname)
{
	int i;
	char buf[MAXBUF];
	int fp;
	struct stat sbuf;
	classFile hand;
	int j;
	jarEntry* entry;
	static iLock jarlock;
	static bool init = false;

	/* Initialise on first use */
	if (init == false) {
		init = true;
		initStaticLock(&jarlock);
	}

	/* Look for the class */
CDBG(	printf("Scanning for element %s\n", cname);		)

	/* One into the jar at once */
	lockStaticMutex(&jarlock);

	for (i = 0; classpath[i].path != 0; i++) {
		hand.type = classpath[i].type;
		switch (classpath[i].type) {
		case CP_ZIPFILE:
ZDBG(			printf("Opening JAR file %s for %s\n", classpath[i].path, cname); )
			if (classpath[i].u.jar == 0) {
				classpath[i].u.jar = openJarFile(classpath[i].path);
				if (classpath[i].u.jar == 0) {
					break;
				}
			}

			entry = lookupJarFile(classpath[i].u.jar, cname);
			if (entry == 0) {
				break;
			}
			hand.base = getDataJarFile(classpath[i].u.jar, entry);
			if (hand.base == 0) {
				throwException(IOException(classpath[i].u.jar->error));
			}
			hand.size = entry->uncompressedSize;
			hand.buf = hand.base;
			if (flag_classload) {
				fprintf(stderr, "Loading %s(%s)", cname, classpath[i].path);
				if (entry->compressionMethod != COMPRESSION_STORED) {
					fprintf(stderr, " [compressed]");
				}
				fprintf(stderr, "\n");
			}
			goto okay;

		case CP_DIR:
			strcpy(buf, classpath[i].path);
			strcat(buf, DIRSEP);
			strcat(buf, cname);
FDBG(			printf("Opening java file %s for %s\n", buf, cname); )
			fp = open(buf, O_RDONLY|O_BINARY, 0);
			if (fp < 0) {
				break;
			}
			if (fstat(fp, &sbuf) < 0) {
				close(fp);
				break;
			}
			hand.size = sbuf.st_size;

			hand.base = gc_malloc_fixed(hand.size);
			hand.buf = hand.base;

			i = 0;
			while (i < hand.size) {
				j = read(fp, hand.buf, hand.size - i);
				if (j >= 0) {
					i += j;
				}
				else if (!(errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)) {
					throwException(IOException("failed to read class data"));
				}
			}
			close(fp);
			if (flag_classload) {
				fprintf(stderr, "Loading %s\n", cname);
			}
			goto okay;

		case CP_SOFILE:
			if (classpath[i].u.sof.loaded == 0) {
				if (loadNativeLibrary(classpath[i].path) == 0) {
					break;
				}
				classpath[i].u.sof.loaded = 1;
			}
			generateMangledName(buf, cname);
			hand.base = loadNativeLibrarySym(buf);
			if (hand.base == NULL) {
				break;
			}
			if (flag_classload) {
				fprintf(stderr, "Registering %s.\n", cname);
			}
			goto okay;

		/* Ignore bad entries */
		default:
			break;
		}
	}
	/* If we call out the loop then we didn't find anything */
	hand.type = CP_INVALID;

	okay:;
	unlockStaticMutex(&jarlock);

	return (hand);
}

/* We generate the name of the class object for the class to match that
 * generated by gcc/jc1, which matches the one g++ uses for a static field.
 * Does not handle Unicode-mangling yet.  FIXME.
 */
static
void
generateMangledName(char* buf, char* cname)
{
	int qualifications;
	char* ptr;
	char* start;
	int j;

	qualifications = 0;
	for (ptr = cname; *ptr != '\0'; ptr++) {
		if (*ptr == '/') {
			qualifications++;
		}
	}
	buf[0] = '_';
	j = 1;
	if (qualifications) {
		sprintf (buf+j, "Q%d", qualifications+1);
		j = strlen(buf);
	}
	for (ptr = start = cname; ; ptr++) {
		if (ptr[0] == '/' || ptr[0] == '\0') {
			sprintf (buf+j, "%d%.*s", ptr-start, ptr-start, start);
			j += strlen (buf+j);
			if (ptr[0] == '\0') {
				break;
			}
			start = ptr + 1;
		}
	}
	buf[j++] = GXX_JOINER;
	strcpy(buf+j, "class");
}

/*
 * Initialise class path.
 */
void
initClasspath(void)
{
	char* cp;
	int i;
	int h;
	int c;

	cp = realClassPath;
	realClassPath = gc_malloc_fixed(strlen(cp) + 1);
	strcpy(realClassPath, cp);

	splitClassPath = gc_malloc_fixed(strlen(cp) + 1);
	strcpy(splitClassPath, cp);
	cp = splitClassPath;

PDBG(	printf("initClasspath(): '%s'\n", cp);				)
	for (i = 0; cp != 0 && i < MAXPATHELEM; i++) {
		classpath[i].path = cp;
		cp = strchr(cp, PATHSEP);
		if (cp != 0) {
			*cp = 0;
			cp++;
		}

		classpath[i].type = getClasspathType(classpath[i].path);

PDBG(		printf("path '%s' type %d\n", classpath[i].path, classpath[i].type); )
	}
	i++;
	classpath[i].path = 0;
}

/*
 * Add an entry in the Classpath dynamically.
 */
int
addClasspath(char* cp)
{
	int i;

	/* If classpath isn't set, initialize it */
	if (realClassPath == 0) {
		initClasspath();
	}

PDBG(	printf("addClasspath(): '%s'\n", cp);				)

	for (i = 0; i < MAXPATHELEM; i++) {
		if (classpath[i].path == 0) {
			goto found;
		}
		if (!strcmp(cp, classpath[i].path)) {
			return(0); /* already in */
		}
	}
	fprintf(stderr, "addClasspath : CLASSPATH is full!\n");
	return(-1);

	found:
	classpath[i].path = strdup(cp);

	classpath[i].type = getClasspathType(classpath[i].path);

PDBG(	printf("path '%s' type %d\n", classpath[i].path, classpath[i].type); )
	i++;
	classpath[i].path = 0;
	return(1);
}

/*
 * Work out what kind of thing this path points at.
 */
static
int
getClasspathType(char* path)
{
	int h;
	int c;
	char buf[2];
	struct stat sbuf;

	if (stat(path, &sbuf) < 0) {
		return (CP_INVALID);
	}

	if (S_ISDIR(sbuf.st_mode)) {
		return (CP_DIR);
	}

	h = open(path, O_RDONLY, 0);
	if (h < 0) {
		return (CP_INVALID);
	}

	c = read(h, buf, sizeof(buf));
	close(h);
	if (c != sizeof(buf)) {
		return (CP_INVALID);
	}

	if (IS_ZIP(buf)) {
		return (CP_ZIPFILE);
	}

	/* We should work out some way to check this ... */
	return (CP_SOFILE);
#if 0
	return (CP_INVALID);
#endif
}
