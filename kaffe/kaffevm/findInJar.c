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
#include "defs.h"
#include "prefix.h"

#ifdef __riscos__
#include <unixlib/local.h>
#endif

/* Handle Manifest Class-Path attribute.  It will be better to handle that
   in a ClassLoader.  */
#define HANDLE_MANIFEST_CLASSPATH	1

#define KLASSES_JAR	"Klasses.jar"

classpathEntry* classpath;

const char* realClassPath;
char* realBootClassPath;

void initClasspath(void);

static int getClasspathType(const char*);
#if 0
static void generateMangledName(char*, const char*);
#endif
static void discoverClasspath(const char*);
static void makeClasspath(char*);
static void findClassInJar(char*, classFile*, struct _errorInfo*);
static int insertClasspath(const char* cp, int prepend);

#if defined(HANDLE_MANIFEST_CLASSPATH)
static int isEntryInClasspath(const char*);
static char* getManifestMainAttribute(jarFile*, const char*);
static void handleManifestClassPath (classpathEntry *);
#endif

static char* discoverClassHome(void);

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
	Hjava_lang_Class* class = NULL;

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
		if (Kaffe_JavaVMArgs.enableVerboseClassloading) {
			/* XXX could say from where, but see above */
			dprintf("Loading precompiled %s\n", cname);
		}
DBG(GCJ,	dprintf(__FUNCTION__": adding class %s to pool@%p\n",
			cname, centry);
    );
		class->centry = centry;
		assert(CLASS_GCJ(class));
		return (class);
	}
#endif

	/* Look for the class */
DBG(CLASSLOOKUP,
	dprintf("Scanning for class %s\n", cname);		);

	buf = checkPtr(KMALLOC(strlen(cname) + 8));
	sprintf(buf, "%s.class", cname);

	/* Find class in Jar file */
	findClassInJar(buf, &hand, einfo);
	KFREE(buf);
	if (hand.type == CP_INVALID) {
		/* We should only throw a ClassNotFoundException. */
		discardErrorInfo(einfo);
		postExceptionMessage(einfo,
				     JAVA_LANG(ClassNotFoundException),
				     "%s",
				     centry->name->data);
		return (NULL);
	}

	switch (hand.type) {
	case CP_DIR:
	case CP_ZIPFILE:
		class = newClass();
		if (class == NULL) {
			postOutOfMemory(einfo);
			KFREE(hand.mem);
			return (NULL);
		}

		utf8ConstAssign(class->name, centry->name);
		class->centry = centry;
		class = readClass(class, &hand, NULL, einfo);

		if (hand.base != NULL) {
#if defined(KAFFE_STATS)
			if (hand.type == CP_ZIPFILE) {
				addToCounter(&jarmem, "vmmem-jar files", 1,
					-(jlong)GCSIZEOF(hand.base));
			}
#endif
			KFREE(hand.mem);
		}
		return (class);

	case CP_INVALID:
	case CP_SOFILE:
	case CP_BYTEARRAY:
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
		dprintf("Cannot find essential class '%s' in class library ... aborting.\n", cname);
		KAFFEVM_ABORT();
	}
	return (NULL);
}

static iStaticLock	jarlock;

/*
 * Locate the given name in the CLASSPATH.  Fill in the provided
 * classFile handle with a buffer containing the class (or
 * set the hand->type to CP_INVALID).
 *
 * May write into cname.
 */
static void
findClassInJar(char* cname, classFile* hand, errorInfo *einfo)
{
	char *buf;
	int fp;
	classpathEntry* ptr;
	int i;
	int rc;

	/* Look for the class */
DBG(CLASSLOOKUP,  dprintf("Scanning for element %s\n", cname); );

	hand->type = CP_INVALID;

	/* One into the jar at once */
	lockStaticMutex(&jarlock);

	for (ptr = classpath; ptr != 0; ptr = ptr->next) {
DBG(CLASSLOOKUP,dprintf("Processing classpath entry '%s'\n", ptr->path); );
		switch (ptr->type) {
		case CP_ZIPFILE:
		{
			jarEntry* entry;
			unsigned char* data;

DBG(CLASSLOOKUP,	dprintf("Opening JAR file %s for %s\n", ptr->path, cname); );
			if (ptr->u.jar == 0) {
				ptr->u.jar = openJarFile(ptr->path);
				if (ptr->u.jar == 0) {
					break;
				}
#if defined(HANDLE_MANIFEST_CLASSPATH)
				/* handle Manifest Class-Path attribute */
				handleManifestClassPath (ptr);
#endif
			}

			entry = lookupJarFile(ptr->u.jar, cname);
			if (entry == 0) {
				break;
			}
			data = getDataJarFile(ptr->u.jar, entry);
			if (data == 0) {
				postExceptionMessage(einfo,
					JAVA_IO(IOException),
					"Couldn't extract data from jar: %s",
					ptr->u.jar->error);
				goto done;
			}

			classFileInit(hand,
				      data,
				      data,
				      entry->uncompressedSize,
				      CP_ZIPFILE);

			if (Kaffe_JavaVMArgs.enableVerboseClassloading) {
				dprintf("Loading %s(%s)", cname, ptr->path);
				if (entry->compressionMethod != COMPRESSION_STORED) {
					dprintf(" [compressed]");
				}
				dprintf("\n");
			}
			goto done;
		}

		case CP_DIR:
		{
			struct stat sbuf;
			unsigned char* data;

			buf = checkPtr(KMALLOC(strlen(ptr->path)
			    + strlen(file_separator) + strlen(cname) + 1));
			sprintf(buf, "%s%s%s",
			    ptr->path, file_separator, cname);
DBG(CLASSLOOKUP,	dprintf("Opening java file %s for %s\n", buf, cname); );
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
				goto done;
			}

			/*
			 * XXX Whlist bogus, a zero-length class file poses
			 * no problems for this code.  Assume the user of
			 * the file will find that problem...
			 */

			data = NULL;
			if (sbuf.st_size > 0)
			{
				data = KMALLOC((size_t)sbuf.st_size);
				if (data == 0) {
					postOutOfMemory(einfo);
					goto done;
				}
			}

			i = 0;
			while (i < sbuf.st_size) {
				ssize_t j;
				rc = KREAD(fp, data, (size_t)(sbuf.st_size - i), &j);
				if (rc != 0) {
					postExceptionMessage(einfo,
						JAVA_IO(IOException),
						"Couldn't read: %s",
						SYS_ERROR(rc));
					KFREE(data);
					break;
				} else {
					if (j > 0) {	/* more data */
						i += j;
					} else {	/* end of file */
						break;
					}
				}
			}

			classFileInit(hand,
				      data,
				      data,
				      (unsigned)sbuf.st_size,
				      CP_DIR);

			KCLOSE(fp);
			if (Kaffe_JavaVMArgs.enableVerboseClassloading) {
				dprintf("Loading %s\n", cname);
			}
			goto done;
		}

		/* Ignore bad entries */
		default:
			/* XXX warning.... */
			break;
		}
	}
	/* If we call out the loop then we didn't find anything */
	assert (hand->type == CP_INVALID);

	/* cut off the ".class" suffix for the exception msg */
	cname[strlen(cname) - strlen(".class")] = '\0';
	/*
	 * Technically, we're just loading a file, so use
	 * FileNotFoundException.
	 */
	postExceptionMessage(einfo,
			     JAVA_IO(FileNotFoundException),
			     "%s",
			     cname);

	done:;
	unlockStaticMutex(&jarlock);
}

/*
 * Initialise class path.
 */
void
initClasspath(void)
{
	const char* cp;
	const char* hm;
	size_t len;
	classpathEntry* ptr;

	DBG(INIT, dprintf("initClasspath()\n"); );

	cp = Kaffe_JavaVMArgs.bootClasspath;
	hm = Kaffe_JavaVMArgs.classhome;

	initStaticLock(&jarlock);

	if (cp != NULL && cp[0] != '\0') {
		/* cp may reside in read-only memory, but
		 * makeClasspath writes to it
		 */
		char *writable_cp = KMALLOC(strlen(cp) + 1);
		strcpy(writable_cp, cp);
		makeClasspath(writable_cp);
		KFREE(writable_cp);
	}
	else {
           if (0 == hm || hm[0] == '\0') {
              hm = discoverClassHome();
           }
           if (hm) {
		discoverClasspath(hm);
           }
        }

	len = 0;
	for (ptr = classpath; ptr != 0; ptr = ptr->next) {
		len += strlen(ptr->path) + 1;
	}

	if (len == 0) {
		/* Error on classpath will be reported latter */
		realBootClassPath = strdup("");
		return;
	}

	
	realBootClassPath = KMALLOC(len);
	for (ptr = classpath; ptr != 0; ptr = ptr->next) {
		if (ptr != classpath) {
			strcat(realBootClassPath, path_separator);
		}
		strcat(realBootClassPath, ptr->path);
	}
	
	realClassPath = Kaffe_JavaVMArgs.classpath;

	DBG(INIT, dprintf("initClasspath() done, got %s\n", realBootClassPath); );
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

DBG(INITCLASSPATH,
    	dprintf("initClasspath(): '%s'\n", cp); );

	for (;;) {
		end = strstr(cp, path_separator);
		if (end != 0) {
			*end = '\0';
			addClasspath(cp);
			cp = end + strlen(path_separator);
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
#ifdef __riscos__
        char unixpath[256];
#endif

DBG(INITCLASSPATH,
	dprintf("insertClasspath(): '%s' %spend\n", cp, prepend ? "pre" : "ap"); );

	if (*cp == '\0')
		return (0);

	lptr = NULL;
	for (ptr = classpath; ptr != 0; ptr = ptr->next) {
		if (strcmp(ptr->path, cp) == 0) {
			/* Already in */
			return (0);
		}
		lptr = ptr;
	}

#ifdef __riscos__
        __unixify(cp, 0, unixpath, 256, __RISCOSIFY_FILETYPE_NOTSPECIFIED);
        cp = unixpath;
#endif

	ptr = KMALLOC(sizeof(classpathEntry) + strlen(cp) + 1);
	ptr->type = getClasspathType(cp);
	ptr->path = (char*)(ptr+1);
	strcpy(ptr->path, cp);

	if (prepend || classpath == 0) {
		ptr->next = classpath;
		classpath = ptr;
	}
	else {
		ptr->next = NULL;
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

	if (KSTAT(path, &sbuf)) {
		return (CP_INVALID);
	}

	if (S_ISDIR(sbuf.st_mode)) {
		return (CP_DIR);
	}

	rc = KOPEN(path, O_RDONLY|O_BINARY, 0, &h);
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

#if defined(HANDLE_MANIFEST_CLASSPATH)

/*
 * Check whether a classpath entry is already in classpath
 */
static int
isEntryInClasspath(const char *path)
{
	classpathEntry *ptr;

	for(ptr = classpath; ptr != 0; ptr = ptr->next) {
		if(!strcmp(ptr->path, path))
			return 1;
	}
	return 0;
}


static char*
getManifestMainAttribute(jarFile* file, const char* attrName)
{
	jarEntry* mf;
	char* mfdata;
	char* attrEntry;
	char* ret;
	size_t i;
	int posAttrValue;

	/* Locate manifest entry in jar */
	mf = lookupJarFile(file, "META-INF/MANIFEST.MF");
	if (mf == 0)
		return (NULL);

	/* Read it */
	mfdata = (char*)getDataJarFile(file, mf);
	if (mfdata == 0)
		return (NULL);

	/* Look for the desired entry */
	attrEntry = mfdata;
	for (i = 0; i < mf->uncompressedSize; ++i) {
		/* Sun's jar, even under Linux, insists on terminating
		   newlines with newline *and* carriage return */
		if (mfdata[i] == '\n' || mfdata[i] == '\r') {
			mfdata[i] = '\0';
			/* Ecco! this line begins with the attribute's name */
			if (strstr(attrEntry, attrName)) {
				/* Skip 'attrName:' */
				posAttrValue = strlen(attrName) + 1;
				attrEntry += posAttrValue;

				/* Skip initial whitespace */
				while (*attrEntry == ' ' || *attrEntry == '\t')
					++attrEntry;

				/* Now look for end of string. */
				while (i < mf->uncompressedSize && attrEntry[i] != 0xd)
					++i;

				attrEntry[i] = '\0';

				/* OK, allocate memory for the result and return */
				ret = KMALLOC(strlen(attrEntry) + 1);
				strcpy(ret, attrEntry);
				KFREE(mfdata);
				return ret;
			}
			attrEntry = mfdata + i + 1;
		}
	}
	KFREE(mfdata);
	return (NULL);
}


/* Partially Handle Manifest Class-Path attribute.  It will be better to
   handle that in a ClassLoader.  */
static void
handleManifestClassPath (classpathEntry *ptr)
{
	char *mfclasspath0,  *mfclasspath;
	char *pathname;

	classpathEntry* newEntry;
	int last_one;

	/* See if there's a Class-Path attribute in the Manifest and have a
	   look there */
	mfclasspath0 = getManifestMainAttribute(ptr->u.jar, "Class-Path");
	if (mfclasspath0 == 0)
		return;
	mfclasspath = mfclasspath0;

	DBG(CLASSLOOKUP,
	    dprintf("%s: Manifest 'Class-Path' attribute is '%s'\n",
		    ptr->path, mfclasspath);
	    );

	last_one = 0;
	pathname = mfclasspath;
	while (*mfclasspath) {
		while (*mfclasspath != ' ' &&
		       *mfclasspath != '\t' &&
		       *mfclasspath != '\0')
			++mfclasspath;

		if(*mfclasspath == '\0')
			last_one = 1;
		else
			*mfclasspath = '\0';

		/* Insert manifest classpath entries */

		newEntry = KMALLOC(sizeof(classpathEntry));
		newEntry->u.jar = NULL;

		/* Manifest classpath entries can be either absolute or
		   relative to the location of the jar file. */
		if (pathname[0] != file_separator[0]) {
			int len;

			/* Path is relative. First, get the directory of
			   the jar file */
			len = strlen(ptr->path);
			while (len > 0 && ptr->path[len - 1] != file_separator[0])
				len--;

			if (len != 0) {
				newEntry->path = KMALLOC(len + strlen(file_separator) + strlen(pathname));
				strncpy (newEntry->path, ptr->path, (size_t)(len - 1));
				sprintf (newEntry->path + len - 1, "%s%s",
					 file_separator, pathname);
			}
			else {
				newEntry->path = KMALLOC(strlen(pathname) + 1);
				strcpy (newEntry->path, pathname);
			}
		}
		else {
			/* Path is absolute */
			newEntry->path = KMALLOC(strlen(pathname) + 1);
			strcpy (newEntry->path, pathname);
		}

		/* Check if newEntry is a valid classpath element add it to
                   classpath if it wasn't already there. */
		newEntry->type = getClasspathType (newEntry->path);
		if ((newEntry->type != CP_INVALID) &&
		    !isEntryInClasspath (newEntry->path)) {
DBG(CLASSLOOKUP,	dprintf("Entry '%s' added to classpath\n", newEntry->path); );
			newEntry->next = ptr->next;
			ptr->next = newEntry;
		}
		else {
			KFREE(newEntry->path);
			KFREE(newEntry);
		}

		/* Go to the following classpath element */
		if (!last_one) {
			/* Skip '\0' inserted at the blank space */
			++mfclasspath;
			pathname = mfclasspath;
		}
		else {
			break;
		}
	}
	KFREE(mfclasspath0);
}
#endif

#ifdef ENABLE_BINRELOC
static char discoveredClassHome[MAXPATHLEN];
#endif

/*
 * Guess the path to kaffe/jre/lib by going upwards from the current
 * module's absolute location. returns the first directory that
 * contains a readable file named "rt.jar", or NULL if the path cannot
 * be discovered.
 */
static char* 
discoverClassHome(void)
{
#ifdef ENABLE_BINRELOC
   if (strlen(file_separator) == 1) {

      char* p;
      const char* referenceName = "rt.jar";

      strcpy(discoveredClassHome, SELFPATH);

      while ((p = strrchr(discoveredClassHome, file_separator[0]))) {
         if (p + 1 + strlen(referenceName) 
             < discoveredClassHome + sizeof discoveredClassHome) {
            strcpy(p + 1, referenceName);
            if (0 == access(discoveredClassHome, R_OK)) {
               *p = '\0';                    
               return discoveredClassHome;
            }
         }
         *p = '\0';                    
      }
   }
   else {
      fprintf(stderr, "WARNING: file_separator not a single character, unable to discover lib directory\n");
   }
#endif
   
   return NULL;
}
