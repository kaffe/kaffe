/*
 * mangle.c
 * Routines for doing name mangling on Java types
 *
 * Copyright (c) 2000 University of Utah and the Flux Group.
 * All rights reserved.
 *
 * This file is licensed under the terms of the GNU Public License.
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * Contributed by the Flux Research Group, Department of Computer Science,
 * University of Utah, http://www.cs.utah.edu/flux/
 */

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "jmalloc.h"
#include "stringSupport.h"
#include "classMethod.h"
#include "xprofiler.h"

#include "mangle.h"

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

int mangleMethodName(struct mangled_method *mm, char *name)
{
	int retval = 0, len, m_len;

	/* Constructors are mangled as an empty string */
	if( !strcmp(name, "<init>") )
	{
		name = "";
	}
	len = strlen(name);
	if( (m_len = mangleLength(name, len, 0, 0)) )
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
		int res;

		res = mangleString(mm->mm_method, name, len, m_len != len);
		assert(res <= (m_len + 1));
		retval = 1;
	}
	return( retval );
}

int mangleMethodClass(struct mangled_method *mm, void *cl, char *name)
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
	int curr_len, lpc, retval = -1;

	/* Figure out the length of the curr_param type string */
	if( curr_param == meth->parsed_sig->nargs )
	{
		/*
		 * Its the last arg, an ')' and the return type follow, so we
		 * use them to find the length
		 */
		curr_len = (meth->parsed_sig->ret_and_args[0] - 1) -
			meth->parsed_sig->ret_and_args[curr_param];
	}
	else
	{
		curr_len = meth->parsed_sig->ret_and_args[curr_param] -
			meth->parsed_sig->ret_and_args[curr_param + 1];
	}
	/*
	 * Loop over the parameters searching for one that matches curr_param,
	 * we start at 1 since 0 is the return type.
	 */
	for( lpc = 1; (lpc < curr_param) && (retval != -1); lpc++ )
	{
		int arg_len;

		/* Figure out the length of the current parameter type */
		if( lpc == meth->parsed_sig->nargs )
		{
			arg_len = (meth->parsed_sig->ret_and_args[0] - 1) -
				meth->parsed_sig->ret_and_args[lpc];
		}
		else
		{
			arg_len = meth->parsed_sig->ret_and_args[lpc] -
				meth->parsed_sig->ret_and_args[lpc + 1];
		}
		if( arg_len > 1 )
		{
			if( (strncmp(&meth->parsed_sig->signature->
				     data[meth->parsed_sig->
					 ret_and_args[curr_param]],
				     &meth->parsed_sig->signature->
				     data[meth->parsed_sig->ret_and_args[lpc]],
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
				(char *)&meth->parsed_sig->signature->
				data[meth->parsed_sig->ret_and_args[lpc]]);
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
	    mangleMethodArgCount(mm, meth->parsed_sig->nargs) &&
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
		if( mm->mm_flags & MMF_UNICODE_METHOD )
			fprintf(file, "U");
		if( ferror(file) )
			retval = 0;
	}
	return( retval );
}

/* Map of primitive Java types to the mangled types */
static char *primitive_type_map[] = {
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

char *manglePrimitiveType(char type)
{
	char *retval = 0;
	int lpc;
	
	for( lpc = 0; primitive_type_map[lpc] && !retval; lpc += 2 )
	{
		if( type == primitive_type_map[lpc][0] )
			retval = primitive_type_map[lpc + 1];
	}
	return( retval );
}

char *mangleClassType(int prepend, void *cl, char *name)
{
	int quals = 0, num_chars = 0, num_underscores = 0, need_escapes = 0;
	int ch, len, m_len = 0, error = 0, total_len = 0;
	char *retval = 0, *curr, *end;

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
		else if( ch == '/' )
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
		else if( ch == '_' )
		{
			num_underscores++;
		}
		else if( ((ch < 'a') || (ch > 'z')) &&
			 ((ch < 'A') || (ch > 'Z')) )
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
			/* Figure out the length of this name segment */
			if( (m_len = mangleLength(curr,
						  quals ? -1 : end - curr,
						  '/',
						  &len)) )
			{
				*dest = 'U';
				dest++;
			}
			else
				m_len = len;
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

char *mangleType(int prepend, char *type)
{
	char *retval = 0;
	
	switch(type[0])
	{
	case 'L':
		/* Object reference */
		if( (retval = mangleClassType(prepend + 1, 0, type + 1)) )
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
			char *prim;
			
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

int mangleLength(char *string, int len, char term, int *out_len)
{
	int num_chars = 0, need_escapes = 0, num_underscores = 0;
	int retval = -1, error = 0;
	char *curr, *end;

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
		}
		else if( ch == term )
		{
			/* Found the specified terminator */
			break;
		}
		else if( (ch >= '0') && (ch <= '9') )
		{
			/* If a number starts a name then we need an escape */
			if( num_chars == 0 )
				need_escapes++;
		}
		else if( ch == '_' )
		{
			num_underscores++;
		}
		else if( ((ch < 'a') || (ch > 'z')) &&
			 ((ch < 'A') || (ch > 'Z')) )
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

int mangleString(char *dest, char *src, int slen, int unicode)
{
	int retval = 0, ch, error = 0, need_escape = 0;
	char *start, *curr, *end;

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
			if( curr == (src - 1) )
				need_escape = 1;
			else
				need_escape = 0;
		}
		else if( ch == '_' )
		{
			if( unicode )
				need_escape = 1;
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
