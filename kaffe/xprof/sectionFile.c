/*
 * sectionFile.c
 * Routines for reading/writing a formatted text file
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
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "kaffe/jmalloc.h"
#include "stringSupport.h"
#include "debug.h"

#include "sectionFile.h"

/* Number of buckets for the file_section hash table */
#define FILE_SECTION_TYPES_SIZE 31

/* Hash table for file_section's */
static struct file_section *file_section_types[FILE_SECTION_TYPES_SIZE];

static void restoreLine(struct parse_state *ps);
static int syncFile(struct parse_state *parent, struct section_file *sf,
		    char *filename);

/*
 * Internal state for the file parser
 */
struct parse_state {
	struct parse_state *ps_parent;		/* Keep track of nesting */
	char *ps_filename;			/* Current file name */
	struct section_file *ps_section_file;	/* Root section file */
	FILE *ps_in_file;			/* Input File handle */
	FILE *ps_out_file;			/* Output File handle */
	struct section_file_data *ps_section;	/* Current section */
	int ps_line;				/* Current line number */
	int ps_column;				/* Current column number */
	char *ps_data;				/* Current line string */
	int ps_len;				/* Length of line */
	char ps_save[2];			/* Storage for damage */
};

/*
 * Produce a hash value within table_size, for string `name'
 */
static int hashName(const char *name, int table_size)
{
	int h;

	for( h = 0; *name; name++ )
		h = (64 * h + tolower(*name)) % table_size;
	return( h );
}

/*
 * Check for valid characters in a string from the file
 */
static int validStringChar(int c)
{
	int retval = 0;

	if( isprint(c) &&
	    (c != '#') &&
	    (c != '%') &&
	    (c != '\\') )
		retval = 1;
	return( retval );
}

/*
 * Write out a utf string with the proper escapes, if needed.
 */
static int writeUtfString(struct parse_state *ps, char *ustr)
{
	int ch, len, error = 0;
	char *curr, *end;

	len = strlen(ustr);
	end = ustr + len;
	curr = ustr;
	while( (curr < end) && !error )
	{
		ch = UTF8_GET(curr, end);
		if( ch < 0 )
		{
			error = 1;
		}
		else if( validStringChar(ch) )
		{
			/* Plain character, just write it out */
			fprintf(ps->ps_out_file, "%c", (char)ch);
		}
		else
		{
			/* Write out a UTF8 character with escape sequence */
			fprintf(ps->ps_out_file, "\\u%04x", ch);
		}
	}
	return( !error );
}

enum {
	SFS_INVALID,
	SFS_PLAIN,
	SFS_UTF8,
};

/*
 * Figure out what kind of string has come from a file.  If it has UTF8
 * escapes we'll need to translate those.
 */
static int stringType(char *str)
{
	int retval = SFS_PLAIN, utf8char = 0;

	while( *str && (retval != SFS_INVALID) )
	{
		if( *str == '\\' )
		{
			/* Do a thorough check of the escape */
			if( (*(str + 1) == 'u') &&
			    (sscanf(str + 2, "%x", &utf8char) == 1) )
				retval = SFS_UTF8;
			else
				retval = SFS_INVALID;
		}
		str++;
	}
	return( retval );
}

/*
 * Convert a UTF8 escaped string from the file into an internal format.
 */
static char *convertUtfString(struct parse_state *ps, char *pstr)
{
	char *retval = 0;
	int len;

	len = strlen(pstr);
	/* We're allocating more than is really needed... */
	if( (retval = (char *)KMALLOC(len + 1)) )
	{
		int lpc, utf8char;
		char *curr;

		curr = retval;
		for( lpc = 0; lpc < len; lpc++ )
		{
			switch( pstr[lpc] )
			{
			case '\\':
				utf8char = 0;
				lpc += 2; /* Skip the 'u' */
				if( sscanf(&pstr[lpc],
					   "%4x",
					   &utf8char) == 1 )
				{
					char plain_char;

					/*
					 * XXX This isn't a proper check for a
					 * UTF char.  We just check if its can
					 * be held within 8 bits and write it
					 * directly to the string, otherwise we
					 * store it as 16 bits.
					 */
					plain_char = utf8char;
					if( plain_char == utf8char )
					{
						*curr = plain_char;
					}
					else
					{
						*curr = utf8char >> 8;
						curr++;
						*curr = utf8char & 0xff;
					}
					curr++;
				}
				lpc += 3; /* Skip the value */
				break;
			default:
				/* Plain character, just copy it over */
				*curr = pstr[lpc];
				curr++;
				break;
			}
		}
		*curr = 0;
	}
	return( retval );
}

/*
 * Do the actual removal of a section from a file, this is called after
 * write sync when a section was scheduled for removal.
 */
static void reallyRemoveSectionFromFile(struct section_file *sf,
					struct section_file_data *sfd)
{
	struct section_file_data **prev, *curr;
	int hash;

	/* Remove it from the hash table */
	hash = hashName(sfd->sfd_name, SECTION_FILE_HASH_SIZE);
	curr = sf->sf_sections[hash];
	prev = &sf->sf_sections[hash];
	while( curr && (curr != sfd) )
	{
		prev = &curr->sfd_next;
		curr = curr->sfd_next;
	}
	if( curr )
		*prev = sfd->sfd_next;
	/* Remove it from the ordered list */
	curr = sf->sf_ordered_sections;
	prev = &sf->sf_ordered_sections;
	while( curr && (curr != sfd) )
	{
		prev = &curr->sfd_order;
		curr = curr->sfd_order;
	}
	if( curr )
		*prev = sfd->sfd_order;
}

/* Parse a directive line */
static int parseDirective(struct parse_state *ps, int curr)
{
	int retval = 1, lpc, directive_len, args_len, line_len;
	char *directive, *args, *save, *line;
	struct section_file *sf;
	FILE *out_file;

	sf = ps->ps_section_file;
	out_file = ps->ps_out_file;
	line_len = ps->ps_len;
	line = ps->ps_data;
	save = ps->ps_save;

	/* The line passed in points to the beginning of the directive */
	directive = &line[curr];
	/* Move to the end of the directive */
	for( lpc = curr; (lpc < line_len) && !isspace(line[lpc]); lpc++ );
	directive_len = lpc - curr;
	/* The arguments to the directive directly follow it */
	args = &line[lpc + 1];
	args_len = line_len - (lpc + 1);
	if( !strncmp(directive, "%include", directive_len) )
	{
		int name_start, name_end;

		/* Try and get a quoted filename from the args */
		for( name_start = 0;
		     (name_start < args_len) && (args[name_start] != '\"');
		     name_start++ );
		for( name_end = name_start + 1;
		     (name_end < args_len) && (args[name_end] != '\"');
		     name_end++ );
		if( (name_start < args_len) && (name_end < args_len) )
		{
			args[name_end] = 0;
			/* Try and sync the include file */
			if( !syncFile(ps, ps->ps_section_file,
				      args + name_start + 1) )
			{
				dprintf(
					"Error:%s:%d:%d - Unable to process "
					"file %s.\n",
					ps->ps_filename,
					ps->ps_line,
					ps->ps_column,
					args + name_start + 1);
			}
			args[name_end] = '\"';
		}
		else
		{
			dprintf(
				"Error:%s:%d:%d - %%include directive needs "
				"a quoted filename.\n",
				ps->ps_filename,
				ps->ps_line,
				ps->ps_column);
		}
		if( out_file )
		{
			fwrite(line, 1, line_len, out_file);
		}
	}
	else if( !strncmp(directive, "%begin", directive_len) )
	{
		char *section_type = 0, *section_name = 0;
		struct file_section *type;
		int stype;

		/* This is the beginning of a section */

		/* Parse the args into type and name */
		ps->ps_data = args;
		ps->ps_len = args_len;
		parseSectionLine(ps, &section_type, &section_name, 0);
		ps->ps_data = line;
		ps->ps_len = line_len;
		type = findSectionType(section_type);
		switch( (stype = stringType(section_name)) )
		{
		case SFS_INVALID:
			retval = 0;
			dprintf(
				"Error:%s:%d - Invalid characters in section "
				"name\n",
				ps->ps_filename,
				ps->ps_line);
			break;
		case SFS_PLAIN:
			break;
		case SFS_UTF8:
			if( !(section_name =
			      convertUtfString(ps, section_name)) )
				retval = 0;
			break;
		}
		if( !retval )
		{
		}
		else if( (ps->ps_section = findSectionInFile(sf,
							     type,
							     section_name)) )
		{
			/* The section already exists */
			if( out_file )
			{
				/*
				 * We'll be writing out the data in a bit
				 * so clear the dirty bit.
				 */
				ps->ps_section->sfd_flags &= ~SFDF_DIRTY;
			}
		}
		else if( out_file )
		{
			/*
			 * If we're writing and there's no section then
			 * something is screwy and we should probably cache
			 * the unseen data...  or not
			 */
		}
		else
		{
			/*
			 * The section hasn't been seen before, make a new
			 * one and add it to the structure.
			 */
			if( (ps->ps_section = createFileSection(section_type,
								section_name,
								NULL)) )
			{
				addSectionToFile(sf, ps->ps_section);
			}
			else
			{
				retval = 0;
			}
		}
		if( stype == SFS_UTF8 )
			KFREE(section_name);
		/* Fix the damage parseSectionLine did */
		restoreLine(ps);
		if( out_file && ps->ps_section &&
		    !(ps->ps_section->sfd_flags & SFDF_REMOVED) )
		{
			fwrite(line, 1, line_len, out_file);
		}
	}
	else if( !strncmp(directive, "%end", directive_len) )
	{
		/* This is the end of a section */
		if( out_file && ps->ps_section )
		{
			if( ps->ps_section->sfd_flags & SFDF_REMOVED )
			{
				/* The section was pending removal */
				reallyRemoveSectionFromFile(sf,
							    ps->ps_section);
				deleteFileSection(ps->ps_section);
			}
			else
			{
				/* Ask the handler to write any new data out */
				retval = ps->ps_section->sfd_type->
					fs_handler(ps->ps_section->sfd_type,
						   sf,
						   SFM_FLUSH,
						   ps->ps_section,
						   ps,
						   0,
						   0,
						   out_file);
				fwrite(line, 1, line_len, out_file);
			}
		}
		/* We're out of the section so clear the current section */
		ps->ps_section = 0;
	}
	else
	{
		dprintf(
			"Error:%s:%d - Directive `%s' is not valid\n",
			ps->ps_filename,
			ps->ps_line,
			directive);
	}
	if( out_file && ferror(out_file) )
		retval = 0;
	return( retval );
}

/* XXX bleh */
#define MAX_LINE_SIZE 1024

/*
 * Parse the input file and send any output to `out_file' if there is one
 */
static int parseFile(struct parse_state *ps)
{
	char *line, buffer[MAX_LINE_SIZE];
	int retval = 1, line_len;
	FILE *in_file, *out_file;
	struct section_file *sf;

	sf = ps->ps_section_file;
	in_file = ps->ps_in_file;
	out_file = ps->ps_out_file;
	ps->ps_data = buffer;

	/* Walk over the input file line by line */
	while( retval && (line = fgets(buffer, MAX_LINE_SIZE, in_file)) )
	{
		int curr;

		ps->ps_line++;
		ps->ps_column = 0;
		ps->ps_len = line_len = strlen(line);
		/* Skip white space */
		for( curr = 0;
		     (curr < line_len) && isspace(line[curr]);
		     curr++ )
			ps->ps_column++;
		if( curr == line_len )
		{
			/* Empty line, dump to output */
			if( out_file )
				fwrite(line, 1, line_len, out_file);
			continue;
		}
		/* Found something, parse it */
		switch( line[curr] )
		{
		case '%':
			/* Directive's start with %, handle it */
			retval = parseDirective(ps, curr);
			curr = line_len;
			break;
		case '#':
			/* A comment, dump the rest of the line to output */
			if( out_file )
			{
				fwrite(line, 1, line_len, out_file);
			}
			curr = line_len;
			break;
		default:
			if( ps->ps_section )
			{
				if( ps->ps_section->sfd_flags & SFDF_REMOVED )
					continue;
				/*
				 * We're in a section, ask the handler to
				 * process it
				 */
				if( !line_len || (line[line_len - 1] != '\n') )
				{
					retval = 0;
				}
				else if( out_file )
				{
					/* Bring the file up to date */
					retval = ps->ps_section->sfd_type->
						fs_handler(ps->ps_section->
							   sfd_type,
							   sf,
							   SFM_FLUSH,
							   ps->ps_section,
							   ps,
							   line,
							   line_len,
							   out_file);
				}
				else
				{
					/* Read new data in */
					retval = ps->ps_section->sfd_type->
						fs_handler(ps->ps_section->
							   sfd_type,
							   sf,
							   SFM_CACHE,
							   ps->ps_section,
							   ps,
							   line,
							   line_len);
				}
			}
			else
			{
				dprintf(
					"Error:%s:%d:%d - Text outside of "
					"section\n",
					ps->ps_filename,
					ps->ps_line,
					ps->ps_column);
				if( out_file )
				{
					/*
					 * Be nice and output it, but put it in
					 * a comment
					 */
					fprintf(out_file, "# %s", line);
				}
			}
			curr = line_len;
			break;
		}
	}
	/* Check for errors */
	if( ferror(in_file) || (out_file && ferror(out_file)) )
		retval = 0;
	return( retval );
}

/*
 * Check for any new sections added to the section_file and output them to
 * the file.
 */
static int writeNewSections(struct parse_state *ps)
{
	struct section_file_data *sfd;
	struct section_file *sf;
	int retval = 1;
	FILE *out_file;

	sf = ps->ps_section_file;
	out_file = ps->ps_out_file;

	/* Walk over the hash table links */
	sfd = sf->sf_ordered_sections;
	while( sfd && retval )
	{
		if( sfd->sfd_flags & SFDF_DIRTY )
		{
			/*
			 * This section wasn't touched while sync'ing
			 * it must be new to the file.
			 */
			if( sfd->sfd_name[0] )
			{
				fprintf(out_file,
					"\n%%begin %s ",
					sfd->sfd_type->fs_name);
				retval = writeUtfString(ps, sfd->sfd_name);
				fprintf(out_file, "\n");
			}
			else
			{
				fprintf(out_file,
					"\n%%begin %s\n",
					sfd->sfd_type->fs_name);
			}
			retval = sfd->sfd_type->fs_handler(sfd->sfd_type,
							   sf,
							   SFM_FLUSH,
							   sfd,
							   ps,
							   0,
							   0,
							   out_file);
			fprintf(out_file, "%%end\n");
			sfd->sfd_flags &= ~SFDF_DIRTY;
		}
		sfd = sfd->sfd_order;
	}
	if( ferror(out_file) )
		retval = 0;
	return( retval );
}

/*
 * Write out the internal data to the file
 */
static int writeFile(struct parse_state *ps)
{
	int retval = 0, temp_fd = -1;
	FILE *in_file, *out_file;
	struct section_file *sf;
	char *temp_name = 0;

	sf = ps->ps_section_file;
	in_file = ps->ps_in_file;

	/* Create a temporary file for the output */
	if( (temp_name = (char *)KMALLOC(10)) &&
	    strcpy(temp_name, "sf.XXXXXX") &&
	    ((temp_fd = mkstemp(temp_name)) >= 0) &&
	    (out_file = fdopen(temp_fd, "w+")) )
	{
		ps->ps_out_file = out_file;
		/* Parse the old file and update with current data */
		if( (retval = parseFile(ps)) )
		{
			if( !ps->ps_parent )
			{
				/*
				 * Write out any sections that don't have peers
				 * in the input file
				 */
				retval = writeNewSections(ps);
			}
		}
		else
		{
			dprintf(
				"Error: Unable to sync file %s\n",
				ps->ps_filename);
		}
		fclose(out_file);
		/*
		 * If there weren't any errors we rename the temporary file
		 * so that it overwrites the input file.  Otherwise, we remove
		 * the bad output file, and leave the input file intact.
		 */
		if( retval )
		{
			rename(temp_name, ps->ps_filename);
		}
		else
		{
			remove(temp_name);
		}
	}
	else
	{
		dprintf(
			"Error: Unable to create temporary file for "
			"rewriting %s.\n",
			ps->ps_filename);
		if( temp_fd >= 0 )
		{
			remove(temp_name);
			close(temp_fd);
		}
	}
	KFREE(temp_name);
	return( retval );
}

/*
 * Internal file sync function.  This allows for included files to be processed
 * and their data contained in the root section file.
 */
static int syncFile(struct parse_state *parent, struct section_file *sf,
		    char *filename)
{
	struct stat file_stat;
	time_t file_time = 0;
	int retval = 0;
	FILE *file;

	/* Make sure theres a file to sync with */
	if( !filename || !filename[0] )
		return( 0 );
	/* Get the file's time stamp so we know whether its new or old */
	if( !stat(filename, &file_stat) )
	{
		file_time = file_stat.st_mtime;
	}
	else
	{
		/* Make an empty file if it doesn't exist */
		if( (file = fopen(filename, "w+")) )
		{
			fclose(file);
		}
	}
	if( (file = fopen(filename, "r")) )
	{
		struct parse_state ps;

		/* Initialize the parser state */
		ps.ps_parent = parent;
		ps.ps_filename = filename;
		ps.ps_section_file = sf;
		ps.ps_in_file = file;
		ps.ps_out_file = 0;
		ps.ps_section = 0;
		ps.ps_line = 0;
		ps.ps_column = 0;
		ps.ps_data = 0;
		ps.ps_len = 0;
		if( sf->sf_time < file_time )
		{
			/* The file is newer, read it in */
			if( (retval = parseFile(&ps)) )
			{
				if( !parent )
					sf->sf_time = time(0);
			}
		}
		else
		{
			/* The file is older, write out our data */
			if( (retval = writeFile(&ps)) )
			{
				if( !parent )
					sf->sf_time = time(0);
			}
		}
		fclose(file);
	}
	return( retval );
}

struct section_file *createSectionFile()
{
	struct section_file *retval;

	/* Allocate the structure and do any initialization */
	if( (retval = (struct section_file *)
	     KMALLOC(sizeof(struct section_file))) )
	{
		int lpc;

		retval->lock = 0;
		retval->sf_filename = 0;
		retval->sf_time = 0;
		retval->sf_ordered_sections = 0;
		retval->sf_last_section = &retval->sf_ordered_sections;
		for( lpc = 0; lpc < SECTION_FILE_HASH_SIZE; lpc++ )
		{
			retval->sf_sections[lpc] = 0;
		}
	}
	return( retval );
}

void deleteSectionFile(struct section_file *sf)
{
	if( sf )
	{
		struct section_file_data *sfd, *sfd_next;
		int lpc;

		/*
		 * Walk through the section and have their handlers delete the
		 * section data objects.
		 */
		for( lpc = 0; lpc < SECTION_FILE_HASH_SIZE; lpc++ )
		{
			sfd = sf->sf_sections[lpc];
			while( sfd )
			{
				sfd_next = sfd->sfd_next;
				sfd->sfd_type->fs_handler(sfd->sfd_type,
							  sf,
							  SFM_DELETE,
							  sfd);
				sfd = sfd_next;
			}
		}
		KFREE(sf);
	}
}

void setSectionFileName(struct section_file *sf, char *name)
{
	sf->sf_filename = name;
}

/*
 * States for parsing a section line made up of an identifier followed by
 * white space, and a data string
 */
enum {
	SS_TAG,		/* Getting the tag */
	SS_GUTTER,	/* White space between tag and value */
	SS_VALUE,	/* Value to the end of the line, minus ws, comments */
};

int parseSectionLine(struct parse_state *ps, char **tag, char **value,
		     FILE *out_file)
{
	int state, tag_start = -1, tag_end = -1;
	int value_start = -1, value_end = -1;
	int retval = 1, curr, line_len;
	char *line, *save;

	line = ps->ps_data;
	line_len = ps->ps_len;
	save = ps->ps_save;

	/* We start out looking for the tag */
	state = SS_TAG;
	/* Walk through the line */
	for( curr = 0; curr < line_len; curr++ )
	{
		switch(line[curr])
		{
		case '#':
			/* Its a comment */
			if( out_file )
			{
				/* Write out value */
				fwrite(*value, 1, strlen(*value), out_file);
				if( value_end > 0 )
				{
					/*
					 * Reset the trampled character at the
					 * end of value
					 */
					line[value_end + 1] = save[1];
					/* Write out the comment */
					fwrite(&line[value_end + 1],
					       1,
					       line_len - (value_end + 1),
					       out_file);
				}
				else
				{
					fwrite(&line[curr], 1, line_len - curr,
					       out_file);
				}
			}
			/* Nothing left to parse, jump to the end */
			curr = line_len;
			break;
		case 0: /* Trampled white space */
		case ' ':
		case '\t':
			switch(state)
			{
			case SS_TAG:
				if( tag_start != -1 )
				{
					/* Found the gutter */
					if( out_file )
					{
						/* Write the tag out */
						fwrite(*tag,
						       1,
						       strlen(*tag),
						       out_file);
						/*
						 * Write out the trampled char
						 */
						fwrite(&save[0],
						       1, 1, out_file);
					}
					state = SS_GUTTER;
				}
				else if( out_file )
				{
					/* Write out the space */
					fwrite(&line[curr], 1, 1, out_file);
				}
				break;
			case SS_GUTTER:
				if( out_file )
				{
					/* Write out the space */
					fwrite(&line[curr], 1, 1, out_file);
				}
				break;
			case SS_VALUE:
				if( out_file )
				{
					/*
					 * If the trampled char was a linefeed
					 * then we need to write the value and
					 * the linefeed.
					 */
					if( (line[curr] == 0) &&
					    (curr == line_len - 1) )
					{
						fwrite(*value, 1,
						       strlen(*value),
						       out_file);
						fwrite("\n", 1, 1, out_file);
					}
				}
				break;
			default:
				break;
			}
			break;
		case '\n':
			if( out_file )
			{
				/* Write out value and the linefeed */
				fwrite(*value, 1, strlen(*value), out_file);
				fwrite(&line[curr], 1, 1, out_file);
			}
			break;
		default:
			switch(state)
			{
			case SS_TAG:
				if( tag_start == -1 )
					tag_start = curr;
				tag_end = curr;
				break;
			case SS_GUTTER:
				state = SS_VALUE;
				/* Fallthrough */
			case SS_VALUE:
				if( value_start == -1 )
					value_start = curr;
				value_end = curr;
				break;
			}
			break;
		}
		ps->ps_column++;
	}
	if( (tag_start != -1) && (tag_end != -1) )
	{
		/* Save the char we're gonna trample */
		save[0] = line[tag_end + 1];
		line[tag_end + 1] = 0;
		*tag = &line[tag_start];
	}
	else
	{
		*tag = ""; /* better than NULL... */
	}
	if( (value_start != -1) && (value_end != -1) )
	{
		/* Save the char we're gonna trample */
		save[1] = line[value_end + 1];
		line[value_end + 1] = 0;
		*value = &line[value_start];
	}
	else
	{
		*value = "";
	}
	/* Check for I/O error */
	if( out_file && ferror(out_file) )
		retval = 0;
	return( retval );
}

/*
 * Silly function to restore the characters that parseLine tramples
 */
static void restoreLine(struct parse_state *ps)
{
	int save_idx = 0, lpc;

	for( lpc = 0; lpc < ps->ps_len; lpc++ )
	{
		/* Find any nulls and replace with saved chars */
		if( !ps->ps_data[lpc] )
		{
			ps->ps_data[lpc] = ps->ps_save[save_idx];
			save_idx++;
		}
	}
}

/* Strings that represent `true' */
static char *trueStrings[] = {
	"1",
	"true",
	"yes",
	"y",
	"t",
	0
};

/* Strings that represent `false' */
static char *falseStrings[] = {
	"0",
	"false",
	"no",
	"n",
	"f",
	0
};
/*
 * Note:  The order of the values in trueStrings and falseStrings is important,
 * and should be of consistent style.
 */

unsigned long parseFlagString(char *value,
			      unsigned long flags,
			      unsigned long flag_bit)
{
	int retval = flags;
	int lpc;

	for( lpc = 0; trueStrings[lpc]; lpc++ )
	{
		/* If `value' is true, then set the bit */
		if( !strcasecmp(value, trueStrings[lpc]) )
		{
			retval |= flag_bit;
			break;
		}
		/* If `value' is false, then clear the bit */
		if( !strcasecmp(value, falseStrings[lpc]) )
		{
			retval &= ~flag_bit;
			break;
		}
	}
	return( retval );
}

char *makeFlagString(unsigned long flags, unsigned long flag_bit, char *value)
{
	/* Default to the old value */
	char *retval = value;

	/* Check for a change from the old value */
	if( parseFlagString(value, flags, flag_bit) != flags )
	{
		int lpc;

		/* Loop through all the strings */
		for( lpc = 0; trueStrings[lpc]; lpc++ )
		{
			/*
			 * If this true matches, then return the corresponding
			 * false string
			 */
			if( !strcasecmp(value, trueStrings[lpc]) )
			{
				retval = falseStrings[lpc];
				break;
			}
			/*
			 * If this false matches, then return the corresponding
			 * true string
			 */
			if( !strcasecmp(value, falseStrings[lpc]) )
			{
				retval = trueStrings[lpc];
				break;
			}
		}
	}
	return( retval );
}

int syncSectionFile(struct section_file *sf)
{
	int retval;

	lockMutex(sf);
	retval = syncFile(0, sf, sf->sf_filename);
	unlockMutex(sf);
	return( retval );
}

void addSectionType(struct file_section *fs)
{
	int hash;

	hash = hashName(fs->fs_name, FILE_SECTION_TYPES_SIZE);
	fs->fs_next = file_section_types[hash];
	file_section_types[hash] = fs;
}

struct file_section *findSectionType(char *name)
{
	struct file_section *retval = 0, *fs;
	int hash;

	hash = hashName(name, FILE_SECTION_TYPES_SIZE);
	fs = file_section_types[hash];
	while( fs && !retval )
	{
		if( !strcmp(name, fs->fs_name) )
		{
			retval = fs;
		}
		fs = fs->fs_next;
	}
	return( retval );
}

void addSectionToFile(struct section_file *sf, struct section_file_data *sfd)
{
	int hash;

	hash = hashName(sfd->sfd_name, SECTION_FILE_HASH_SIZE);
	lockMutex(sf);
	sfd->sfd_next = sf->sf_sections[hash];
	/* The section is new to this file so we set the dirty bit */
	sfd->sfd_flags |= SFDF_DIRTY;
	sf->sf_sections[hash] = sfd;
	*sf->sf_last_section = sfd;
	sf->sf_last_section = &sfd->sfd_order;
	sfd->sfd_order = 0;
	unlockMutex(sf);
}

void remSectionFromFile(struct section_file *sf, struct section_file_data *sfd)
{
	sfd->sfd_flags |= SFDF_REMOVED;
}

struct section_file_data *findSectionInFile(struct section_file *sf,
					    struct file_section *type,
					    char *name)
{
	struct section_file_data *retval = 0, *sfd;
	int hash;

	lockMutex(sf);
	hash = hashName(name, SECTION_FILE_HASH_SIZE);
	sfd = sf->sf_sections[hash];
	while( sfd && !retval )
	{
		if( (type == sfd->sfd_type) &&
		    !strcmp(name, sfd->sfd_name) )
			retval = sfd;
		sfd = sfd->sfd_next;
	}
	unlockMutex(sf);
	return( retval );
}

int walkFileSections(struct section_file *sf,
		     int (*handler)(void *arg,
				    struct section_file *sf,
				    struct section_file_data *sfd),
		     void *arg)
{
	struct section_file_data *sfd;
	int retval = 1;

	if( !sf )
		return( 0 );
	lockMutex(sf);
	/* Walk over the ordered links */
	sfd = sf->sf_ordered_sections;
	while( sfd && retval )
	{
		retval = handler(arg, sf, sfd);
		sfd = sfd->sfd_order;
	}
	unlockMutex(sf);
	return( retval );
}

struct section_file_data *createFileSection(char *section_type,
					    char *section_name,
					    ...)
{
	struct section_file_data *retval = 0;
	struct file_section *fs;
	char *new_name;
	va_list args;

	va_start(args, section_name);
	/* Get the section type and ask it to create a section data object */
	if( (fs = findSectionType(section_type)) &&
	    (new_name = (char *)KMALLOC(strlen(section_name) + 1)) )
	{
		strcpy(new_name, section_name);
		if( !fs->fs_handler(fs, 0, SFM_CREATE,
				    &retval, new_name, &args) )
		{
			KFREE(new_name);
			retval = 0;
		}
	}
	va_end(args);
	return( retval );
}

void deleteFileSection(struct section_file_data *sfd)
{
	if( sfd )
	{
		KFREE(sfd->sfd_name);
		/* Ask the handler to clean up the object */
		sfd->sfd_type->fs_handler(sfd->sfd_type, 0, SFM_DELETE, sfd);
	}
}

#endif /* KAFFE_FEEDBACK */
