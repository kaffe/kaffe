/*
 * java.text.DateFormat.c
 *
 * Copyright (c) 1996, 1997, 1998
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "lib-license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include <native.h>
#include <time.h>

#if defined(HAVE_STRFTIME)
#define SIMPLETIME(S, T) strftime(S, sizeof(S), "%a %h %d %H:%M:%S %Z %Y", T)
#define LOCALETIME(S, T) strftime(S, sizeof(S), "%a %h %d %H:%M:%S %Y", T)
#define GMTTIME(S, T)    strftime(S, sizeof(S), "%d %h %Y %H:%M:%S GMT", T)
#else
#define SIMPLETIME(S, T) strcpy(S, asctime(T)); S[strlen(S)-1] = 0
#define LOCALETIME(S, T) strcpy(S, asctime(T)); S[strlen(S)-1] = 0
#define GMTTIME(S, T)	 strcpy(S, asctime(T)); S[strlen(S)-1] = 0
#endif

Hjava_lang_String*
java_text_DateFormat_format0(jlong time, int fmt)
{
	time_t date;
	char str[64];

	date = time / 1000;
	switch (fmt) {
	case 0:
		SIMPLETIME(str, localtime(&date));
		break;
	case 1:
		LOCALETIME(str, localtime(&date));
		break;
	case 2:
	default:
		GMTTIME(str, localtime(&date));
		break;
	}
	return (makeJavaString(str, strlen(str)));
}
