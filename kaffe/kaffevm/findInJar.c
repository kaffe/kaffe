/*
 * findInJar.c
 * Search the CLASSPATH for the given class or property name.
 *
 * Copyright (c) 1996, 1997, 1998, 1999
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
#include "support.h"
#include "file.h"
#include "exception.h"
#include "readClass.h"
#include "system.h"
#include "errors.h"
#include "lerrno.h"
#include "locks.h"
#include "files.h"
#include "baseClasses.h"
#include "classMethod.h"
#include "external.h"
#include "jar.h"
#include "jsyscall.h"
#include "jni.h"
#include "classpath.h"
#include "stringSupport.h"
#include "stats.h"

#define	MAXBUF		256

classpathEntry* classpath;

char* realClassPath;

void initClasspath(void);

static int getClasspathType(const char*);
#if 0
static void generateMangledName(char*, const char*);
#endif
static void discoverClasspath(const char*);
static void makeClasspath(char*);

/*
 * Find the named class in a directory or JAR file.
 * Returns class if successful, NULL otherwise.
 */
Hjava_lang_Class*
findClass(classEntry* centry, errorInfo *einfo)
{
	char buf[MAXBUF];
	classFile hand;
	const char* cname;
	Hjava_lang_Class* class = 0;

	cname = centry->name->data;

	/* Note: In order to avoid that a thread goes on and tries to link a
	 * class that hasn't been fully read, we make sure we don't set 
	 * centry->class before the reading has completed.  It's the 
	 * caller's responsibility to set centry->class after the reading 
	 * is completed and threads can start racing for who gets to process 
	 * it.
	 */
	assert(centry->class == 0);

	/* Look for the class */
CDBG(	dprintf("Scanning for class %s\n", cname);		)

	strcpy(buf, cname);
	strcat(buf, ".class");

	/* Find class in Jar file */
	hand = findInJar(buf, einfo);
	if (hand.type == CP_INVALID) {
		return (0);
	}

	switch (hand.type) {
	case CP_DIR:
	case CP_ZIPFILE:
		class = newClass();
		if (class == 0) {
			postOutOfMemory(einfo);
			return (0);
		}

		utf8ConstAssign(class->name, centry->name);
		class->centry = centry;
		class = readClass(class, &hand, NULL, einfo);

		if (hand.base != 0) {
#if defined(KAFFE_STATS)
			if (hand.type == CP_ZIPFILE) {
				addToCounter(&jarmem, "vmmem-jar files", 1, 
					-(jlong)GCSIZEOF(hand.base));
			}
#endif
			KFREE(hand.base);
		}
		return (class);

	default:
		break;
	}

	/*
	 * Certain classes are essential.  If we don't find them then
	 * abort.  Note that loadStaticClass will abort for essential
	 * classes, so we only have to check for these two here.
	 */
	if (strcmp(cname, "java/lang/ClassNotFoundException") == 0 ||
	    strcmp(cname, "java/lang/Object") == 0) {
		fprintf(stderr, "Cannot find essential class '%s' in class library ... aborting.\n", cname);
		ABORT();
	}
	return (0);
}

/*
 * Locate the given name in the CLASSPATH.
 */
classFile
findInJar(char* cname, errorInfo *einfo)
{
	char buf[MAXBUF];
	int fp;
	struct stat sbuf;
	classFile hand;
	ssize_t j;
	jarEntry* entry;
	static iLock* jarlock;
	classpathEntry* ptr;
	int i;
	int rc;
	int iLockRoot;

	/* Look for the class */
CDBG(	dprintf("Scanning for element %s\n", cname);		)

	/* One into the jar at once */
	lockStaticMutex(&jarlock);

	for (ptr = classpath; ptr != 0; ptr = ptr->next) {
		hand.type = ptr->type;
		switch (ptr->type) {
		case CP_ZIPFILE:
ZDBG(			dprintf("Opening JAR file %s for %s\n", ptr->path, cname); )
			if (ptr->u.jar == 0) {
				ptr->u.jar = openJarFile(ptr->path);
				if (ptr->u.jar == 0) {
					break;
				}
			}

			entry = lookupJarFile(ptr->u.jar, cname);
			if (entry == 0) {
				break;
			}
			hand.base = getDataJarFile(ptr->u.jar, entry);
			if (hand.base == 0) {
				postExceptionMessage(einfo,
					JAVA_IO(IOException),
					"Couldn't extract data from jar: %s", 
					ptr->u.jar->error);
				hand.type = CP_INVALID;
				goto done;
			}
			hand.size = entry->uncompressedSize;
			hand.buf = hand.base;
			if (Kaffe_JavaVMArgs[0].enableVerboseClassloading) {
				fprintf(stderr, "Loading %s(%s)", cname, ptr->path);
				if (entry->compressionMethod != COMPRESSION_STORED) {
					fprintf(stderr, " [compressed]");
				}
				fprintf(stderr, "\n");
			}
			goto done;

		case CP_DIR:
			strcpy(buf, ptr->path);
			strcat(buf, file_separator);
			strcat(buf, cname);
FDBG(			dprintf("Opening java file %s for %s\n", buf, cname); )
			rc = KOPEN(buf, O_RDONLY|O_BINARY, 0, &fp);
			/* if we can't open the file, we keep looking */
			if (rc) {
				break;	/* will be NoClassDefFoundError */
			}
			/* if we can open the file, but cannot stat or read it,
			 * we flag an IOException (!?)
			 */
			if ((rc = KFSTAT(fp, &sbuf)) != 0) {
				KCLOSE(fp);
				postExceptionMessage(einfo, 
					JAVA_IO(IOException),
					"Couldn't fstat: %s", SYS_ERROR(rc));
				hand.type = CP_INVALID;
				goto done;
			}
			hand.size = sbuf.st_size;

			hand.base = hand.size == 0 ? NULL : KMALLOC(hand.size);
			if (hand.size != 0 && hand.base == 0) {
				postOutOfMemory(einfo);
				hand.type = CP_INVALID;
				goto done;
			}
			hand.buf = hand.base;

			i = 0;
			while (i < hand.size) {
				rc = KREAD(fp, hand.buf, hand.size - i, &j);
				if (rc != 0) {
					postExceptionMessage(einfo, 
						JAVA_IO(IOException),
						"Couldn't read: %s", 
						SYS_ERROR(rc));
					hand.type = CP_INVALID;
					KFREE(hand.base);
					break;
				} else {
					if (j > 0) {	/* more data */
						i += j;
					} else {	/* end of file */
						break;
					}
				}
			}
			KCLOSE(fp);
			if (Kaffe_JavaVMArgs[0].enableVerboseClassloading) {
				fprintf(stderr, "Loading %s\n", cname);
			}
			goto done;

		/* Ignore bad entries */
		default:
			break;
		}
	}
	/* If we call out the loop then we didn't find anything */
	hand.type = CP_INVALID;
	/* cut off the ".class" suffix for the exception msg */
	{	int dot = strlen(cname) - strlen(".class");
		if ((dot > 0) && (strcmp(cname + dot, ".class") == 0)) {
			cname[dot] = 0;
		}
		else {
			dot = -1;
		}
		cname[dot] = '\0';
		postExceptionMessage(einfo, JAVA_LANG(NoClassDefFoundError), cname);
		if (dot > 0) {
			cname[dot] = '.';
		}
	}

	done:;
	unlockStaticMutex(&jarlock);

	return (hand);
}

/*
 * Initialise class path.
 */
void
initClasspath(void)
{
	char* cp;
	char* hm;
	int len;
	classpathEntry* ptr;

	cp = (char*)Kaffe_JavaVMArgs[0].classpath;
	hm = (char*)Kaffe_JavaVMArgs[0].classhome;

	if (cp != 0) {
		/* cp may reside in read-only memory, but 
		 * makeClasspath writes to it
		 */
		char *writable_cp = KMALLOC(strlen(cp) + 1);
		strcpy(writable_cp, cp);
		makeClasspath(writable_cp);
		KFREE(writable_cp);
	}
	else {
		discoverClasspath(hm);
	}

	len = 0;
	for (ptr = classpath; ptr != 0; ptr = ptr->next) {
		len += strlen(ptr->path) + 1;
	}

	realClassPath = KMALLOC(len);
	for (ptr = classpath; ptr != 0; ptr = ptr->next) {
		if (ptr != classpath) {
			strcat(realClassPath, path_separator);
		}
		strcat(realClassPath, ptr->path);
	}
}


/*
 * Build classpathEntries from the given classpath.
 */
static
void
makeClasspath(char* cp)
{
	char* end;

PDBG(	dprintf("initClasspath(): '%s'\n", cp);				)

	for (;;) {
		/* FIXME: requires path_separator to have length 1 */
		end = strchr(cp, path_separator[0]);
		if (end != 0) {
			*end = 0;
			addClasspath(cp);
			cp = end + 1;
		}
		else {
			addClasspath(cp);
			break;
		}
	}
}

/*
 * Discover all available jar and zip files in this home location and
 * build a classpath from them.
 */
static
void
discoverClasspath(const char* home)
{
#if defined(DIR) /* Windows HACK - XXX */
	DIR* dir;
	struct dirent* entry;
	int len;
	int hlen;
	char* name;
	char buf[256];		/* FIXME:  FIXED SIZED BUFFER */

	dir = opendir(home);
	if (dir == 0) {
		return;
	}

	/* Add '.' and <home>/Klasses.zip at the beginning */
	addClasspath(".");
	strcpy(buf, home);
	strcat(buf, "/Klasses.jar");
	addClasspath(buf);

	hlen = strlen(home);
	while ((entry = readdir(dir)) != 0) {
		name = entry->d_name;
		len = strlen(name);
		if (strcmp(&name[len-4], ".zip") == 0 || strcmp(&name[len-4], ".jar") == 0) {
			strcpy(buf, home);
			strcat(buf, "/");
			strcat(buf, name);
			addClasspath(buf);
		}
	}
	closedir(dir);
#endif
}

/*
 * Add an entry in the Classpath dynamically.
 */
int
addClasspath(const char* cp)
{
	classpathEntry* ptr;
	classpathEntry* lptr;

PDBG(	dprintf("addClasspath(): '%s'\n", cp);				)

	lptr = 0;
	for (ptr = classpath; ptr != 0; ptr = ptr->next) {
		if (strcmp(ptr->path, cp) == 0) {
			/* Already in */
			return (0);
		}
		lptr = ptr;
	}

	ptr = KMALLOC(sizeof(classpathEntry) + strlen(cp) + 1);
	ptr->type = getClasspathType(cp);
	ptr->path = (char*)(ptr+1);
	ptr->next = 0;
	strcpy(ptr->path, cp);

	if (lptr == 0) {
		classpath = ptr;
	}
	else {
		lptr->next = ptr;
	}
	return(1);
}

/*
 * Work out what kind of thing this path points at.
 */
static
int
getClasspathType(const char* path)
{
	int h;
	ssize_t c;
	int rc;
	char buf[4];
	struct stat sbuf;

	if (KSTAT(path, &sbuf) < 0) {
		return (CP_INVALID);
	}

	if (S_ISDIR(sbuf.st_mode)) {
		return (CP_DIR);
	}

	rc = KOPEN(path, O_RDONLY, 0, &h);
	if (rc) {
		return (CP_INVALID);
	}

	rc = KREAD(h, buf, sizeof(buf), &c);
	KCLOSE(h);
	if (c != sizeof(buf)) {
		return (CP_INVALID);
	}

	if (IS_ZIP(buf)) {
		return (CP_ZIPFILE);
	}
	if (IS_SOFILE(buf)) {
		return (CP_SOFILE);
	}

	return (CP_INVALID);
}
