/*
 * java.util.TimeZone.c
 *
 * Copyright (c) 2003
 *      Kaffe's team.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
#include "config.h"
#include "config-std.h"
#include <native.h>
#include "java_util_TimeZone.h"

struct Hjava_lang_String*
java_util_TimeZone_getDefaultTimeZoneId(void)
{
#ifdef HAVE_TIME_H
	static int called_tzset = 0;

	if (!called_tzset) 
	{
	  tzset();
	}
	return stringC2Java(tzname[0]);
#else
	return NULL;
#endif
}
