/*
 * feedback.h
 * Routines for generating information that can be fed back into kaffe for
 * future runs.
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

#ifndef __feedback_h
#define __feedback_h

#include "sectionFile.h"

/*
 * Specify the file to read/store any information
 */
int feedbackFile(char *filename);
/*
 * Process information read from the feedback file
 */
int processFeedback(void);
/*
 * Synchronize the feedback file, reading in anything new, or writing out
 * new information.
 */
int syncFeedback(void);
/*
 * Add information about a jitted method
 */
int feedbackJITMethod(char *method, char *address, int size, int precompile);
/*
 * Add information about a dynamically loaded library
 */
int feedbackLibrary(char *name, int preload);

/* This is the section file that holds all the feedback information. */
extern struct section_file *kaffe_feedback_file;
extern char *feedback_filename;

#endif /* __feedback_h */
