/*
 * java.lang.System.c
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "lib-license.terms" for information on usage and redistribution 
 * of this file. 
 */

#define	DBG(s)

#include "config.h"
#include "config-std.h"
#include "config-io.h"
#include "config-mem.h"
#if defined(HAVE_SYS_UTSNAME_H)
#include <sys/utsname.h>
#endif
#if defined(HAVE_PWD_H)
#include <pwd.h>
#endif
#include "../../../kaffe/kaffevm/classMethod.h"
#include "../../../kaffe/kaffevm/gtypes.h"
#include "../../../kaffe/kaffevm/object.h"
#include "../../../kaffe/kaffevm/constants.h"
#include "../../../kaffe/kaffevm/access.h"
#include "../../../kaffe/kaffevm/baseClasses.h"
#include "../../../kaffe/kaffevm/support.h"
#include "../../../kaffe/kaffevm/soft.h"
#include "../../../include/system.h"
#include "defs.h"
#include "InputStream.h"
#include "PrintStream.h"
#include "System.h"
#include <native.h>

static char cwdpath[MAXPATHLEN];

extern jlong currentTime(void);
extern userProperty* userProperties;
extern Hjava_lang_Class* SystemClass;
extern char* realClassPath;
extern jint java_lang_Object_hashCode(struct Hjava_lang_Object*);

/*
 * Copy one part of an array to another.
 */
void
java_lang_System_arraycopy(struct Hjava_lang_Object* src, jint srcpos, struct Hjava_lang_Object* dst, jint dstpos, jint len)
{
	char* in;
	char* out;
	int elemsz;
	Hjava_lang_Class* sclass;
	Hjava_lang_Class* dclass;

	if (len == 0) {
		return;
	}

	sclass = OBJECT_CLASS(src);
	dclass = OBJECT_CLASS(dst);

	/* Must be arrays */
	if (! CLASS_IS_ARRAY(sclass) || ! CLASS_IS_ARRAY(dclass)) {
		SignalError("java.lang.ArrayStoreException", "");
	}

	/* Make sure we'll keep in the array boundaries */
	if ((srcpos < 0 || srcpos + len > ARRAY_SIZE(src)) ||
	    (dstpos < 0 || dstpos + len > ARRAY_SIZE(dst)) ||
	    (len < 0)) {
		SignalError("java.lang.ArrayIndexOutOfBoundsException", "");
	}

	sclass = CLASS_ELEMENT_TYPE(sclass);
	dclass = CLASS_ELEMENT_TYPE(dclass);
	elemsz = TYPE_SIZE(sclass);

	len *= elemsz;
	srcpos *= elemsz;
	dstpos *= elemsz;

	in = &((char*)ARRAY_DATA(src))[srcpos];
	out = &((char*)ARRAY_DATA(dst))[dstpos];

	if (sclass == dclass) {
#if defined(HAVE_MEMMOVE)
		memmove((void*)out, (void*)in, len);
#else
		/* Do it ourself */
#if defined(HAVE_MEMCPY)
		if (src != dst) {
			memcpy((void*)out, (void*)in, len);
		}
		else
#endif
		if (out < in) {
			/* Copy forwards */
			for (; len > 0; len--) {
				*out++ = *in++;
			}
		}
		else {
			/* Copy backwards */
			out += len;
			in += len;
			for (; len > 0; len--) {
				*--out = *--in;
			}
		}
#endif
		return;
	}

	if (CLASS_IS_PRIMITIVE(sclass) || CLASS_IS_PRIMITIVE(dclass))
		SignalError("java.lang.ArrayStoreException", "");
	
	for (; len > 0; len -= sizeof(Hjava_lang_Object**)) {
		Hjava_lang_Object* val = *(Hjava_lang_Object**)in;
		if (val != 0 && !soft_instanceof(dclass, val)) {
			SignalError("java.lang.ArrayStoreException", "");
		}
		*(Hjava_lang_Object**)out = val;
		in += sizeof(Hjava_lang_Object*);
		out += sizeof(Hjava_lang_Object*);
	}
}

/*
 * Initialise system properties to their defaults.
 */
struct Hjava_util_Properties*
java_lang_System_initProperties(struct Hjava_util_Properties* p)
{
	int r;
	char* jhome;
	char* cpath;
	char* dir;
	userProperty* prop;
#if defined(HAVE_SYS_UTSNAME_H)
	struct utsname system;
#endif
#if defined(HAVE_PWD_H)
	struct passwd* pw;
#endif

	/* Add the default properties:
	 * java.version		Java version number
	 * java.vendor          Java vendor specific string
	 * java.vendor.url      Java vendor URL
	 * java.class.version   Java class version number
	 * java.class.path      Java classpath
	 * java.home            Java installation directory
	 * java.compiler        Java JIT compiler
	 *
	 * os.name              Operating System Name
	 * os.arch              Operating System Architecture
	 * os.version           Operating System Version
	 *
	 * file.separator       File separator ("/" on Unix)
	 * path.separator       Path separator (":" on Unix)
	 * line.separator       Line separator ("\n" on Unix)
	 *
	 * user.name            User account name
	 * user.home            User home directory
	 * user.dir             User's current working directory
	 * user.language        User's language locale code
	 * user.region          User's country locale code
	 * user.timezone	User's timezone
	 *
	 * file.encoding	Character encoding for locale
	 * file.encoding.pkg	Character encoding package
	 */

	setProperty(p, "java.version", kaffe_version);
	setProperty(p, "java.vendor", kaffe_vendor);
	setProperty(p, "java.vendor.url", kaffe_vendor_url);
	setProperty(p, "java.compiler", "kaffe.jit");

	jhome = getenv(KAFFEHOME);
	if (jhome == 0) {
		jhome = ".";
	}
	setProperty(p, "java.home", jhome);

	setProperty(p, "java.class.version", kaffe_class_version);

        cpath = realClassPath;
	if (cpath == 0) {
		cpath = ".";
	}
	setProperty(p, "java.class.path", cpath);

	setProperty(p, "file.separator", file_seperator);
	setProperty(p, "path.separator", path_seperator);
	setProperty(p, "line.separator", line_seperator);

#if defined(HAVE_GETCWD)
	dir = getcwd(cwdpath, MAXPATHLEN);
#elif defined(HAVE_GETWD)
	dir = getwd(cwdpath);
#else
	dir = 0;	/* Cannot get current directory */
#endif
	if (dir == 0) {
		dir = ".";
	}
	setProperty(p, "user.dir", dir);

#if defined(HAVE_SYS_UTSNAME_H) && defined(HAVE_UNAME)
	/* Setup system properties */
	r = uname(&system);
	assert(r >= 0);
	setProperty(p, "os.name", system.sysname);
	setProperty(p, "os.arch", system.machine);
	setProperty(p, "os.version", system.version);
#else
	setProperty(p, "os.name", "Unknown");
	setProperty(p, "os.arch", "Unknown");
	setProperty(p, "os.version", "Unknown");
#endif
#if defined(HAVE_PWD_H) && defined(HAVE_GETUID)
	/* Setup user properties */
	pw = getpwuid(getuid());
	if (pw != 0) {
		setProperty(p, "user.name", pw->pw_name);
		setProperty(p, "user.home", pw->pw_dir);
	}
	else
#endif
	{
		setProperty(p, "user.name", "Unknown");
		setProperty(p, "user.home", "Unknown");
	}

	/* We should try to work this stuff out really - XXX */
	setProperty(p, "user.language", "EN");
	setProperty(p, "user.region", "US");
	setProperty(p, "user.timezone", "PST");

	setProperty(p, "file.encoding.pkg", "kaffe.io");

	/* Now process user defined properties */
	for (prop = userProperties; prop != 0; prop = prop->next) {
		setProperty(p, prop->key, prop->value);
	}

	return (p);
}

/*
 * Return current time.
 */
jlong
java_lang_System_currentTimeMillis(void)
{
	return (currentTime());
}

/*
 * Set the stdin stream.
 */
void
java_lang_System_setIn0(struct Hjava_io_InputStream* stream)
{
	*(struct Hjava_io_InputStream**)FIELD_ADDRESS(&CLASS_SFIELDS(SystemClass)[0]) = stream;
}

/*
 * Set the stdout stream.
 */
void
java_lang_System_setOut0(struct Hjava_io_PrintStream* stream)
{
	*(struct Hjava_io_PrintStream**)FIELD_ADDRESS(&CLASS_SFIELDS(SystemClass)[1]) = stream;
}

/*
 * Set the error stream.
 */
void
java_lang_System_setErr0(struct Hjava_io_PrintStream* stream)
{
	*(struct Hjava_io_PrintStream**)FIELD_ADDRESS(&CLASS_SFIELDS(SystemClass)[2]) = stream;
}

jint
java_lang_System_identityHashCode(struct Hjava_lang_Object* o)
{
       return (java_lang_Object_hashCode(o));
}
