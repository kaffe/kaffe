/*
 * sigs.c
 * Translate a class into stubs.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "config-mem.h"


static char fp[100];

/*
 * Translate signature into argument count.
 */
char*
translateSig(char* str, char** nstr, int* argp)
{
	int j;
	int arg;
	int k;

	switch (*str++) {
	case 'L':
		arg = 1;
		strcpy(fp, "struct H");
		k = strlen(fp);
		for (j = 0; str[j] != ';'; j++, k++) {
			if (str[j] == '/') {
				fp[k] = '_';
			}
			else {
				fp[k] = str[j];
			}
		}
		fp[k] = '*';
		fp[k+1] = 0;
		str += j + 1;
		break;
	case '[':
		arg = 1;
		switch (*str++) {
		case 'B':
			strcpy(fp, "HArrayOfByte*");
			break;
		case 'C':
			strcpy(fp, "HArrayOfChar*");
			break;
		case 'D':
			strcpy(fp, "HArrayOfDouble*");
			break;
		case 'F':
			strcpy(fp, "HArrayOfFloat*");
			break;
		case 'I':
		case 'Z':	/* Bool */
			strcpy(fp, "HArrayOfInt*");
			break;
		case 'S':
			strcpy(fp, "HArrayOfShort*");
			break;
		case 'J':
			strcpy(fp, "HArrayOfLong*");
			break;
		case '[':
			strcpy(fp, "HArrayOfArray*");
			/* Skip rest of definition */
			while (*str == '[') {
				str++;
			}
			if (*str++ == 'L') {
				while (*str++ != ';')
					;
			}
			break;
		case 'L':
			strcpy(fp, "HArrayOfObject*");
			while (*str++ != ';')
				;
			break;
		}
		break;
	case 'B':
		arg = 1;
		strcpy(fp, "jbyte");
		break;
	case 'C':
		arg = 1;
		strcpy(fp, "jchar");
		break;
	case 'D':
		arg = 2;
		strcpy(fp, "jdouble");
		break;
	case 'F':
		arg = 1;
		strcpy(fp, "jfloat");
		break;
	case 'I':
		arg = 1;
		strcpy(fp, "jint");
		break;
	case 'J':
		arg = 2;
		strcpy(fp, "jlong");
		break;
	case 'S':
		arg = 1;
		strcpy(fp, "jshort");
		break;
	case 'Z':
		arg = 1;
		strcpy(fp, "jbool");
		break;
	case 'V':
		arg = 0;
		strcpy(fp, "void");
		break;
	default:
		abort();
	}

	if (argp != 0) {
		(*argp) += arg;
	}
	if (nstr != 0) {
		(*nstr) = str;
	}

	return (fp);
}

/*
 * Translate signature to union type.
 */
char*
translateSigType(char* str, char* type)
{
	switch (*str++) {
	case 'L':
		type[0] = 'p';
		while (*str != ';') {
			str++;
		}
		str++;
		break;
	case '[':
		type[0] = 'p';
		if (*str++ == 'L') {
			while (*str != ';') {
				str++;
			}
			str++;
		}
		break;
	case 'B':
		type[0] = 'b';
		break;
	case 'C':
		type[0] = 'c';
		break;
	case 'I':
		type[0] = 'i';
		break;
	case 'S':
		type[0] = 's';
		break;
	case 'Z':
		type[0] = 'z';
		break;
	case 'D':
		type[0] = 'd';
		break;
	case 'F':
		type[0] = 'f';
		break;
	case 'J':
		type[0] = 'l';
		break;
	case 'V':
		type[0] = 'v';
		break;
	}
	return (str);
}
