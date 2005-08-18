/*
 * java.util.TimeZone.c
 *
 * Copyright (c) 2003, 2004
 *      The Kaffe.org's team.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * PORTABILITY NOTES
 * tm->tm_zone is the preferred way to access time zone name
 * on other sytem the use of the external
 *       extern char *tzname[]
 * is attempted.
 * On some systems the external symbol is present in the libc but is not exported in the headers
 * I suggest in that case to put it in the respective md.h of that platform.
 */
#include "config.h"
#include "config-std.h"

#if defined (HAVE_TIME_H)
#include <time.h>
#endif

#include <native.h>
#include "java_util_VMTimeZone.h"

struct Hjava_lang_String*
java_util_VMTimeZone_getSystemTimeZoneId(void)
{
	        char *tempZoneName = NULL;
#if defined(HAVE_TM_ZONE)
	        struct tm *tempTimeStruct;
	        time_t tempTimeArithm;

	        tempTimeArithm = 0;
	        tempTimeStruct = localtime (&tempTimeArithm);
	        tempZoneName = tempTimeStruct->tm_zone;
#elif defined(HAVE_TZNAME)
	        tzset();
	        tempZoneName = tzname[0];  /* read above for this external symbol */
#endif /* defined(HAVE_TM_ZONE) */
	        assert(tempZoneName != NULL);
	        return stringC2Java(tempZoneName);
}
