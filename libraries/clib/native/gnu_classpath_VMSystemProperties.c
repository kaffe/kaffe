/*
 * gnu.classpath.VMSystemProperties.c
 *
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 * Copyright (c) 2003
 *      Mark J. Wielaard <mark@klomp.org>
 * Copyright (c) 2004
 *      Kaffe.org contributors. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#include <jcl.h>
#include <locale.h>
#if defined(HAVE_PWD_H)
#include <pwd.h>
#endif
#include <stdlib.h>
#if defined(HAVE_SYS_TYPES_H)
#include <sys/types.h>
#endif
#if defined(HAVE_SYS_UTSNAME_H)
#include <sys/utsname.h>
#endif
#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif

#include "defs.h"
#include "external.h"
#include "gnu_classpath_VMSystemProperties.h"
#include "support.h"
#include "system.h"

static char cwdpath[MAXPATHLEN];

extern char* realClassPath;
extern char* realBootClassPath;

/* taken from libxmlj */
static void
xmljSetProperty (JNIEnv * env, jobject outputProperties,
                 jmethodID setPropertyMethodID, const char *name,
                 const char *value)
{
  if (NULL != value)
    {
      jstring nameString = (*env)->NewStringUTF (env, name);
      jstring valueString = (*env)->NewStringUTF (env, value);

      jobject prevValue = (*env)->CallObjectMethod (env, outputProperties,
                                                    setPropertyMethodID,
                                                    nameString, valueString);
      if (NULL != prevValue)
        {
          (*env)->DeleteLocalRef (env, prevValue);
        }

      (*env)->DeleteLocalRef (env, nameString);
      (*env)->DeleteLocalRef (env, valueString);
    }
}

JNIEXPORT void JNICALL
Java_gnu_classpath_VMSystemProperties_postInit(JNIEnv *env, 
					       jclass klass UNUSED, 
					       jobject outputProperties)
{
  userProperty* prop;

  /* lookup code from libxmlj */
  jclass propertiesClass;
  jmethodID setPropertyMethod;

  if (outputProperties == NULL)
    {
      return;
    }
  propertiesClass = (*env)->FindClass (env, "java/util/Properties");
  if (propertiesClass == NULL)
    {
      return;
    }
  setPropertyMethod =
    (*env)->GetMethodID (env, propertiesClass, "setProperty",
                         "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/Object;");
  if (setPropertyMethod == NULL)
    {
      return;
    }

  /* Now process user defined properties */
  for (prop = userProperties; prop != 0; prop = prop->next) 
    {
      xmljSetProperty(env, outputProperties, setPropertyMethod, 
		  prop->key, prop->value);
    }

  prop = userProperties;

  while (prop != 0) 
    {
      userProperty *nextProperty = prop->next;
    
      free(prop->key);
      free(prop);
      prop = nextProperty;
    }

  userProperties = NULL;
}

JNIEXPORT jstring JNICALL
Java_gnu_classpath_VMSystemProperties_getLocale (JNIEnv *env,
                                                 jclass klass UNUSED)
{
  const char *locale;

#if defined(HAVE_LC_MESSAGES)
  locale = setlocale (LC_MESSAGES, "");
#else
  locale = "";
#endif

  return (*env)->NewStringUTF(env, locale);
}

JNIEXPORT jstring JNICALL
Java_gnu_classpath_VMSystemProperties_getKaffeLibraryPath (JNIEnv *env,
                                                           jclass klass UNUSED)
{
  const char * path;

  path = getLibraryPath();
  return (*env)->NewStringUTF(env, path);
}

JNIEXPORT jstring JNICALL
Java_gnu_classpath_VMSystemProperties_getSunBootClassPath (JNIEnv *env,
                                                           jclass klass UNUSED)
{
  return (*env)->NewStringUTF(env, realBootClassPath);
}

JNIEXPORT jstring JNICALL
Java_gnu_classpath_VMSystemProperties_getFileSeparator (JNIEnv *env,
                                                        jclass klass UNUSED)
{
  return (*env)->NewStringUTF(env, file_separator);
}

JNIEXPORT jstring JNICALL
Java_gnu_classpath_VMSystemProperties_getLineSeparator (JNIEnv *env,
                                                        jclass klass UNUSED)
{
  return (*env)->NewStringUTF(env, line_separator);
}

JNIEXPORT jstring JNICALL
Java_gnu_classpath_VMSystemProperties_getPathSeparator (JNIEnv *env,
                                                        jclass klass UNUSED)
{
  return (*env)->NewStringUTF(env, path_separator);
}

JNIEXPORT jstring JNICALL
Java_gnu_classpath_VMSystemProperties_getJavaClassPath (JNIEnv *env,
                                                        jclass klass UNUSED)
{
  const char *cpath;

  cpath = realClassPath;
  if (cpath == 0) {
    cpath = ".";
  }

  return (*env)->NewStringUTF(env, cpath);
}

JNIEXPORT jstring JNICALL
Java_gnu_classpath_VMSystemProperties_getJavaIoTmpdir (JNIEnv *env,
                                                       jclass klass UNUSED)
{
#if defined(__WIN32__)
  return (*env)->NewStringUTF(env, "C:\\TEMP");
#else
  return (*env)->NewStringUTF(env, "/tmp");
#endif
}

JNIEXPORT jstring JNICALL
Java_gnu_classpath_VMSystemProperties_getJavaCompiler (JNIEnv *env,
                                                       jclass klass UNUSED)
{
  const char *engine;

  engine = getEngine();
  return (*env)->NewStringUTF(env, engine);
}

JNIEXPORT jstring JNICALL
Java_gnu_classpath_VMSystemProperties_getJavaHome (JNIEnv *env,
                                                   jclass klass UNUSED)
{
  const char *jhome;

  jhome = getenv(KAFFEHOME);

  if (jhome == 0) {
    jhome = ".";
  }

  return (*env)->NewStringUTF(env, jhome);
}

JNIEXPORT jstring JNICALL
Java_gnu_classpath_VMSystemProperties_getOsName (JNIEnv *env,
                                                 jclass klass UNUSED)
{
  int r;
  const char *osname;

#if defined(HAVE_SYS_UTSNAME_H)
  struct utsname system_id;
#endif

#if defined(HAVE_SYS_UTSNAME_H) && defined(HAVE_UNAME)
  r = uname(&system_id);
  assert(r >= 0);
  osname = system_id.sysname;
#else
  osname = "Unknown";
#endif

  return (*env)->NewStringUTF(env, osname);
}

JNIEXPORT jstring JNICALL
Java_gnu_classpath_VMSystemProperties_getOsArch (JNIEnv *env,
                                                 jclass klass UNUSED)
{
  int r;
  const char *osarch;

#if defined(HAVE_SYS_UTSNAME_H)
  struct utsname system_id;
#endif

#if defined(HAVE_SYS_UTSNAME_H) && defined(HAVE_UNAME)
  r = uname(&system_id);
  assert(r >= 0);
  osarch = system_id.machine;
#else
  osarch = "Unknown";
#endif

  return (*env)->NewStringUTF(env, osarch);
}

JNIEXPORT jstring JNICALL
Java_gnu_classpath_VMSystemProperties_getOsVersion (JNIEnv *env,
                                                    jclass klass UNUSED)
{
  int r;
  const char *osversion;

#if defined(HAVE_SYS_UTSNAME_H)
  struct utsname system_id;
#endif

#if defined(HAVE_SYS_UTSNAME_H) && defined(HAVE_UNAME)
   r = uname(&system_id);
   assert(r >= 0);
   osversion = system_id.release;
#else
   osversion = "Unknown";
#endif

  return (*env)->NewStringUTF(env, osversion);
}

JNIEXPORT jstring JNICALL
Java_gnu_classpath_VMSystemProperties_getUserDir (JNIEnv *env,
                                                  jclass klass UNUSED)
{
  const char *dir;

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

  return (*env)->NewStringUTF(env, dir);
}

JNIEXPORT jstring JNICALL
Java_gnu_classpath_VMSystemProperties_getUserName (JNIEnv *env,
                                                   jclass klass UNUSED)
{
  const char *name;
#if defined(HAVE_PWD_H)
  struct passwd* pw;
#endif

#if defined(HAVE_PWD_H) && defined(HAVE_GETUID)
  /* Setup user properties */
  pw = getpwuid(getuid());
  if (pw != 0) {
    name = pw->pw_name;
  }
  else
#endif
  {
    name = "Unknown";
  }

  return (*env)->NewStringUTF(env, name);
}

JNIEXPORT jstring JNICALL
Java_gnu_classpath_VMSystemProperties_getUserHome (JNIEnv *env,
                                                   jclass klass UNUSED)
{
  const char *home;
#if defined(HAVE_PWD_H)
  struct passwd* pw;
#endif

#if defined(HAVE_PWD_H) && defined(HAVE_GETUID)
  /* Setup user properties */
  pw = getpwuid(getuid());
  if (pw != 0) {
    home = pw->pw_dir;
  }
  else
#endif
  {
    home = "Unknown";
  }

  return (*env)->NewStringUTF(env, home);
}

JNIEXPORT jstring JNICALL
Java_gnu_classpath_VMSystemProperties_getGnuCpuEndian (JNIEnv *env,
                                                       jclass klass UNUSED)
{
  const char *endian;

#if defined(WORDS_BIGENDIAN)
  endian = "big";
#else
  endian = "little";
#endif

  return (*env)->NewStringUTF(env, endian);
}
