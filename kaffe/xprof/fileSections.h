/*
 * fileSections.h
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

#ifndef __filesections_h
#define __filesections_h

#include "sectionFile.h"

enum {
	JSDB_PRECOMPILE	/* Precompile the method */
};

enum {
	JSDF_PRECOMPILE = (1L << JSDB_PRECOMPILE)
};

/* Structure that stores information about jitted code */
struct jit_section_data {
	struct section_file_data jsd_link;
	unsigned long jsd_flags;		/* Mask of the above flags */
	unsigned long jsd_size;			/* Size of the code */
	char *jsd_address;			/* Address of the code */
};

extern struct file_section jit_section;

enum {
	LSDB_PRELOAD	/* Preload the library */
};

enum {
	LSDF_PRELOAD = (1L << LSDB_PRELOAD)
};

/* Structure that stores information about dynamic libraries */
struct lib_section_data {
	struct section_file_data lsd_link;
	unsigned long lsd_flags;		/* Mask of the above flags */
};

extern struct file_section lib_section;

void installFileSections(void);

#endif /* __filesections_h */
