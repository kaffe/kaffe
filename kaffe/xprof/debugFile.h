/*
 * debugFile.h
 * Routines for generating an assembly file with debugging information
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

#ifndef __debugfile_h
#define __debugfile_h

#include <stdio.h>

/* The root structure for a debugging information file */
struct debug_file {
	char *df_filename;
	FILE *df_file;
};

/* Argument tags for the varargs addDebugInfo */
enum {
	DIA_DONE,		/* Terminate the tag list */
	DIA_Comment,		/* (char *name)  Add a comment to the file */
	DIA_FunctionSymbol,	/*
				 * (struct mangled_method *mm,
				 *  char *addr, int size) Add a mangled
				 * symbol to the file.  If size > 0 then
				 * then another symbol is added with the same
				 * name and `_end' appeneded to it.
				 */
	DIA_FunctionSymbolS,	/*
				 * (char *name, char *addr, int size) Add a
				 * symbol to the file.  If size > 0 then then
				 * another symbol is added with the same name
				 * and `_end' appeneded to it.
				 */
	DIA_Symbol,		/*
				 * (char *name, char *addr) Add a generic
				 * symbol to the file.
				 */
	DIA_Function,		/*
				 * (Method *, struct mangled_method *mm,
				 *  int line, char *addr, int size)
				 * Add a information about a function
				 */
	DIA_EndFunction,	/* (char *addr)  Mark the end of a function */
	DIA_SourceLine,		/* (int line, char *addr)  Add a source line */
	DIA_SourceFile		/*
				 * (char *name, char *addr)  Add information
				 * about a source file
				 */
};

/*
 * Create the root structure and corresponding output file.
 */
struct debug_file *createDebugFile(char *filename);
/*
 * Delete the root structure and close the output file.
 */
void deleteDebugFile(struct debug_file *df);
/*
 * Add debugging information to the file, the function takes a list of
 * arguments that indicate the type of information to add.  For example,
 * to add information about the `debugFile.c' file, you would call the function
 * like so:
 *
 * addDebugInfo(df,
 *              DIA_SourceFile, "debugFile.c", debugFileStartAddress,
 *              DIA_DONE);
 */
int addDebugInfo(struct debug_file *file, int tag, ...);

/* Default debugging file for the virtual machine */
extern struct debug_file *machine_debug_file;
extern char *machine_debug_filename;

#endif /* __debugfile_h */
