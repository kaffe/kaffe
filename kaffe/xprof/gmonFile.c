/*
 * gmonFile.c
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

#include "config.h"

#if defined(KAFFE_XPROFILER)

#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/gmon.h>

#include "kaffe/jmalloc.h"

#include "gmonFile.h"

#define min(x, y) ((x < y) ? x : y)

struct gmon_file *createGmonFile(char *filename)
{
	struct gmon_file *retval = 0;
	FILE *file;

	if( (file = fopen(filename, "w")) )
	{
		struct gmon_hdr gh;
		
		/* Write out the header */
		memcpy(gh.cookie, GMON_MAGIC, sizeof(gh.cookie));
		*((int *)gh.version) = GMON_VERSION;
		bzero(&gh.spare, sizeof(gh.spare));
		fwrite(&gh, sizeof(struct gmon_hdr), 1, file);
		if( !ferror(file) &&
		    (retval = (struct gmon_file *)
		     KMALLOC(sizeof(struct gmon_file))) )
		{
			retval->gf_name = filename;
			retval->gf_file = file;
			retval->gf_low = 0;
			retval->gf_addr = 0;
			retval->gf_high = 0;
			/* Start out writing records */
			retval->gf_state = GFS_RECORD;
		}
		else
		{
			fclose(file);
			remove(filename);
		}
	}
	return( retval );
}

void deleteGmonFile(struct gmon_file *gf)
{
	if( gf )
	{
		fclose(gf->gf_file);
		/* Delete the file if there was an error in writing */
		if( gf->gf_state == GFS_ERROR )
			remove(gf->gf_name);
		KFREE(gf);
	}
}

long writeGmonRecord(struct gmon_file *gf, int tag, ...)
{
	struct gmon_cg_arc_record cga;
	long retval = 0, rewrite = -1;
	struct gmon_hist_hdr ghh;
	unsigned char hdr_tag;
	va_list args;

	va_start(args, tag);
	/* Walk over the tag list getting whatever info we care about */
	while( tag != GRA_DONE )
	{
		switch( tag )
		{
		case GRA_Type:
			hdr_tag = va_arg(args, int);
			/* Initialize the record structure */
			switch(hdr_tag)
			{
			case GMON_TAG_TIME_HIST:
				bzero(ghh.low_pc, sizeof(ghh.low_pc));
				bzero(ghh.high_pc, sizeof(ghh.high_pc));
				bzero(ghh.hist_size, sizeof(ghh.hist_size));
				*((int *)ghh.prof_rate) = 100;
				strcpy(ghh.dimen, "seconds");
				ghh.dimen_abbrev = 's';
				break;
			case GMON_TAG_CG_ARC:
				bzero(cga.from_pc, sizeof(cga.from_pc));
				bzero(cga.self_pc, sizeof(cga.self_pc));
				bzero(cga.count, sizeof(cga.count));
				break;
			}
			break;
		case GRA_Rewrite:
			rewrite = va_arg(args, long);
			break;

			/* Histogram attributes */
		case GRA_LowPC:
			{
				char *lowpc = va_arg(args, char *);
				
				*((char **)ghh.low_pc) = lowpc;
				gf->gf_low = lowpc;
			}
			break;
		case GRA_HighPC:
			*((char **)ghh.high_pc) = va_arg(args, char *);
			break;
		case GRA_PCSize:
			*((char **)ghh.high_pc) = *((char **)ghh.low_pc) +
				va_arg(args, int);
			break;
		case GRA_Dimension:
			strcpy(ghh.dimen, va_arg(args, char *));
			break;
		case GRA_DimensionAbbrev:
			ghh.dimen_abbrev = va_arg(args, int);
			break;
		case GRA_ProfilingRate:
			*((int *)ghh.prof_rate) = va_arg(args, int);
			break;

			/* Call graph attributes */
		case GRA_FromPC:
			*((char **)cga.from_pc) = va_arg(args, char *);
			break;
		case GRA_SelfPC:
			*((char **)cga.self_pc) = va_arg(args, char *);
			break;
		case GRA_Count:
			*((int *)cga.count) = va_arg(args, int);
			break;

		default:
			break;
		}
		tag = va_arg(args, int);
	}

	if( rewrite == -1 )
	{
		assert(gf->gf_state == GFS_RECORD);
		/* Write out the tag */
		fwrite(&hdr_tag, sizeof(hdr_tag), 1, gf->gf_file);
	}
	if( !ferror(gf->gf_file) )
	{
		/* Write out the actual record */
		switch(hdr_tag)
		{
		case GMON_TAG_TIME_HIST:
			*((int *)ghh.hist_size) = (*((char **)ghh.high_pc) -
						   *((char **)ghh.low_pc)) /
				HISTFRACTION;
			gf->gf_high = *((char **)ghh.high_pc);
			retval = ftell(gf->gf_file);
			/* If this is a rewrite, seek back to the record */
			if( rewrite != -1 )
				fseek(gf->gf_file, rewrite, SEEK_SET);
			fwrite(&ghh, sizeof(ghh), 1, gf->gf_file);
			if( !ferror(gf->gf_file) )
			{
				gf->gf_state = GFS_SAMPLES;
				/*
				 * If it was a rewrite, seek forward to where
				 * we were
				 */
				if( rewrite != -1 )
				{
					/*
					 * If the high was changed we might
					 * need to switch states.
					 */
					if( gf->gf_addr == gf->gf_high )
						gf->gf_state = GFS_RECORD;
					fseek(gf->gf_file, retval, SEEK_SET);
				}
			}
			else
			{
				gf->gf_state = GFS_ERROR;
				retval = 0;
			}
			gf->gf_addr = gf->gf_low;
			break;
		case GMON_TAG_CG_ARC:
			retval = ftell(gf->gf_file);
			/* If this is a rewrite seek back to the record */
			if( rewrite != -1 )
				fseek(gf->gf_file, rewrite, SEEK_SET);
			fwrite(&cga, sizeof(cga), 1, gf->gf_file);
			if( !ferror(gf->gf_file) )
			{
				/*
				 * If it was a rewrite, seek forward to where
				 * we were
				 */
				if( rewrite != -1 )
					fseek(gf->gf_file, retval, SEEK_SET);
			}
			else
			{
				gf->gf_state = GFS_ERROR;
				retval = 0;
			}
			break;
		default:
			break;
		}
	}
	else
	{
		gf->gf_state = GFS_ERROR;
	}
	va_end(args);
	return( retval );
}

int writeGmonSamples(struct gmon_file *gf, char *addr, short *bins, size_t size)
{
	int retval = 1;

	assert(gf->gf_state == GFS_SAMPLES);
	/* Fill any gaps between the current address and the one we're given */
	retval = fillGmonSamples(gf, addr);
	if( retval )
	{
		fwrite(bins, sizeof(short), size, gf->gf_file);
		if( !ferror(gf->gf_file) )
		{
			/* Increment the current sample index */
			gf->gf_addr += (size * HISTFRACTION);
			if( gf->gf_addr == gf->gf_high )
			{
				/* Switch back to writing records */
				gf->gf_state = GFS_RECORD;
			}
			else
			{
				assert( gf->gf_addr < gf->gf_high );
			}
		}
		else
		{
			gf->gf_state = GFS_ERROR;
			retval = 0;
		}
	}
	else
		gf->gf_state = GFS_ERROR;
	return( retval );
}

int fillGmonSamples(struct gmon_file *gf, char *addr)
{
	short filler = 0;
	int retval = 1;
	
	/* If there was a gap in writing the samples then fill it with zeros */
	for( ; (gf->gf_addr < addr) && retval; gf->gf_addr += HISTFRACTION )
	{
		fwrite(&filler, sizeof(filler), 1, gf->gf_file);
	}
	if( !ferror(gf->gf_file) )
	{
		/* Switch back to writing records */
		if( gf->gf_addr == gf->gf_high )
			gf->gf_state = GFS_RECORD;
	}
	else
		gf->gf_state = GFS_ERROR;
	return( retval );
}

int gmonSampleWalker(void *handle, char *addr, short *bins, size_t size)
{
	struct gmon_file *gf = handle;

	return( !writeGmonSamples(gf, addr, bins, size) );
}

#endif /* KAFFE_XPROFILER */
