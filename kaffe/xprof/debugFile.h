/*
 * debugFile.h
 * Routines for generating an assembly file with debugging information
 *
 * Copyright (c) 2000, 2004 University of Utah and the Flux Group.
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

#include "locks.h"

/**
 * The root structure for a debugging information file.
 */
struct debug_file {
	iLock *lock;		/**< Lock to serialize access to the file. */
	char *df_filename;	/**< The file name being written. */
	FILE *df_file;		/**< The FILE object to write to. */
	int df_current_type_id;	/**< The current type identifier. */
	char *df_high;		/**< The highest address seen so far. */
};

/**
 * Argument tags for the varargs addDebugInfo.
 */
typedef enum {
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
				 * Add information about a function.
				 */
	DIA_Parameter,		/*
				 * (char *name, Hjava_lang_Class *cl,
				 * int offset) Add a parameter for a function.
				 * Name is the name of the parameter, 'cl' is
				 * the parameter type, and 'offset' is the
				 * parameter offset in the stack frame.
				 */
	DIA_LocalVariable,	/*
				 * (char *name, char *sig, int offset)  Add a
				 * local variable for a function.  Similar to
				 * DIA_Parameter.
				 */
	DIA_LeftBrace,		/* (char *addr) */
	DIA_RightBrace,		/* (char *addr) */
	DIA_EndFunction,	/* (char *addr)  Mark the end of a function */
	DIA_SourceLine,		/* (int line, char *addr)  Add a source line */
	DIA_SourceFile,		/*
				 * (char *name, char *addr)  Add information
				 * about a source file.
				 */
	DIA_IncludeFile,	/*
				 * (char *name, char *addr)  Add information
				 * about an include file.
				 */
	DIA_Class,		/*
				 * (Hjava_lang_Class *cl)  Add type information
				 * about a class.
				 */
	DIA_Array,		/*
				 * (Hjava_lang_Class *cl)  Add type information
				 * about an array.
				 */
} df_tag_t;

/**
 * Create the root structure and corresponding output file.
 *
 * @param filename The debugging file name.
 * @return An initialized debug_file object.
 */
struct debug_file *createDebugFile(const char *filename);

/**
 * Delete the root structure and close the output file.
 *
 * @param The debug_file to delete.
 */
void deleteDebugFile(struct debug_file *df);

/**
 * Add debugging information to the file, the function takes a list of
 * arguments that indicate the type of information to add.  For example,
 * to add information about the `debugFile.c' file, you would call the function
 * like so:
 *
 * @code
 * addDebugInfo(df,
 *              DIA_SourceFile, "debugFile.c", debugFileStartAddress,
 *              DIA_DONE);
 * @endcode
 *
 * @param file The debugging file to add information to.
 * @param tag The tag list to be added to the file.
 * @return True on success, false otherwise.
 */
int addDebugInfo(struct debug_file *file, df_tag_t tag, ...);

/**
 * Default debugging file for the virtual machine.
 */
extern struct debug_file *machine_debug_file;

/**
 * Default name for the debugging file.
 */
extern const char *machine_debug_filename;

#endif /* __debugfile_h */
