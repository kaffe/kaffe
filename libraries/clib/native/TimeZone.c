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
	struct tm *tempTimeStruct;
	time_t tempTimeArithm;
	char *tempZoneName;

	tempTimeArithm = 0;
	tempTimeStruct = localtime (&tempTimeArithm);
	tempZoneName = tempTimeStruct->tm_zone;
	assert(tempZoneName != NULL);
	return stringC2Java(tempZoneName);
#else
	return NULL;
#endif
}
