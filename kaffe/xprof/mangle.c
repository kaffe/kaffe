/*
 * mangle.c
 * Routines for doing name mangling on Java types
 *
 * Copyright (c) 2000, 2004 University of Utah and the Flux Group.
 * All rights reserved.
 *
 * This file is licensed under the terms of the GNU Public License.
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * Contributed by the Flux Research Group, Department of Computer Science,
 * University of Utah, http://www.cs.utah.edu/flux/
 */

#include "config.h"

#if defined(KAFFE_XDEBUGGING) || defined(KAFFE_XPROFILER)

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "config.h"
#include "kaffe/jmalloc.h"
#include "stringSupport.h"
#include "classMethod.h"
#include "xprofiler.h"

#include "mangle.h"

#ifndef MANGLE_GCJ
# ifdef HAVE_GCJ_SUPPORT
#  define MANGLE_GCJ 1
# else
#  define MANGLE_GCJ 0
# endif
#endif

/*
 * GCJ name mangling can confuse older tools.  It would be nice to be
 * able to dectect whether gcj-aware versions of gdb and gprof are
 * present, but we are already doing the next-best thing:  detecting
 * whether GCJ itself is present.
 *
 * Our simplified mangler differs from gcj in two respects:  First, it
 * it treats outer class names (left of a $), as qualifiers.  And
 * second, it does not use U to indicate a unicode name part.
 * Instead, is always escapes underscores preceding hex digits.
 */
#if MANGLE_GCJ
# define IS_SEP(c) ((c) == '/')
# define NAME_SEPARATORS "/"

  /* GCJ escapes out underscore iff this is a unicode word */
  static inline int bad_underscore(const char *p, const char *end) { return 1; }
#else
# define IS_SEP(c) ((c) == '/' || (c) == '$')
# define NAME_SEPARATORS "/$"

  /* In kaffe, everything is potential unicode-mangled, but underscores
   * are only escaped out if they precede hex digits or underscores.
   */
  static inline int bad_underscore(const char *p, const char *end)
  {
	int next = UTF8_GET(p, end);
	return   ((next >= '0' && next <= '9')
		  || (next >= 'a' && next <= 'f')
		  || next == '_');
  }
#endif

struct mangled_method *createMangledMethod(void)
{
	struct mangled_method *retval;

	xProfilingOff();
	if( (retval = (struct mangled_method *)
	     KMALLOC(sizeof(struct mangled_method))) )
	{
		retval->mm_flags = 0;
		retval->mm_method = 0;
		retval->mm_class = 0;
		retval->mm_args = 0;
		retval->mm_nargs = 0;
	}
	xProfilingOn();
	return( retval );
}

void deleteMangledMethod(struct mangled_method *mm)
{
	xProfilingOff();
	if( mm )
	{
		int lpc;

		KFREE(mm->mm_method);
		KFREE(mm->mm_class);
		for( lpc = 0; lpc < mm->mm_nargs; lpc++ )
		{
			KFREE(mm->mm_args[lpc]);
		}
		KFREE(mm->mm_args);
		KFREE(mm);
	}
	xProfilingOn();
}

int mangleMethodName(struct mangled_method *mm, const char *name)
{
	int retval = 0;
	size_t len, m_len;

	/* Constructors are mangled as an empty string */
	if( !strcmp(name, "<init>") )
	{
		name = "";
	}
	len = strlen(name);
	if( (m_len = mangleLength(name, (int)len, 0, 0)) )
	{
		/*
		 * A method name with special chars has the `U' placed at the
		 * end
		 */
		mm->mm_flags |= MMF_UNICODE_METHOD;
	}
	else
		m_len = len;
	if( (mm->mm_method = (char *)KMALLOC(m_len + 1)) )
	{
		size_t res;

		res = mangleString(mm->mm_method, name, len, m_len != len);
		assert(res <= (m_len + 1));
		retval = 1;
	}
	return( retval );
}

int mangleMethodClass(struct mangled_method *mm, void *cl, const char *name)
{
	int retval = 0;

	/* Just mangle the type directly */
	if( (mm->mm_class = mangleClassType(0, cl, name)) )
	{
		retval = 1;
	}
	return( retval );
}

int mangleMethodArgCount(struct mangled_method *mm, int count)
{
	int retval = 0;

	if( !count ||
	    (mm->mm_args = (char **)KMALLOC(sizeof(char *) * count)) )
	{
		mm->mm_nargs = count;
		retval = 1;
	}
	return( retval );
}

/*
 * Helper function that checks for duplicate parameter types.
 */
static int duplicateParameter(Method *meth, int curr_param)
{
	int lpc, retval = -1;
	size_t curr_len;

	/* Figure out the length of the curr_param type string */
	if( curr_param == METHOD_PSIG(meth)->nargs )
	{
		/*
		 * Its the last arg, an ')' and the return type follow, so we
		 * use them to find the length
		 */
		curr_len = (METHOD_PSIG(meth)->ret_and_args[0] - 1) -
			METHOD_PSIG(meth)->ret_and_args[curr_param];
	}
	else
	{
		curr_len = METHOD_PSIG(meth)->ret_and_args[curr_param] -
			METHOD_PSIG(meth)->ret_and_args[curr_param + 1];
	}
	/*
	 * Loop over the parameters searching for one that matches curr_param,
	 * we start at 1 since 0 is the return type.
	 */
	for( lpc = 1; (lpc < curr_param) && (retval != -1); lpc++ )
	{
		size_t arg_len;

		/* Figure out the length of the current parameter type */
		if( lpc == METHOD_PSIG(meth)->nargs )
		{
			arg_len = (METHOD_PSIG(meth)->ret_and_args[0] - 1) -
				METHOD_PSIG(meth)->ret_and_args[lpc];
		}
		else
		{
			arg_len = METHOD_PSIG(meth)->ret_and_args[lpc] -
				METHOD_PSIG(meth)->ret_and_args[lpc + 1];
		}
		if( arg_len > 1 )
		{
			if( (strncmp(&METHOD_PSIG(meth)->signature->
				     data[METHOD_PSIG(meth)->
					 ret_and_args[curr_param]],
				     &METHOD_PSIG(meth)->signature->
				     data[METHOD_PSIG(meth)->ret_and_args[lpc]],
				     arg_len) == 0) &&
			    (curr_len == arg_len) )
			{
				retval = lpc;
			}
		}
	}
	return( retval );
}

int mangleMethodArgs(struct mangled_method *mm, Method *meth)
{
	int retval = 1, lpc, ref;

	for( lpc = 1; lpc <= mm->mm_nargs; lpc++ )
	{
		if( (ref = duplicateParameter(meth, lpc)) >= 0 )
		{
			/*
			 * Duplicate parameter, use `T' to back ref the
			 * previous one
			 */
			if( (mm->mm_args[lpc - 1] = (char *)
				     KMALLOC(5)) )
			{
				sprintf(mm->mm_args[lpc - 1],
					"T%d%s",
					ref,
					(ref > 9) ? "_" : "");
			}
		}
		else
		{
			/* Unique parameter, mangle the type */
			mm->mm_args[lpc - 1] = mangleType(
				0,
				(char *)&METHOD_PSIG(meth)->signature->
				data[METHOD_PSIG(meth)->ret_and_args[lpc]]);
		}
	}
	return( retval );
}

int mangleMethod(struct mangled_method *mm, Method *meth)
{
	int retval = 0;

	xProfilingOff();
	/* Try to mangle everything provided by `meth' */
	if( mangleMethodName(mm, (char *)meth->name->data) &&
	    mangleMethodClass(mm,
			      meth->class->loader,
			      (char *)CLASS_CNAME(meth->class)) &&
	    mangleMethodArgCount(mm, METHOD_PSIG(meth)->nargs) &&
	    mangleMethodArgs(mm, meth) )
	{
		retval = 1;
	}
	xProfilingOn();
	return( retval );
}

int printMangledMethod(struct mangled_method *mm, FILE *file)
{
	int retval = 0;

	/* Atleast check for method and class */
	if( mm &&
	    mm->mm_method &&
	    mm->mm_class )
	{
		int lpc;

		retval = 1;
		fprintf(file, "%s__%s", mm->mm_method, mm->mm_class);
		for( lpc = 0; (lpc < mm->mm_nargs) && retval; lpc++ )
		{
			if( mm->mm_args[lpc] )
				fprintf(file, "%s", mm->mm_args[lpc]);
			else
				retval = 0;
		}
		/*
		 * If the method name has escapes we need to append the `U' to
		 * the end
		 */
#if MANGLE_GCJ
		if( mm->mm_flags & MMF_UNICODE_METHOD )
			fprintf(file, "U");
#endif
		if( ferror(file) )
			retval = 0;
	}
	return( retval );
}

/* Map of primitive Java types to the mangled types */
static const char *primitive_type_map[] = {
	"Z", "b",	/* boolean */
	"C", "w",	/* wide char */
	"V", "v",	/* void */
	"B", "c",	/* byte */
	"S", "s",	/* short */
	"I", "i",	/* integer */
	"J", "x",	/* long */
	"F", "f",	/* float */
	"D", "d",	/* double */
	0
};

const char *manglePrimitiveType(char type)
{
	const char *retval = NULL;
	int lpc;

	for( lpc = 0; primitive_type_map[lpc] && !retval; lpc += 2 )
	{
		if( type == primitive_type_map[lpc][0] )
			retval = primitive_type_map[lpc + 1];
	}
	return( retval );
}

char *mangleClassType(int prepend, void *cl, const char *name)
{
	int quals = 0, num_chars = 0, num_underscores = 0, need_escapes = 0;
	int ch, error = 0;
	size_t len, m_len = 0, total_len = 0;
	char *retval = 0;
	const char *curr, *end;

	/* First we find the length of mangled string */
	len = strlen(name);
	curr = name;
	end = name + len;
	while( (curr < end) && !error )
	{
		ch = UTF8_GET(curr, end);
		if( ch < 0 )
		{
			error = 1;
		}
		else if( ch == ';' )
		{
			/*
			 * The given name was of the form Ljava/lang/Object;,
			 * so the `;' marks the end instead of the given null
			 */
			end = curr - 1;
			break;
		}
		else if( IS_SEP(ch) )
		{
			/*
			 * Its a qualified name, record the current counts for
			 * this name segment and increment the number of
			 * qualifiers
			 */
			quals++;
			m_len += 4 + (need_escapes ? 7 : 0) + num_chars +
				4 * (need_escapes + num_underscores);
			num_chars = 0;
			need_escapes = 0;
			num_underscores = 0;
		}
		else if( (ch >= '0') && (ch <= '9') )
		{
			/* If a number starts a name then we need an escape */
			if( num_chars == 0 )
				need_escapes++;
		}
		else if( ch == '_' && bad_underscore(curr, end))
		{
#if MANGLE_GCJ
			num_underscores++;
#else
			need_escapes++;
#endif
		}
		else if( ((ch < 'a') || (ch > 'z')) &&
			 ((ch < 'A') || (ch > 'Z')) &&
			 (ch != '_') )
		{
			/* Its a special char, we'll need an escape */
			need_escapes++;
		}
		num_chars++;
	}
	/* Figure out the total length of the mangled name */
	total_len = m_len + 4 + (need_escapes ? 7 : 0) +
		(quals ? 7 : 0) + num_chars +
		4 * (need_escapes + num_underscores);
	/*
	 * If the class uses a non-root classLoader we need to encode that in
	 * the name, otherwise we can make duplicate names for the same class
	 * that are loaded by different class loaders.
	 */
	if( cl )
	{
		total_len += (quals ? 0 : 7) +
			2 + /* character count of `cl' + the number */
			2 + /* 'cl' */
			2 + /* '0x' */
			(sizeof(void *) * 2); /* the number */
		quals++;
	}
	if( !error && (retval = (char *)KMALLOC(prepend + total_len + 1)) )
	{
		char *dest;

		/* Start after the prepended section */
		dest = retval + prepend;
		dest[0] = 0;
		if( quals )
		{
			/*
			 * Its a qualified name, print out how many qualifiers
			 * there are before continuing
			 */
			quals++;
			if( quals < 10 )
				sprintf(dest, "Q%d", quals);
			else
				sprintf(dest, "Q_%d_", quals);
			quals--;
		}
		dest += strlen(dest);
		/* Encode the class loader, if there is one */
		if( cl )
		{
			int cl_len;

			sprintf(dest + 3, "l%p", cl);
			cl_len = strlen(dest + 3) + 1;
			sprintf(dest, "%d", cl_len);
			dest[2] = 'c'; /* The previous sprintf overwrote it */
			dest += cl_len + 2;
			quals--;
		}
		curr = name;
		while( curr < end )
		{
			if( (m_len = mangleLength(curr,
						  quals ? -1 : end - curr,
						  NAME_SEPARATORS,
						  &len)) )
			{
#if MANGLE_GCJ
				*dest = 'U';
				dest++;
#endif
			}
			else
			{
				m_len = len;
			}
			/* Write the length of the name */
			sprintf(dest, "%d", m_len);
			dest += strlen(dest);
			/* Mangle the string */
			mangleString(dest, curr, len, m_len != len);
			/* Move on to the next name */
			dest += strlen(dest);
			curr += len + 1;
			quals--;
		}
		assert((dest - retval) <= (prepend + total_len + 1));
	}
	return( retval );
}

char *mangleType(size_t prepend, const char *type)
{
	char *retval = 0;

	switch(type[0])
	{
	case 'L':
		/* Object reference */
		if( (retval = mangleClassType((int)prepend + 1, 0, type + 1)) )
			retval[prepend] = 'P';
		break;
	case '[':
		/* Array type */
		if( (retval = mangleType(prepend + 11, type + 1)) )
			strncpy(&retval[prepend], "Pt6JArray1Z", 11);
		break;
	default:
		/* Most likely a primitive */
		{
			const char *prim;

			if( (prim = manglePrimitiveType(type[0])) )
			{
				if( (retval = (char *)KMALLOC(prepend + 2)) )
				{
					retval[prepend] = prim[0];
					retval[prepend + 1] = 0;
				}
			}
		}
		break;
	}
	return( retval );
}

size_t mangleLength(const char *string, int len, const char *term, size_t *out_len)
{
	int num_chars = 0, need_escapes = 0, num_underscores = 0;
	int retval = -1, error = 0;
	const char *curr, *end;

	curr = string;
	if( len < 0 )
		end = (char *)-1; /* ick */
	else
		end = string + len;
	while( !error && ((len < 0) || (curr < end)) )
	{
		int ch = UTF8_GET(curr, end);

		if( ch < 0 )
		{
			error = 1;
			break;
		}
		else if( term )
		{
			int found_term = 0;
			int lpc;

			for( lpc = 0; term[lpc]; lpc++ )
			{
				if( term[lpc] == ch )
					found_term = 1;
			}
			if( found_term )
			{
				/* Found the specified terminator */
				break;
			}
		}
		if( (ch >= '0') && (ch <= '9') )
		{
			/* If a number starts a name then we need an escape */
			if( (curr - 1) == string)
				need_escapes++;
		}
		else if( ch == '_' && bad_underscore(curr, end))
		{
#if MANGLE_GCJ
			num_underscores++;
#else
			need_escapes++;
#endif
		}
		else if( ((ch < 'a') || (ch > 'z')) &&
			 ((ch < 'A') || (ch > 'Z')) &&
			 (ch != '_') )
		{
			/* Special character, we'll need an escape */
			need_escapes++;
		}
		num_chars++;
	}
	if( !error )
	{
		if( need_escapes )
		{
			retval = num_chars +
				4 * (need_escapes + num_underscores);
		}
		else
			retval = 0;
		/* Write back the length */
		if( out_len )
			*out_len = num_chars;
	}
	return( retval );
}

size_t mangleString(char *dest, const char *src, size_t slen, int _unicode)
{
	int retval = 0, ch, error = 0, need_escape = 0;
	char *start;
	const char *curr, *end;

	start = dest;
	curr = src;
	end = src + slen;
	while( (curr < end) && !error )
	{
		ch = UTF8_GET(curr, end);

		if( ch < 0 )
		{
			error = 1;
		}
		else if( (ch >= '0') && (ch <= '9') )
		{
			if( (curr - 1) == src )
				need_escape = 1;
			else
				need_escape = 0;
		}
		else if( ch == '_' )
		{
			if( _unicode && bad_underscore(curr, end) )
			{
				need_escape = 1;
			}
		}
		else if( ((ch < 'a') || (ch > 'z')) &&
			 ((ch < 'A') || (ch > 'Z')) )
		{
			need_escape = 1;
		}
		else
		{
			need_escape = 0;
		}
		if( !error )
		{
			if( need_escape )
			{
				sprintf(dest, "_%04x", ch);
				dest += 5;
			}
			else
			{
				*dest = ch;
				dest++;
			}
		}
	}
	*dest = 0;
	if( error )
		retval = -1;
	else
		retval = dest - start + 1;
	return( retval );
}

static int fputss(const char *str, size_t len, FILE *stream)
{
    size_t lpc;
    int retval = 0;

    for( lpc = 0; lpc < len; lpc++ )
    {
	putc(str[lpc], stream);
    }
    return( retval );
}

int vfmanglef(FILE *file, const char *format, va_list args)
{
    unsigned int sindex, eindex;
    int retval = 0;

    for( sindex = eindex = 0; format[eindex] != '\0'; eindex++ )
    {
	switch( format[eindex] )
	{
	case '%':
	    {
		int done = 0;

		while( !done )
		{
		    eindex += 1;
		    switch( format[eindex] )
		    {
		    case 'q': /* qualified string */
			{
			    const char *quals, *str;
			    size_t offset = 0, len;
			    void *ptr;
			    
			    quals = va_arg(args, const char *);
			    str = va_arg(args, const char *);
			    ptr = va_arg(args, void *);
			    
			    len = strlen(str);

			    while( offset < len )
			    {
				char buffer[1 + 2 + sizeof(void *) * 2 + 1];
				const char *sub_end;
				size_t slen;

				buffer[0] = '\0';
				sub_end = strpbrk(&str[offset], quals);
				if( sub_end == NULL )
				{
				    sub_end = &str[len];
				    if( ptr != NULL )
				    {
					snprintf(buffer,
						 sizeof(buffer),
						 "_%p",
						 ptr);
				    }
				}
				slen = sub_end - &str[offset];
				fprintf(file, "%d", slen + strlen(buffer));
				fputss(&str[offset], slen, file);
				fprintf(file, "%s", buffer);
				offset += slen + 1;
			    }
			    done = 1;
			}
			break;
		    case 't':
			{
			    const char *in, *out, *str;
			    unsigned int lpc;

			    in = va_arg(args, const char *);
			    out = va_arg(args, const char *);
			    str = va_arg(args, const char *);
			    
			    assert(strlen(in) == strlen(out));
			    for( lpc = 0; str[lpc] != '\0'; lpc++ )
			    {
				const char *rep;

				if( (rep = strchr(in, str[lpc])) != NULL )
				{
				    putc(out[rep - in], file);
				}
				else
				{
				    putc(str[lpc], file);
				}
			    }
			    done = 1;
			}
			break;
		    case 'c':
			{
				char c;

				c = (char)va_arg(args, int);
				fprintf(file, "%c", c);
				done = 1;
			}
			break;
		    case 'd':
			{
			    int i;

			    i = va_arg(args, int);
			    fprintf(file, "%d", i);
			    done = 1;
			}
			break;
		    case 'p':
			{
			    void *ptr;

			    ptr = va_arg(args, void *);
			    fprintf(file, "%p", ptr);
			    done = 1;
			}
			break;
		    case 's':
			{
			    const char *str;
			    
			    str = va_arg(args, const char *);

			    fprintf(file, "%s", str);
			    done = 1;
			}
			break;
		    case 'S':
			{
			    const char *str;
			    size_t len;
			    
			    str = va_arg(args, const char *);
			    len = va_arg(args, size_t);

			    fputss(str, len, file);
			    done = 1;
			}
			break;
		    default:
			assert(0);
			break;
		    }
		}
	    }
	    break;
	default:
	    putc(format[eindex], file);
	    break;
	}
    }
    return( retval );
}

int fmanglef(FILE *file, const char *format, ...)
{
    va_list args;
    int retval;

    va_start(args, format);
    retval = vfmanglef(file, format, args);
    va_end(args);
    return( retval );
}

#endif /* defined(KAFFE_XDEBUGGING) || defined(KAFFE_XPROFILER) */
