/*
 * stringParsing.c
 * A handy string parsing function.
 *
 * Copyright (c) 2000, 2001, 2002, 2003 The University of Utah and the Flux Group.
 * All rights reserved.
 *
 * @JANOSVM_KAFFE_MISC_LICENSE@
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "stringParsing.h"

/* We use this in JSI stuff too, and we don't want to drag in kaffe goo. */
#include "gtypes.h"
#include "gc.h"
#include "kaffe/jmalloc.h"
#include "defs.h"

#define spMalloc(x) gc_malloc(x, KGC_ALLOC_FIXED)
#define spFree(x) gc_free(x)

int cmpPStrStr(parsedString *ps, char *str)
{
	char *ps_pos, *ps_end;
	int retval = 0;

	assert(ps != 0);
	assert(str != 0);
	
	for( ps_pos = ps->data, ps_end = ps->data + ps->len;
	     !retval && (ps_pos < ps_end) && *str;
	     ps_pos++, str++ )
	{
		if( *ps_pos != *str )
		{
			/* XXX */
			retval = *ps_pos - *str;
		}
	}
	if( !retval && ((ps_pos != ps_end) || (*str != 0)) )
	{
		/* XXX */
		retval = 1;
	}
	return( retval );
}

typedef struct _parseValue {
	int type;
	char *value;
	union {
		void *p;
		char *c;
		short *s;
		int *i;
		float *f;
		double *d;
		long long *ll;
		parsedString *ps;
	} storage;
} parseValue;

typedef struct _parseFrame {
	struct _parseFrame *prev;
	int op;
	parseValue pv;
	stringScript *script_pos;
	int values_pos;
	va_list args;
} parseFrame;

#define PREALLOC_FRAMES 8

typedef struct _parseStack {
	struct _parseFrame *top;
	struct _parseFrame frames[PREALLOC_FRAMES];
	int depth;
} parseStack;

static
int pushFrame(parseErrorInfo *pe,
	      parseStack *ps,
	      int op,
	      stringScript *script_pos,
	      int values_pos,
	      va_list args)
{
	parseFrame *pf;
	int retval = 0;

	assert(ps != 0);

	ps->depth++;
	if( ps->depth < PREALLOC_FRAMES )
	{
		pf = &ps->frames[ps->depth];
	}
	else
	{
		pf = spMalloc(sizeof(parseFrame));
	}
	if( pf )
	{
		pf->prev = ps->top;
		pf->op = op;
		pf->pv.type = SPO_Noop;
		pf->script_pos = script_pos;
		pf->values_pos = values_pos;
		VA_LIST_COPY(pf->args, args);
		ps->top = pf;
		retval = 1;
	}
	else
	{
		pe->position = NULL;
		pe->op = op;
	}
	return( retval );
}

static
void popFrame(parseStack *ps)
{
	parseFrame *prev;

	assert(ps != NULL);

	prev = ps->top->prev;
	if( ps->depth >= PREALLOC_FRAMES )
		spFree(ps->top);
	ps->depth--;
	ps->top = prev;
}

static
void cutFrames(parseStack *ps)
{
	assert(ps != NULL);
	
	while( ps->depth >= PREALLOC_FRAMES )
	{
		popFrame(ps);
	}
}

static
int storeValue(parseErrorInfo *pe, parseValue *pv, char *str, int clear)
{
	int retval = 1;

	assert(pv != 0);
	assert(str != 0);
	
	switch( pv->type )
	{
	case SPO_String:
		pv->storage.ps->data = pv->value;
		pv->storage.ps->len = str - pv->value;
		break;
	case SPO_NonEmptyString:
		if( (str - pv->value) > 0 )
		{
			pv->storage.ps->data = pv->value;
			pv->storage.ps->len = str - pv->value;
		}
		else
		{
			retval = 0;
		}
		break;
	case SPO_Character:
		if( (str - pv->value) == 1 )
			*pv->storage.c = pv->value[0];
		else
			retval = 0;
		break;
	case SPO_Byte:
		if( strncasecmp(pv->value, "0x", 2) == 0 )
		{
			int c;
			
			if( sscanf(&pv->value[2], "%x", &c) != 1 )
				retval = 0;
			else
				*pv->storage.c = (char)(c & 0xff);
		}
		else
		{
			int c;
			
			if( sscanf(pv->value, "%d", &c) != 1 )
				retval = 0;
			else
				*pv->storage.c = (char)(c & 0xff);
		}
		break;
	case SPO_HexByte:
		{
			int c;
			
			if( !(((strncasecmp(pv->value, "0x", 2) == 0) &&
			       (sscanf(&pv->value[2], "%x", &c) == 1)) ||
			      (sscanf(pv->value, "%x", &c) == 1)) )
			{
				retval = 0;
			}
			else
			{
				*pv->storage.c = (char)(c & 0xff);
			}
		}
		break;
	case SPO_Short:
		if( strncasecmp(pv->value, "0x", 2) == 0 )
		{
			if( sscanf(&pv->value[2], "%hx", pv->storage.s) != 1 )
				retval = 0;
		}
		else
		{
			if( sscanf(pv->value, "%hd", pv->storage.s) != 1 )
				retval = 0;
		}
		break;
	case SPO_HexShort:
		if( !(((strncasecmp(pv->value, "0x", 2) == 0) &&
		       (sscanf(&pv->value[2], "%hx", pv->storage.s) == 1)) ||
		      (sscanf(pv->value, "%hx", pv->storage.s) == 1)) )
		{
			retval = 0;
		}
		break;
	case SPO_Integer:
		if( strncasecmp(pv->value, "0x", 2) == 0 )
		{
			if( sscanf(&pv->value[2], "%x", pv->storage.i) != 1 )
				retval = 0;
		}
		else
		{
			if( sscanf(pv->value, "%d", pv->storage.i) != 1 )
				retval = 0;
		}
		break;
	case SPO_LongInteger:
		if( strncasecmp(pv->value, "0x", 2) == 0 )
		{
			if( sscanf(&pv->value[2], "%qx", pv->storage.ll) != 1 )
				retval = 0;
		}
		else
		{
			if( sscanf(pv->value, "%qd", pv->storage.ll) != 1 )
				retval = 0;
		}
		break;
	case SPO_HexInteger:
		if( !(((strncasecmp(pv->value, "0x", 2) == 0) &&
		       (sscanf(&pv->value[2], "%x", pv->storage.i) == 1)) ||
		      (sscanf(pv->value, "%x", pv->storage.i) == 1)) )
		{
			retval = 0;
		}
		break;
	case SPO_HexLongInteger:
		if( !(((strncasecmp(pv->value, "0x", 2) == 0) &&
		       (sscanf(&pv->value[2], "%qx", pv->storage.ll) == 1)) ||
		      (sscanf(pv->value, "%qx", pv->storage.ll) == 1)) )
		{
			retval = 0;
		}
		break;
	case SPO_Float:
		if( sscanf(pv->value, "%f", pv->storage.f) != 1 )
		{
			retval = 0;
		}
		break;
	case SPO_Double:
		if( sscanf(pv->value, "%lf", pv->storage.d) != 1 )
		{
			retval = 0;
		}
		break;
	case SPO_Count:
		pv->storage.i[0]++;
		break;
	default:
		break;
	}
	if( clear )
		pv->type = SPO_Noop;
	if( retval == 0 )
	{
		pe->position = pv->value;
		pe->op = pv->type;
	}
	return( retval );
}

static
void skipBlock(stringScript *script, void **values,
	       stringScript **script_pos, int *values_pos, va_list *args)
{
	int op, skip_depth = 0;
	/* Used to quiet the compiler */
	char *c_ptr;
	void *v_ptr;

	if( script )
	{
		op = (*script_pos)->op;
	}
	else
	{
		op = va_arg(*args, int);
	}
	while( skip_depth >= 0 )
	{
		switch( op )
		{
		case SPO_End:
			skip_depth--;
			(*script_pos)++;
			break;
		case SPO_Noop:
			(*script_pos)++;
			break;
		case SPO_Cond:
			if( script )
			{
				(*script_pos) = STRING_SCRIPT_NEXT(*script_pos,
								   1);
			}
			else
			{
				c_ptr = va_arg(*args, char *);
			}
			skip_depth++;
			break;
		case SPO_Do:
		case SPO_NotEmpty:
			skip_depth++;
			(*script_pos)++;
			break;
		case SPO_Character:
		case SPO_Byte:
		case SPO_HexByte:
		case SPO_Short:
		case SPO_HexShort:
		case SPO_Integer:
		case SPO_LongInteger:
		case SPO_HexInteger:
		case SPO_HexLongInteger:
		case SPO_Float:
		case SPO_Double:
		case SPO_Count:
		case SPO_NonEmptyString:
		case SPO_String:
			if( values )
				(*values_pos)++;
			else
				v_ptr = va_arg(*args, void *);
			(*script_pos)++;
			break;
		case SPO_While:
			if( script )
			{
				(*script_pos) = STRING_SCRIPT_NEXT(*script_pos,
								   2);
			}
			else
			{
				c_ptr = va_arg(*args, char *);
				c_ptr = va_arg(*args, char *);
			}
			break;
		case SPO_Expect:
			if( script )
			{
				(*script_pos) = STRING_SCRIPT_NEXT(*script_pos,
								   1);
			}
			else
			{
				c_ptr = va_arg(*args, char *);
			}
			break;
		case SPO_WhileSpace:
		case SPO_ExpectSpace:
			(*script_pos)++;
			break;
		case SPO_Handle:
			if( script )
			{
				(*script_pos) = STRING_SCRIPT_NEXT(*script_pos,
								   2);
			}
			else
			{
				v_ptr = va_arg(*args, void *);
				v_ptr = va_arg(*args, void *);
			}
			break;
		case SPO_OneOf:
			if( script )
			{
				(*script_pos) = STRING_SCRIPT_NEXT(*script_pos,
								   1);
			}
			else
			{
				c_ptr = va_arg(*args, char *);
			}
			break;
		default:
			break;
		}
		if( skip_depth >= 0 )
		{
			if( script )
				op = (*script_pos)->op;
			else
				op = va_arg(*args, int);
		}
	}
}

static
char *skipChars(char *str, char *str_end)
{
	assert(str != 0);
	assert(str_end != 0);

	while( (str < str_end) && !isspace(*str) )
	{
		str++;
	}
	return( str );
}

static
char *skipSpace(char *str, char *str_end)
{
	assert(str != 0);
	assert(str_end != 0);

	while( (str < str_end) && isspace(*str) )
	{
		str++;
	}
	return( str );
}

static
int parseString_private(parseErrorInfo *pe,
			parsedString *subString,
			stringScript *script,
			void **values,
			int op,
			va_list args)
{
	char *str, *str_end, *str_ptr = NULL, *term_ptr, *new_pos = NULL;
	int values_pos = 0, len, retval = 1;
	stringScript *script_pos = script;
	parseValue pv;
	parseStack ps;

#ifdef VA_LIST_IS_ARRAY
	/* Use temporary copy of args on platforms where va_list
	 * is an array.
	 *
	 * We sometimes need to pass the address of a va_list to
	 * another function. C Standard mandates array types in
	 * prototypes to be silently coerced into pointers to base
	 * objects. If va_list is an array, this results in the
	 * receiving function expecting a pointer to a va_list array
	 * member, but getting a pointer to a pointer instead when
	 * we pass &args.
	 *
	 * Copying the va_list into a temporary buffer, and copying
	 * it back 'undoes' the coercion.
	 *
	 * A longer explanation was posted by Graeme Peterson on the
	 * GDB mailing list on 2002-04-15.
	 */

        va_list     tmp_args;
        VA_LIST_COPY (tmp_args, args);
#endif

	assert(subString != NULL);

	str = subString->data;
	str_end = subString->data + subString->len;
	pv.type = SPO_Noop;
	ps.top = &ps.frames[0];
	ps.top->op = SPO_Noop;
	ps.depth = 0;
	if( script )
	{
		op = script_pos->op;
	}
	while( retval && (ps.depth >= 0) )
	{
		switch( op )
		{
		case SPO_End:
			if( ps.top->op == SPO_Do )
				VA_LIST_COPY(args, ps.top->args);
			else
				popFrame(&ps);
			script_pos++;
			break;
		case SPO_Expect:
			if( script )
			{
				str_ptr = script_pos->args[0];
				script_pos = STRING_SCRIPT_NEXT(script_pos, 1);
			}
			else
			{
				str_ptr = va_arg(args, char *);
			}
			if( (str = strstr(str, str_ptr)) &&
			    (str < str_end) )
			{
				retval = storeValue(pe, &pv, str, 1);
				str += strlen(str_ptr);
			}
			else
			{
				pe->position = str_end;
				pe->op = op;
				pe->args[0] = str_ptr;
				retval = 0;
			}
			break;
		case SPO_ExpectSpace:
			str = skipChars(str, str_end);
			retval = storeValue(pe, &pv, str, 1);
			str = skipSpace(str, str_end);
			script_pos++;
			break;
		case SPO_Character:
		case SPO_Byte:
		case SPO_HexByte:
		case SPO_Short:
		case SPO_HexShort:
		case SPO_Integer:
		case SPO_LongInteger:
		case SPO_HexInteger:
		case SPO_HexLongInteger:
		case SPO_Float:
		case SPO_Double:
		case SPO_Count:
		case SPO_String:
		case SPO_NonEmptyString:
			pv.type = op;
			pv.value = str;
			if( values )
				pv.storage.p = values[values_pos++];
			else
				pv.storage.p = va_arg(args, void *);
			script_pos++;
			break;
		case SPO_OneOf:
			if( script )
			{
				str_ptr = script_pos->args[0];
				script_pos = STRING_SCRIPT_NEXT(script_pos,
								1);
			}
			else
			{
				str_ptr = va_arg(args, char *);
			}
			if( (new_pos = strpbrk(str, str_ptr)) &&
			    (new_pos < str_end) )
			{
				retval = storeValue(pe, &pv, new_pos, 1);
				retval = retval && pushFrame(pe,
							     &ps,
							     op,
							     script_pos,
							     values_pos,
							     args);
				str = new_pos;
			}
			else
			{
#ifdef VA_LIST_IS_ARRAY
			        VA_LIST_COPY (args, tmp_args);
#endif
				skipBlock(script, values,
					  &script_pos, &values_pos, &args);
			}
			break;
		case SPO_Do:
			ps.top->pv = pv;
			retval = pushFrame(pe,
					   &ps,
					   op,
					   script_pos,
					   values_pos,
					   args);
			script_pos++;
			break;
		case SPO_While:
			if( script )
			{
				str_ptr = script_pos->args[0];
				term_ptr = script_pos->args[1];
				script_pos = STRING_SCRIPT_NEXT(script_pos,
								2);
			}
			else
			{
				str_ptr = va_arg(args, char *);
				term_ptr = va_arg(args, char *);
			}
			if( (new_pos = strstr(str, str_ptr)) &&
			    (new_pos < str_end) )
			{
				retval = storeValue(pe,
						    &ps.top->prev->pv,
						    new_pos,
						    0);
				retval = retval && storeValue(pe,
							      &pv,
							      new_pos,
							      1);
				str = new_pos + strlen(str_ptr);
			}
			else if( ((term_ptr[0] == '\0') ?
				  (new_pos = str_end) :
				  (new_pos = strstr(str, term_ptr))) &&
				 (new_pos <= str_end) )
			{
				retval = storeValue(pe, &pv, new_pos, 1);
				str = new_pos + strlen(term_ptr);
				ps.top->op = SPO_Noop;
			}
			else
			{
				pe->position = str_end;
				pe->op = op;
				pe->args[0] = str_ptr;
				pe->args[1] = term_ptr;
				retval = 0;
			}
			break;
		case SPO_SkipSpace:
			str = skipSpace(str, str_end);
			break;
		case SPO_WhileSpace:
			new_pos = skipChars(str, str_end);
			retval = storeValue(pe, &pv, new_pos, 1);
			str = skipSpace(new_pos, str_end);
			if( str == str_end )
			{
				ps.top->op = SPO_Noop;
			}
			script_pos++;
			break;
		case SPO_Cond:
			if( script )
			{
				str_ptr = script_pos->args[0];
				script_pos = STRING_SCRIPT_NEXT(script_pos,
								1);
			}
			else
			{
				str_ptr = va_arg(args, char *);
			}
			len = strlen(str_ptr);
			new_pos = str;
			if( ((pv.type == SPO_Noop) &&
			     !strncmp(str, str_ptr, (size_t)len)) ||
			    ((pv.type != SPO_Noop) &&
			     (new_pos = strstr(str, str_ptr)) &&
			     (new_pos < str_end)) )
			{
				if( ps.top->op == SPO_OneOf )
				{
					ps.top->op = SPO_Noop;
				}
				else
				{
					retval = storeValue(pe,
							    &pv,
							    new_pos,
							    1);
				}
				retval = retval && pushFrame(pe,
							     &ps,
							     op,
							     script_pos,
							     values_pos,
							     args);
				str = new_pos + len;
			}
			else
			{
#ifdef VA_LIST_IS_ARRAY
			        VA_LIST_COPY (args, tmp_args);
#endif

				skipBlock(script, values,
					  &script_pos, &values_pos, &args);
			}
			break;
		case SPO_Handle:
			{
				int (*handler)(void *arg);
				void *v_ptr;

				if( script )
				{
					handler = (int (*)(void *))
						script_pos->args[0];
					v_ptr = script_pos->args[1];
					script_pos =
						STRING_SCRIPT_NEXT(script_pos,
								   2);
				}
				else
				{
					handler = (int (*)(void *))
						va_arg(args, void *);
					v_ptr = va_arg(args, void *);
				}
				if( !(retval = handler(v_ptr)) )
				{
					pe->position = str;
					pe->op = op;
					pe->args[0] = handler;
					pe->args[1] = v_ptr;
				}
			}
			break;
		case SPO_NotEmpty:
			if( str < str_end )
			{
				retval = pushFrame(pe,
						   &ps,
						   op,
						   script_pos,
						   values_pos,
						   args);
			}
			else
			{
#ifdef VA_LIST_IS_ARRAY
			        VA_LIST_COPY (args, tmp_args);
#endif

				skipBlock(script, values,
					  &script_pos, &values_pos, &args);
			}
			script_pos++;
			break;
		default:
			assert(0);
			break;
		}
		if( script )
			op = script_pos->op;
		else
			op = va_arg(args, int);
	}
	if( retval && (str < str_end) )
	{
		if( pv.type == SPO_Noop )
		{
			pe->position = str;
			pe->op = SPO_Noop;
			retval = 0;
		}
		else
		{
			retval = storeValue(pe, &pv, str_end, 1);
		}
	}
	cutFrames(&ps);
	return( retval );
}

int parseString(char *str, int op, ...)
{
	parseErrorInfo pe;
	parsedString ps;
	va_list args;
	int retval;

	assert(str != 0);
	
	va_start(args, op);
	ps.data = str;
	ps.len = strlen(str);
	retval = parseString_private(&pe, &ps, NULL, NULL, op, args);
	va_end(args);
	return( retval );
}

int parseString_error(parseErrorInfo *pe, char *str, int op, ...)
{
	parsedString ps;
	va_list args;
	int retval;

	assert(str != 0);
	
	va_start(args, op);
	ps.data = str;
	ps.len = strlen(str);
	retval = parseString_private(pe, &ps, NULL, NULL, op, args);
	va_end(args);
	return( retval );
}

int parseSubString(parsedString *ps, int op, ...)
{
	parseErrorInfo pe;
	va_list args;
	int retval;

	assert(ps != 0);
	
	va_start(args, op);
	retval = parseString_private(&pe, ps, NULL, NULL, op, args);
	va_end(args);
	return( retval );
}

int parseString_script(char *str, stringScript *ss, ...)
{
	parseErrorInfo pe;
	parsedString ps;
	va_list args;
	int retval;

	va_start(args, ss);
	ps.data = str;
	ps.len = strlen(str);
	retval = parseString_private(&pe, &ps, ss, NULL, SPO_Noop, args);
	va_end(args);
	return( retval );
}

int parseString_script_values(char *str, stringScript *ss, void **values)
{
	parseErrorInfo pe;
	parsedString ps;
	int retval;
	va_list * args;

	args = KCALLOC(1, sizeof(args));

	ps.data = str;
	ps.len = strlen(str);
	retval = parseString_private(&pe, &ps, ss, values, SPO_Noop, *args);
	KFREE(args);
	return( retval );
}

char *promoteParsedString(parsedString *ps)
{
	char *retval;

	if( (retval = spMalloc((size_t)(ps->len + 1))) )
	{
		strncpy(retval, ps->data, (size_t)ps->len);
		retval[ps->len] = '\0';
	}
	return( retval );
}
