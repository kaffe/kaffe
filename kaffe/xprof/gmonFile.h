/*
 * gmonFile.h
 * gmon_out file handling routines
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

#ifndef __gmonfile_h
#define __gmonfile_h

#include <stdio.h>
#include <sys/types.h>

#include "gmon_out.h"

/* States for a gmon_file */
enum {
	GFS_NONE,	/* Just a barrier */
	GFS_ERROR,	/* There was an error writing the file */
	GFS_RECORD,	/* Adding records */
	GFS_SAMPLES,	/* Writing samples for a hist record */
};

struct gmon_file {
	char *gf_name;	/* The file name */
	FILE *gf_file;	/* The file handle */
	char *gf_low;	/* Lowest sample (usually the PC) for the file */
	char *gf_addr;	/* Tracks current sample when writing out the file */
	char *gf_high;	/* Highest sample for the file */
	int gf_state;	/* State of the file */
};

/* Tags for writeGmonRecord() */
enum {
	GRA_DONE,		/* () Terminates the tag list */

	GRA_Type,		/*
				 * (enum {
				 *    GMON_TAG_TIME_HIST,
				 *    GMON_TAG_CG_ARG,
				 *    GMON_TAG_BB_COUNT}) Type of record to
				 * add
				 */
	GRA_Rewrite,		/* (long) Rewrite the record at the index */
	
	GRA_LowPC,		/* (char *) Lowest PC for a histogram */
	GRA_HighPC,		/* (char *) Highest PC for a histogram */
	GRA_PCSize,		/*
				 * (unsigned int) Sets the highest PC to
				 * GRA_LowPC + GRA_PCSize
				 */
	GRA_Dimension,		/* (string) The physical dimension */
	GRA_DimensionAbbrev,	/* (char) The abbreviation of the dimension */
	GRA_ProfilingRate,	/* The profiling rate */

	GRA_FromPC,		/* (char *) The from PC for a call arc */
	GRA_SelfPC,		/* (char *) The self PC for a call arc */
	GRA_Count,		/* (int) The number of arc traversals */
};

/*
 * Create a gmon_file structure, also opens the given file for writing.
 */
struct gmon_file *createGmonFile(char *filename);
/*
 * Delete a gmon_file structure, this also closes the file.
 */
void deleteGmonFile(struct gmon_file *gf);
/*
 * Write a record to the gmon file, if the record is a histogram header, than
 * it needs to be followed by a call to writeGmonSamples, to write the samples
 * out.
 */
long writeGmonRecord(struct gmon_file *gf, int tag, ...);
/*
 * Write out a subset of the histograms to the gmon file.  If `addr' is not
 * contiguous with previous calls to this function the gap will be filled
 * in with zeroes.
 */
int writeGmonSamples(struct gmon_file *gf, char *addr, short *bins, size_t count);
/*
 * A walker function for walking over a memory profile and calling
 * writeGmonSamples.
 */
int gmonSampleWalker(void *handle, char *addr, short *bins, size_t size);
/*
 * Fill the gap up to `addr' with zeros in the file.
 */
int fillGmonSamples(struct gmon_file *gf, char *addr);

#endif /* __gmonfile_h */
