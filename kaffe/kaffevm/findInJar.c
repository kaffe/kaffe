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
#include "debug.h"
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
#include "access.h"
#include "gcj/gcj.h"

#define KLASSES_JAR	"Klasses.jar"

classpathEntry* classpath;

char* realClassPath;

void initClasspath(void);

static int getClasspathType(const char*);
#if 0
static void generateMangledName(char*, const char*);
#endif
static void discoverClasspath(const char*);
static void makeClasspath(char*);
static classFile findClassInJar(char*, struct _errorInfo*);
static int insertClasspath(const char* cp, int prepend);

/*
 * Find the named class in a directory or JAR file.
 * Returns class if successful, NULL otherwise.
 */
Hjava_lang_Class*
findClass(classEntry* centry, errorInfo *einfo)
{
	char *buf;
	classFile hand;
	const char* cname;
	Hjava_lang_Class* class = 0;

	cname = centry->name->data;

#if defined(HAVE_GCJ_SUPPORT)
	/* 
 	 * XXX: for now, prefer *any* classes given in a .so file
	 * What we really want is to have separate lists for each .so module
	 * so that we can search for classes in the order that the classpath
	 * specifies.
	 * A good reimplementation would use a global hashtable to map
	 * all available names to the sources from which they'd be loaded.
	 */
	class = gcjFindClassByUtf8Name(cname, einfo);
	if (class != 0) {
		if (Kaffe_JavaVMArgs[0].enableVerboseClassloading) {
			/* XXX could say from where, but see above */
			fprintf(stderr, "Loading precompiled %s\n", cname);
		}
DBG(GCJ,	dprintf(__FUNCTION__": adding class %s to pool@%p\n",
			cname, centry);
    )
		class->centry = centry;
		assert(CLASS_GCJ(class));
		return (class);
	}
#endif

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

	buf = checkPtr(KMALLOC(strlen(cname) + 8));
	sprintf(buf, "%s.class", cname);

	/* Find class in Jar file */
	hand = findClassInJar(buf, einfo);
	KFREE(buf);
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
static classFile
findClassInJar(char* cname, errorInfo *einfo)
{
	char *buf;
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
			buf = checkPtr(KMALLOC(strlen(ptr->path)
			    + strlen(file_separator) + strlen(cname) + 1));
			sprintf(buf, "%s%s%s",
			    ptr->path, file_separator, cname);
FDBG(			dprintf("Opening java file %s for %s\n", buf, cname); )
			rc = KOPEN(buf, O_RDONLY|O_BINARY, 0, &fp);
			KFREE(buf);
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
	cname[strlen(cname) - strlen(".class")] = '\0';
	postNoClassDefFoundError(einfo, cname);

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

void
gcjInit(void)
{
#if defined(HAVE_GCJ_SUPPORT)
        classpathEntry* entry;

        /* Load any shared objects into VM now */
        for (entry = classpath; entry != 0; entry = entry->next) {
                if (entry->type == CP_SOFILE) {
			gcjLoadSharedObject(entry->path);
		}
	}
#endif
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
#if defined(HAVE_DIRENT_H)
	DIR* dir;
	struct dirent* entry;
	int len;
	int hlen;
	char* name;
	char* buf;

	dir = opendir(home);
	if (dir == 0) {
		return;
	}

	/* Add '.' and <home>/Klasses.jar at the beginning */
	addClasspath(".");
	buf = KMALLOC(strlen(home) + strlen(KLASSES_JAR) + 2);
	sprintf(buf, "%s/%s", home, KLASSES_JAR);
	addClasspath(buf);
	KFREE(buf);

	hlen = strlen(home);
	while ((entry = readdir(dir)) != 0) {
		name = entry->d_name;
		len = strlen(name);
		if (len > 4 && (strcmp(&name[len-4], ".zip") == 0
		    || strcmp(&name[len-4], ".jar") == 0)) {
			buf = KMALLOC(strlen(home) + strlen(name) + 2);
			sprintf(buf, "%s/%s", home, name);
			addClasspath(buf);
			KFREE(buf);
		}
	}
	closedir(dir);
#endif
}

/*
 * Append an entry in the Classpath dynamically.
 */
int
addClasspath(const char* cp)
{
	return insertClasspath(cp, 0);
}

/*
 * Prepend an entry in the Classpath dynamically.
 */
int
prependClasspath(const char* cp)
{
	return insertClasspath(cp, 1);
}

/*
 * Prepend or append an entry in the Classpath dynamically.
 */
static int
insertClasspath(const char* cp, int prepend)
{
	classpathEntry* ptr;
	classpathEntry* lptr;

PDBG(	dprintf("insertClasspath(): '%s' %spend\n", cp, prepend ? "pre" : "ap");)

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
	strcpy(ptr->path, cp);

	if (prepend || classpath == 0) {
		ptr->next = classpath;
		classpath = ptr;
	}
	else {
		ptr->next = 0;
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
