/*
 * feedback.c
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

#include "config.h"

#if defined(KAFFE_FEEDBACK)

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "gtypes.h"
#include "access.h"
#include "classMethod.h"
#include "lookup.h"
#include "support.h"
#include "debug.h"
#include "stringSupport.h"
#include "methodCache.h"
#include "thread.h"
#include "external.h"

#include "feedback.h"
#include "fileSections.h"

/* XXX Temporary for now until we define interface header file that
 * declares "translate"
 */
#if defined(TRANSLATOR)
#if defined(JIT3)
#include "jit3/machine.h"
#else
#include "jit/machine.h"
#endif
#endif

#define FEEDBACKSTACKSIZE 4096

struct section_file *kaffe_feedback_file = 0;
char *feedback_filename = 0;

int feedbackFile(char *filename)
{
	int retval = 0;

	/* Create a section file object to store the feedback information */
	if( (kaffe_feedback_file = createSectionFile()) )
	{
		setSectionFileName(kaffe_feedback_file, filename);
		/*
		 * Add an atexit call to syncFeedback so we know the
		 * information got out
		 */
		if( !atexit((void (*)(void))syncFeedback) )
			retval = 1;
		else
		{
			deleteSectionFile(kaffe_feedback_file);
			kaffe_feedback_file = 0;
		}
	}
	return( retval );
}

int syncFeedback(void)
{
	int retval = 0;

	if( kaffe_feedback_file && syncSectionFile(kaffe_feedback_file) )
	{
		retval = 1;
	}
	return( retval );
}

/*
 * Function that is called by the sections walker
 */
static int feedbackWalker(void *arg,
			  struct section_file *sf,
			  struct section_file_data *sfd)
{
	int retval = 1;

	/* Check for library section */
	if( sfd->sfd_type == &lib_section )
	{
		struct lib_section_data *lsd = (struct lib_section_data *)sfd;

		if( lsd->lsd_flags & LSDF_PRELOAD )
			loadNativeLibrary2(sfd->sfd_name, 0, 0, 0);
	}
	/* Check for jit-code section */
	else if( sfd->sfd_type == &jit_section )
	{
#if defined(TRANSLATOR)
		struct jit_section_data *jsd = (struct jit_section_data *)sfd;

		if( jsd->jsd_flags & JSDF_PRECOMPILE )
		{
			int len, lpc, sig_start = -1, meth_start = -1;
			Utf8Const *u8cname, *u8mname, *u8sig;
			Hjava_lang_Class *cls;
			char *full_name;

			/*
			 * Parse the name of the section to get the class,
			 * method, and signature
			 */
			full_name = sfd->sfd_name;
			len = strlen(full_name);
			for( lpc = len - 1;
			     (lpc >= 0) && (meth_start < 0);
			     lpc-- )
			{
				switch( full_name[lpc] )
				{
				case '(':
					sig_start = lpc;
					break;
				case '/':
					if( sig_start > 0 )
						meth_start = lpc + 1;
					break;
				}
			}
			if( (sig_start > 0) && (meth_start > 0) )
			{
				jobject loader = 0;
				errorInfo info;

				/* Get the right strings and find the class */
				u8cname = utf8ConstNew(full_name,
						       meth_start - 1);
				u8mname = utf8ConstNew(&full_name[meth_start],
						       sig_start - meth_start);
				u8sig = utf8ConstNew(&full_name[sig_start],
						     len - sig_start);
				if( u8cname && u8mname && u8sig &&
				    (cls = loadClass(u8cname, loader, &info)) )
				{
					Method *meth;

					if( (meth = findMethodLocal(cls,
								    u8mname,
								    u8sig)) &&
					    !(meth->accflags & ACC_NATIVE) )
					{
						if( translate(meth, &info) )
						{
						}
						else
						{
							dprintf(
								"Feedback: "
								" Precompile "
								"failed for "
								"%s!\n",
								full_name);
						}
					}
					else if( !meth )
					{
						dprintf(
							"Feedback: Didn't "
							"find method"
							" %s\n",
							full_name);
					}
				}
				else
				{
					dprintf(
						"Feedback: Couldn't load "
						"class %s\n",
						u8cname->data);
				}
				utf8ConstRelease(u8cname);
				utf8ConstRelease(u8mname);
				utf8ConstRelease(u8sig);
			}
			else
			{
				dprintf(
					"Feedback: Malformed method `%s'\n",
					full_name);
			}
		}
#else
		{
			static int precompile_msg = 0;

			if( !precompile_msg )
			{
				precompile_msg = 1;
				dprintf(
					"Feedback: Cannot precompile java for "
					"the interpreter\n");
			}
		}
#endif
	}
	return( retval );
}

static void feedbackRunnable(void *arg)
{
	/* Walk over the sections with our function */
	walkFileSections(kaffe_feedback_file, feedbackWalker, 0);
}

int processFeedback(void)
{
	int retval = 1;

	if( kaffe_feedback_file )
	{
		feedbackRunnable(0);
		retval = 1;
	}
	return( retval );
}

int feedbackJITMethod(char *method, char *address, int size, int precompile)
{
	struct section_file_data *sfd;
	struct jit_section_data *jsd;
	int retval = 0;

	if( !kaffe_feedback_file )
		return( 0 );
	lockMutex(kaffe_feedback_file);
	if( !(sfd = findSectionInFile(kaffe_feedback_file,
				      &jit_section, method)) )
	{
		/*
		 * If the section doesn't exist we need to create and add it.
		 * We only set precompile here since the user might've changed
		 * the file to specify otherwise.
		 */
		if( (sfd = createFileSection(jit_section.fs_name, method,
					     "precompile", precompile ?
					     "true" : "false",
					     NULL)) )
		{
			addSectionToFile(kaffe_feedback_file, sfd);
		}
	}
	if( sfd )
	{
		/* Set whatever attributes are interesting */
		jsd = (struct jit_section_data *)sfd;
		jsd->jsd_address = address;
		jsd->jsd_size = size;
		retval = 1;
	}
	unlockMutex(kaffe_feedback_file);
	return( retval );
}

int feedbackLibrary(char *name, int preload)
{
	struct section_file_data *sfd;
	int retval = 0;

	if( !kaffe_feedback_file )
		return( 0 );
	lockMutex(kaffe_feedback_file);
	if( !(sfd = findSectionInFile(kaffe_feedback_file,
				      &lib_section, name)) )
	{
		/*
		 * If the section doesn't exist we need to create and add it.
		 * We only set preload here since the user might've changed
		 * the file to specify otherwise.
		 */
		if( (sfd = createFileSection(lib_section.fs_name, name,
					     "preload", preload ?
					     "true" : "false",
					     NULL)) )
		{
			addSectionToFile(kaffe_feedback_file, sfd);
			retval = 1;
		}
	}
	else
		retval = 1;
	unlockMutex(kaffe_feedback_file);
	return( retval );
}

#endif /* KAFFE_FEEDBACK */
