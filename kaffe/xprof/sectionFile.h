/*
 * sectionFile.h
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

#ifndef __sectionfile_h
#define __sectionfile_h

/*
 * The section file code reads and writes files that can be used for storing
 * interesting information.  The files themselves are just plain text thats
 * formatted with a number of directives to drive the interpretation of the
 * rest of the data.  The current set of recognized directives are:
 *
 *   %include "<filename>" - Include another file
 *
 *   %begin <type> <name> - Begin a section with the given type and name
 *     <attribute1 name> <attribute1 value> - Specify a value for an attribute
 *     ...
 *     <attributeN name> <attributeN value>
 *   %end - End a section
 *   # - Starts a comment that goes until the end of the line
 *
 * The code tries to keep the file pristine so the user can edit it without
 * things moving around and disappearing in future edits.
 *
 * The internals of the system works by having the program define a set of
 * types and handlers that can translate between the text names and values in
 * the file and the internal data structures.  The section file takes care
 * of the majority of the work by parsing the outer structure of the file
 * as well as providing functions for handling tasks common to all handlers.
 */

#include <time.h>
#include <stdio.h>

#include "gtypes.h"
#include "locks.h"

/* Identifiers for operations that file_section_handler's implement */
enum {
	SFM_CREATE, /*
		     * (struct section_file_data **out_sfd,
		     *  char *name,
		     *  va_list values) Create a new section_data structure
		     */
	SFM_DELETE, /* (struct section_file_data *sfd) Delete section */
	SFM_CACHE, /*
		    * (struct section_file_data *sfd,
		    *  struct parse_state *ps,
		    *  char *line,
		    *  int len) Read the data from the file
		    */
	SFM_FLUSH  /*
		    * (struct section_file_data *sfd,
		    *  struct parse_state *ps,
		    *  char *line, int line_len,
		    *  FILE *out_file) Write the data to the file
		    */
};

/* Forward decls */
struct section_file;
struct file_section;
struct parse_state;

/* Typedef for a handler function for a section */
typedef int (*file_section_handler_t)(struct file_section *,
				      struct section_file *,
				      int method,
				      ...);

/* Structure specifying a type of section in a file */
struct file_section {
	struct file_section *fs_next;		/* Hash table link */
	char *fs_name;				/* Name of the section */
	file_section_handler_t fs_handler;	/* Handler for this section */
};

/* Bit numbers for section_file_data flags */
enum {
	SFDB_DIRTY,	/* Theres new data in the section */
	SFDB_REMOVED	/* The section needs to be removed from the file */
};

/* Flag bits for section_file_data flags */
enum {
	SFDF_DIRTY = (1L << SFDB_DIRTY),
	SFDF_REMOVED = (1L << SFDB_REMOVED)
};

/* Stores data for a section */
struct section_file_data {
	struct section_file_data *sfd_next;	/* Hash table link */
	struct section_file_data *sfd_order;	/* Time ordered link */
	struct file_section *sfd_type;		/* The type of this section */
	unsigned long sfd_flags;		/* Flags for this section */
	char *sfd_name;				/* Name of the section */
};

/* The number of buckets for the section_file hash table */
#define SECTION_FILE_HASH_SIZE 31

/* The root structure for a section file */
struct section_file {
	iLock *lock;
	char *sf_filename;	/* The name of the file */
	time_t sf_time;		/* The time stamp for the last file sync */
	/*
	 * Time ordered link through the sections, the earliest added is the
	 * first in the list
	 */
	struct section_file_data *sf_ordered_sections;
	/* Pointer to the last order pointer in the sf_ordered_sections list */
	struct section_file_data **sf_last_section;
	/* The hash table of data sections */
	struct section_file_data *sf_sections[SECTION_FILE_HASH_SIZE];
};

/*
 * Create a section file structure
 */
struct section_file *createSectionFile(void);
/*
 * Delete the section file structure
 */
void deleteSectionFile(struct section_file *sf);
/*
 * Set the file name of the section file
 */
void setSectionFileName(struct section_file *sf, char *name);
/*
 * Synchronize the section_file structure with the actual file.
 */
int syncSectionFile(struct section_file *sf);
/*
 * Add a section type to the system
 */
void addSectionType(struct file_section *fs);
/*
 * Find a section type
 */
struct file_section *findSectionType(char *name);
/*
 * Add a section to a file
 */
void addSectionToFile(struct section_file *sf, struct section_file_data *sfd);
/*
 * Schedule a section to be removed from the file
 */
void remSectionFromFile(struct section_file *sf,
			struct section_file_data *sfd);
/*
 * Find a section in a file
 */
struct section_file_data *findSectionInFile(struct section_file *sf,
					    struct file_section *type,
					    char *name);
/*
 * Create a file section of the given type, with the given name
 */
struct section_file_data *createFileSection(char *section_type,
					    char *section_name,
					    ...);
/*
 * Delete a file section data object
 */
void deleteFileSection(struct section_file_data *sfd);
/*
 * Walk over the collection of sections
 */
int walkFileSections(struct section_file *sf,
		     int (*handler)(void *arg,
				    struct section_file *sf,
				    struct section_file_data *sfd),
		     void *arg);
/*
 * Helper function for file_section handlers to parse a line into an identifier
 * and value
 */
int parseSectionLine(struct parse_state *ps, char **tag, char **value,
		     FILE *out_file);
/*
 * Helper function for file_section handlers to parse a flag string
 * (e.g. "true", "false", etc...) and return the `flags' bitmask with the
 * `flag_bit' set/cleared.
 */
unsigned long parseFlagString(char *value,
			      unsigned long flags,
			      unsigned long flag_bit);
/*
 * Helper function for file_section handlers to get a string which that
 * indicates whether `flag_bit' is set/cleared in `flags'.  The `value'
 * string should be the previous value of the flag, and the string
 * returned tries mimic the value.  For example, if value is "true", but
 * the bit is cleared in the flags, the returned string is "false", instead
 * of "0, "f", etc...
 */
char *makeFlagString(unsigned long flags, unsigned long flag_bit, char *value);

#endif /* __sectionfile_h */
