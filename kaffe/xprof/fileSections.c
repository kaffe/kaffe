/*
 * fileSections.c
 * Definitions for feedback file sections
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

#include "config.h"

#if defined(KAFFE_FEEDBACK)

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "kaffe/jmalloc.h"

#include "sectionFile.h"
#include "fileSections.h"

typedef va_list *va_list_ptr;


/* Set a value in the jit section structure by name */
static int setJITSectionValue(struct jit_section_data *jsd,
			      char *tag, char *value)
{
	int retval = 1;

	if( !strcmp(tag, "precompile") )
	{
		jsd->jsd_flags = parseFlagString(value,
						 jsd->jsd_flags,
						 JSDF_PRECOMPILE);
	}
	else if( !strcmp(tag, "address") )
	{
		void *addr;

		if( sscanf(value, "%p", &addr) != 1 )
			jsd->jsd_address = addr;
		else
			retval = 0;
	}
	else if( !strcmp(tag, "size") )
	{
		if( sscanf(value, "%lu", &jsd->jsd_size) != 1 )
			retval = 0;
	}
	return( retval );
}

/* Write out the current value of a jit attribute */
static int writeJITSectionValue(struct jit_section_data *jsd,
				struct parse_state *ps,
				char *tag, char *value,
				FILE *out_file)
{
	char scratch[128];
	int retval = 1;

	/*
	 * Figure out which attribute we're processing and construct the new
	 * value
	 */
	if( !tag )
	{
		tag = "";
		value = "";
	}
	else if( !strcmp(tag, "precompile") )
	{
		value = makeFlagString(jsd->jsd_flags, JSDF_PRECOMPILE, value);
	}
	else if( !strcmp(tag, "size") )
	{
		sprintf(scratch, "%lu", jsd->jsd_size);
		value = scratch;
	}
	else if( !strcmp(tag, "address") )
	{
		sprintf(scratch, "%p", jsd->jsd_address);
		value = scratch;
	}
	/*
	 * parseSectionLine will do the actual writing so its formatted
	 * correctly
	 */
	retval = parseSectionLine(ps, &tag, &value, out_file);
	return( retval );
}

static int jitSectionHandler(struct file_section *fs, struct section_file *sf,
			     int method, ...)
{
	int retval = 1;
	va_list args;

	va_start(args, method);
	switch( method )
	{
	case SFM_CREATE:
		{
			struct section_file_data **out_sfd;
			struct jit_section_data *jsd;
			char *name, *tag, *value;
			va_list_ptr values;

			/* Get the args */
			out_sfd = va_arg(args, struct section_file_data **);
			name = va_arg(args, char *);
			values = va_arg(args, va_list_ptr);
			/* Allocate the section struct and initialize it */
			if( (jsd = (struct jit_section_data *)
			     KMALLOC(sizeof(struct jit_section_data))) )
			{
				assert(name[0]);
				jsd->jsd_link.sfd_type = fs;
				jsd->jsd_link.sfd_flags = 0;
				jsd->jsd_link.sfd_name = name;
				jsd->jsd_flags = 0;
				jsd->jsd_size = 0;
				jsd->jsd_address = 0;
				/* Process the rest of the args */
				tag = va_arg((*values), char *);
				while( tag )
				{
					value = va_arg((*values), char *);
					setJITSectionValue(jsd, tag, value);
					tag = va_arg((*values), char *);
				}
				*out_sfd = &jsd->jsd_link;
			}
			else
			{
				retval = 0;
			}
		}
		break;
	case SFM_DELETE:
		{
			struct section_file_data *sfd;

			sfd = va_arg(args, struct section_file_data *);
			KFREE(sfd);
		}
		break;
	case SFM_CACHE:
		{
			char *line, *tag = 0, *value = 0;
			struct jit_section_data *jsd;
			struct parse_state *ps;
			int line_len;

			jsd = va_arg(args, struct jit_section_data *);
			ps = va_arg(args, struct parse_state *);
			line = va_arg(args, char *);
			line_len = va_arg(args, int);
			/* Read the line in and set the value */
			parseSectionLine(ps, &tag, &value, 0);
			if( tag )
				setJITSectionValue(jsd, tag, value);
		}
		break;
	case SFM_FLUSH:
		{
			struct jit_section_data *jsd;
			char *line, *tag, *value;
			struct parse_state *ps;
			FILE *out_file;
			int line_len;

			jsd = va_arg(args, struct jit_section_data *);
			ps = va_arg(args, struct parse_state *);
			line = va_arg(args, char *);
			line_len = va_arg(args, int);
			out_file = va_arg(args, FILE *);
			if( line )
			{
				/*
				 * Parse the line and write out the current
				 * value
				 */
				retval = parseSectionLine(ps, &tag, &value, 0);
				retval = writeJITSectionValue(jsd, ps,
							      tag, value,
							      out_file);
			}
			else if( jsd->jsd_link.sfd_flags & SFDF_DIRTY )
			{
				/*
				 * The section wasn't in the file before, write
				 * the whole thing out
				 */
				fprintf(out_file,
					"\tprecompile %s\n",
					jsd->jsd_flags & JSDF_PRECOMPILE ?
					"true" : "false");
				fprintf(out_file,
					"\tsize %lu\n",
					jsd->jsd_size);
				fprintf(out_file,
					"\taddress %p\n",
					jsd->jsd_address);
			}
			else
			{
			}
		}
		break;
	default:
		break;
	}
	return( retval );
}

struct file_section jit_section;

/* Set a value in the lib section structure by name */
static int setLibSectionValue(struct lib_section_data *lsd,
			      char *tag, char *value)
{
	int retval = 1;

	if( !strcmp(tag, "preload") )
	{
		lsd->lsd_flags = parseFlagString(value,
						 lsd->lsd_flags,
						 LSDF_PRELOAD);
	}
	return( retval );
}

/* Write out the current value of a jit attribute */
static int writeLibSectionValue(struct lib_section_data *lsd,
				struct parse_state *ps,
				char *tag, char *value,
				FILE *out_file)
{
	int retval = 1;

	/*
	 * Figure out which attribute we're processing and construct the new
	 * value
	 */
	if( !tag )
	{
		tag = "";
		value = "";
	}
	else if( !strcmp(tag, "preload") )
	{
		value = makeFlagString(lsd->lsd_flags, LSDF_PRELOAD, value);
	}
	/*
	 * parseSectionLine will do the actual writing so its formatted
	 * correctly
	 */
	retval = parseSectionLine(ps, &tag, &value, out_file);
	return( retval );
}

static int libSectionHandler(struct file_section *fs, struct section_file *sf,
			     int method, ...)
{
	int retval = 1;
	va_list args;

	va_start(args, method);
	switch( method )
	{
	case SFM_CREATE:
		{
			struct section_file_data **out_sfd;
			struct lib_section_data *lsd;
			char *name, *tag, *value;
			va_list_ptr values;

			/* Get the args */
			out_sfd = va_arg(args, struct section_file_data **);
			name = va_arg(args, char *);
			values = va_arg(args, va_list_ptr);
			/* Allocate the section struct and initialize it */
			if( (lsd = (struct lib_section_data *)
			     KMALLOC(sizeof(struct lib_section_data))) )
			{
				assert(name[0]);
				lsd->lsd_link.sfd_type = fs;
				lsd->lsd_link.sfd_flags = 0;
				lsd->lsd_link.sfd_name = name;
				lsd->lsd_flags = 0;
				/* Process the rest of the args */
				tag = va_arg((*values), char *);
				while( tag )
				{
					value = va_arg((*values), char *);
					setLibSectionValue(lsd, tag, value);
					tag = va_arg((*values), char *);
				}
				*out_sfd = &lsd->lsd_link;
			}
			else
			{
				retval = 0;
			}
		}
		break;
	case SFM_DELETE:
		{
			struct section_file_data *sfd;

			sfd = va_arg(args, struct section_file_data *);
			KFREE(sfd);
		}
		break;
	case SFM_CACHE:
		{
			char *line, *tag = 0, *value = 0;
			struct lib_section_data *lsd;
			struct parse_state *ps;
			int line_len;

			lsd = va_arg(args, struct lib_section_data *);
			ps = va_arg(args, struct parse_state *);
			line = va_arg(args, char *);
			line_len = va_arg(args, int);
			/* Read the line in and set the value */
			parseSectionLine(ps, &tag, &value, 0);
			if( tag )
				setLibSectionValue(lsd, tag, value);
		}
		break;
	case SFM_FLUSH:
		{
			struct lib_section_data *lsd;
			char *line, *tag, *value;
			struct parse_state *ps;
			FILE *out_file;
			int line_len;

			lsd = va_arg(args, struct lib_section_data *);
			ps = va_arg(args, struct parse_state *);
			line = va_arg(args, char *);
			line_len = va_arg(args, int);
			out_file = va_arg(args, FILE *);
			if( line )
			{
				/*
				 * Parse the line and write out the current
				 * value
				 */
				retval = parseSectionLine(ps, &tag, &value, 0);
				retval = writeLibSectionValue(lsd, ps,
							      tag, value,
							      out_file);
			}
			else if( lsd->lsd_link.sfd_flags & SFDF_DIRTY )
			{
				/*
				 * The section wasn't in the file before, write
				 * the whole thing out
				 */
				fprintf(out_file,
					"\tpreload %s\n",
					lsd->lsd_flags & LSDF_PRELOAD ?
					"true" : "false");
			}
			else
			{
			}
		}
		break;
	default:
		break;
	}
	return( retval );
}

struct file_section lib_section;

void installFileSections(void)
{
	/* Setup and install the handlers */
	jit_section.fs_name = "jit-code";
	jit_section.fs_handler = jitSectionHandler;
	addSectionType(&jit_section);
	lib_section.fs_name = "library";
	lib_section.fs_handler = libSectionHandler;
	addSectionType(&lib_section);
}

#endif /* KAFFE_FEEDBACK */
