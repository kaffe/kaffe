/*
 * java.lang.System.c
 *
 * Copyright (c) 1996, 1997, 1998, 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 * Copyright (c) 2003
 *	Mark J. Wielaard <mark@klomp.org>
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

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
#include <sys/time.h>
#include <time.h>
#include <ctype.h>
#include "classMethod.h"
#include "gtypes.h"
#include "object.h"
#include "constants.h"
#include "access.h"
#include "baseClasses.h"
#include "stringSupport.h"
#include "support.h"
#include "external.h"
#include "soft.h"
#include "debug.h"
#include "exception.h"
#include "system.h"
#include "defs.h"
#include "java_io_InputStream.h"
#include "java_io_PrintStream.h"
#include "java_lang_System.h"
#include "java_lang_Throwable.h"
#include <native.h>
#include <jni.h>

#include <locale.h>

static char cwdpath[MAXPATHLEN];

extern char* realClassPath;
extern char* realBootClassPath;
extern void printStackTrace(struct Hjava_lang_Throwable*,
	struct Hjava_lang_Object*, int);

/*
 * Initialise kaffe.net.www.protocole.http.HttpURLConnction defaults
 * properties.
 *
 * http.proxyHost	Host name of HTTP proxy server
 * http.proxyPort	Port number of HTTP proxy server
 * http.nonProxyHosts	Avoid proxy on these hosts
 *
 * How these properties are undocumented in the API doc.  We know
 * about them from www.icesoft.no's webpage.
 */
static void
initProxyProperties (struct Hjava_util_Properties *prop)
{
	static const char *http_prefix = "http://";
	char *proxy;
	char *start, *p;
	char c;
	unsigned int len;
	
	
	/* Build HTTP proxy properties from $http_proxy and $no_proxy
           environement variables */
	proxy = getenv("http_proxy");
	if (proxy == NULL)
		return;

	/* Check it's HTTP protocol */
	start = proxy;
	p = http_prefix;
	while (*p && (tolower((int) *start) == *p)) {
		start++;
		p++;
	}
	if (*p != '\0')
		return;
	
	/* Extract the given URL of the form
	   http://(user(:password)?@)?hostname(:port)?(/path)?  */

	/* for now, skip user and password */
	for (p = start; *p && (*p != '/'); p++) {
		if (*p == '@')
			break;
	}
	if (*p == '@')
		start = p + 1;

	/* retreive hostname */
	for (p = start; *p && (*p != '/'); p++) {
		if (*p == ':')
			break;
	}
	if (start == p)
		return;
	
	c = *p;
	*p = '\0';
	setProperty(prop, "http.proxyHost", start);
	*p = c;

	if (c == ':') {
		/* retreive port number */
		for (start = ++p; *p && (*p != '/'); p++)
			;
		if (start != p) {
			c = *p;
			*p = '\0';
			setProperty (prop, "http.proxyPort", start);
			*p = c;
		}
	}

	proxy = getenv("no_proxy");
	if (proxy == NULL)
		return;

	/* $no_proxy, it's a list of domains separated by coma
	   as in ".foo.org,.bar.org", translate it to
	   format "*.foo.org|*.bar.org".  */
	len = 0;
	for (p = proxy; *p; p++, len++) {
		if (*p == ',')
			len++;
	}
	if (len == 0)
	    return;

	/* allocate translation buffer but ignore out of memory errors */
	start = KMALLOC(len + 2);
	if (start == NULL)
		return;
	*start = '*';
	for (p = start + 1; *proxy; proxy++) {
		if (*proxy == ',') {
			*p++ = '|';
			*p++ = '*';
		}
		else {
			*p++ = *proxy;
		}
	}
	*p = '\0';
	setProperty (prop, "http.nonProxyHosts", start);
	KFREE(start);
}


/* Adapted from GNU Classpath */
struct Hjava_lang_String*
java_lang_System_getenv0(struct Hjava_lang_String *str) 
{
  const char *cname;
  const char *envname;

  cname = checkPtr(stringJava2C(str));
  if (cname == NULL)
    return NULL;

  envname = getenv(cname);
  if (envname == NULL)
    return NULL;

  return stringC2Java(envname);
}

/*
 * Initialise system properties to their defaults.
 */
struct Hjava_util_Properties*
java_lang_System_initProperties(struct Hjava_util_Properties* p)
{
	int r;
	const char* jhome;
	const char* cpath;
	const char* dir;
	userProperty* prop;
#if defined(HAVE_SYS_UTSNAME_H)
	struct utsname system_id;
#endif
#if defined(HAVE_PWD_H)
	struct passwd* pw;
#endif
	
	/* Add the default properties:
	 *
	 * Standard:
	 *
	 * java.version		Java version number
	 * java.vendor          Java vendor specific string
	 * java.vendor.url      Java vendor URL
	 * java.home            Java installation directory
	 *
	 * java.io.tmpdir	Default directory for temporary files
	 * java.library.path	Search path for native libraries
	 *
	 * java.vm.specification.version
	 *			Java Virtual Machine specification version
	 * java.vm.specification.vendor
	 *			Java Virtual Machine specification vendor
	 * java.vm.specification.name
	 *			Java Virtual Machine specification name
	 * java.vm.version	Java Virtual Machine implementation version
	 * java.vm.vendor	Java Virtual Machine implementation vendor
	 * java.vm.name		Java Virtual Machine implementation name
	 *
	 * java.specification.version
	 *			Java Runtime Environment specification version
	 * java.specification.vendor
	 *			Java Runtime Environment specification vendor
	 * java.specification.name
	 *			Java Runtime Environment specification name
	 *
	 * java.class.version   Java class version number
	 * java.class.path      Java classpath
	 *
	 * java.runtime.name    Alias for java.vm.name
	 * java.runtime.version Alias for java.vm.version
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
	 *
	 * Others:
	 *
	 * java.compiler        Java JIT compiler
	 * java.ext.dirs        Path of extension directory or directories
	 * file.encoding	Character encoding for locale
	 * file.encoding.pkg	Character encoding package
	 * kaffe.compiler	Default java compiler
	 * sun.boot.class.path  Bootclasspath
	 * kaffe.library.path   Search path for kaffe's native libraries
	 * gnu.cpu.endian	Specify the native endian-ness.
         * gnu.classpath.vm.shortname
         *                      Alias for java.vm.name.
	 *
	 */

	setProperty(p, "java.version", "1.4");
	setProperty(p, "java.vendor", kaffe_vendor);
	setProperty(p, "java.vendor.url", kaffe_vendor_url);
	setProperty(p, "java.vendor.url.bug", kaffe_vendor_url_bug);
	setProperty(p, "java.compiler", getEngine());

#if defined(__WIN32__)
	setProperty(p, "java.io.tmpdir", "C:\\TEMP");
#else
	setProperty(p, "java.io.tmpdir", "/tmp");	/* XXX or /var/tmp?? */
#endif

	setProperty(p, "java.library.path", "");
	setProperty(p, "kaffe.library.path", getLibraryPath());

	setProperty(p, "java.vm.specification.version",
		  kaffe_vm_specification_version);
	setProperty(p, "java.vm.specification.vendor", 
		  kaffe_vm_specification_vendor);
	setProperty(p, "java.vm.specification.name", 
		  kaffe_vm_specification_name);
	setProperty(p, "java.vm.version", kaffe_vm_version);
	setProperty(p, "java.vm.vendor", kaffe_vm_vendor);
	setProperty(p, "java.vm.name", kaffe_vm_name);
	setProperty(p, "java.specification.version", 
		  kaffe_specification_version);
	setProperty(p, "java.specification.vendor", kaffe_specification_vendor);
	setProperty(p, "java.specification.name", kaffe_specification_name);
	setProperty(p, "java.class.version", kaffe_class_version);
        setProperty(p, "gnu.classpath.vm.shortname", kaffe_vm_name);

	/* Undocumented properties used by some applications */
	setProperty(p, "java.runtime.name", kaffe_vm_name);
	setProperty(p, "java.runtime.version", kaffe_vm_version);

	jhome = getenv(KAFFEHOME);
	if (jhome == 0) {
		jhome = ".";
	}
	setProperty(p, "java.home", jhome);

        cpath = realClassPath;
	if (cpath == 0) {
		cpath = ".";
	}
	setProperty(p, "java.class.path", cpath);

	setProperty(p, "sun.boot.class.path", realBootClassPath); 

	setProperty(p, "java.ext.dirs", "");

	setProperty(p, "file.separator", file_separator);
	setProperty(p, "path.separator", path_separator);
	setProperty(p, "line.separator", line_separator);

#if defined(WORDS_BIGENDIAN)
	setProperty(p, "gnu.cpu.endian", "big");
#else
	setProperty(p, "gnu.cpu.endian", "little");
#endif

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
	r = uname(&system_id);
	assert(r >= 0);
	setProperty(p, "os.name", system_id.sysname);
	setProperty(p, "os.arch", system_id.machine);
	setProperty(p, "os.version", system_id.release);
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
	
	{
	char *tmp;
	char *locale;
	char lang[3];

#if defined(HAVE_LC_MESSAGES)
	locale = setlocale (LC_MESSAGES, "");

	tmp = strchr (locale, '_');
#else
	tmp = NULL;
#endif
	if (tmp != NULL) {
		lang[2] = '\0';

		strncpy (lang, locale, 2);
		setProperty (p, "user.language", lang);

		strncpy (lang, tmp+1, 2);
		setProperty (p, "user.region", lang);
	} else {
		/* locale not set or not of the form <lang>_<region> */
		setProperty (p, "user.language", "en");
		setProperty (p, "user.region", "US");
	}	
	}

	setProperty(p, "file.encoding.pkg", "kaffe.io");
	setProperty(p, "file.encoding", "Default");

	/* Define the default java compiler - this should be configured
	 * somehow.
	 */
	setProperty(p, "kaffe.compiler", "kjc");

	initProxyProperties (p);
	
	/* Now process user defined properties */
	for (prop = userProperties; prop != 0; prop = prop->next) {
		setProperty(p, prop->key, prop->value);
	}

	prop = userProperties;
	while (prop != 0) {
		userProperty *nextProperty = prop->next;

		free(prop->key);
		free(prop);
		prop = nextProperty;
	}
	userProperties = NULL;

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
Java_java_lang_System_setIn0(JNIEnv *env, jclass system_cls, jobject stream)
{
	jfieldID in = (*env)->GetStaticFieldID(env, system_cls, 
					"in", "Ljava/io/InputStream;");
	assert(in != NULL);
	(*env)->SetStaticObjectField(env, system_cls, in, stream);
}

/*
 * Set the stdout stream.
 */
void
Java_java_lang_System_setOut0(JNIEnv *env, jclass system_cls, jobject stream)
{
	jfieldID out = (*env)->GetStaticFieldID(env, system_cls, 
					"out", "Ljava/io/PrintStream;");
	assert(out != NULL);
	(*env)->SetStaticObjectField(env, system_cls, out, stream);
}

/*
 * Set the error stream.
 */
void
Java_java_lang_System_setErr0(JNIEnv *env, jclass system_cls, struct Hjava_io_PrintStream* stream)
{
	jfieldID err = (*env)->GetStaticFieldID(env, system_cls, 
					"err", "Ljava/io/PrintStream;");
	assert(err != NULL);
	(*env)->SetStaticObjectField(env, system_cls, err, stream);
}

jint
java_lang_System_identityHashCode(struct Hjava_lang_Object* o)
{
  /* Hash code is object's address */
  return ((jint)(jword)o);
}

/*
 * See java/lang/System.java for info on these two routines.
 */
void
java_lang_System_debug(struct Hjava_lang_String *str)
{
	char *s;

	s = checkPtr(stringJava2C(str));
	dprintf("%s\n", s);
	KFREE(s);
}

void
java_lang_System_debugE(struct Hjava_lang_Throwable *t)
{
	Hjava_lang_String *msg;
	const char *cname;
	char *s;

	cname = CLASS_CNAME(OBJECT_CLASS(&t->base));
	msg = unhand(t)->detailMessage;

	if (msg) {
		s = checkPtr(stringJava2C(msg));
		dprintf("%s: %s\n", cname, s);
		KFREE(s);
	} else {
		dprintf("%s\n", cname);
	}
	printStackTrace(t, 0, 1);
}

void
java_lang_System_arraycopy(struct Hjava_lang_Object* src, jint srcpos,
			   struct Hjava_lang_Object* dst, jint dstpos,
			   jint len) {
	char* in; 	 
	char* out; 	 
	int elemsz; 	 
	Hjava_lang_Class* sclass; 	 
	Hjava_lang_Class* dclass;

	if (src==0 || dst==0) {
		throwException (NullPointerException);
	}

	if (len == 0) { 	 
		return; 	 
	} 	 

	sclass = OBJECT_CLASS(src); 	 
	dclass = OBJECT_CLASS(dst);

	/* Must be arrays */ 	 
	if (!CLASS_IS_ARRAY(sclass) || !CLASS_IS_ARRAY(dclass)) { 	 
		throwException (ArrayStoreException);
	} 	 

	/* Make sure we'll keep in the array boundaries */ 	 
	if ((srcpos < 0 || srcpos + len > ARRAY_SIZE(src)) || 	 
	    (dstpos < 0 || dstpos + len > ARRAY_SIZE(dst)) || 	 
	    (len < 0)) { 	 
		throwException (ArrayIndexOutOfBoundsException);
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
		memmove((void*)out, (void*)in, (size_t)len); 	 
#else 	 
		/* Do it ourself */ 	 
#if defined(HAVE_MEMCPY) 	 
		if (src != dst) { 	 
			memcpy((void*)out, (void*)in, (size_t)len); 	 
		} else 	 
#endif 	 
		if (out < in) { 	 
			/* Copy forwards */ 	 
			for (; len > 0; len--) { 	 
				*out++ = *in++; 	 
			} 	 
		} else { 	 
			/* Copy backwards */ 	 
			out += len; 	 
			in += len; 	 
			for (; len > 0; len--) { 	 
				*--out = *--in; 	 
			} 	 
		} 	 
#endif 	 
	} else {
		if (CLASS_IS_PRIMITIVE(sclass) || CLASS_IS_PRIMITIVE(dclass)) {
			throwException (ArrayStoreException);
		}

		for (; len > 0; len -= sizeof(Hjava_lang_Object*)) { 	 
			Hjava_lang_Object* val = *(Hjava_lang_Object**)in; 	 
			if (val != 0 && !instanceof(dclass, OBJECT_CLASS(val))) { 	 
				throwException (ArrayStoreException);
			}
			*(Hjava_lang_Object**)out = val; 	 
			in += sizeof(Hjava_lang_Object*); 	 
			out += sizeof(Hjava_lang_Object*); 	 
		}
	}
}
