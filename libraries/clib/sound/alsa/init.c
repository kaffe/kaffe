/*
 *	init.c
 */

/*
 *  Copyright (c) 1999 - 2001 by Matthias Pfisterer <Matthias.Pfisterer@gmx.de>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as published
 *   by the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details.
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdio.h>
#include <dlfcn.h>


/*
  These methods are needed to circumvent a problem with the dynamic linker:
  The VM calls dlopen() to load this library, which depends on libasound.so.
  libasound.so, in turn, uses dlopen() to load plugins. It is this chained
  calling of dlopen() which results in the second call not resolving symbols
  properly. Making the symbols of libasound.so global here (RTLD_GLOBAL)
  solves the problem.
 */
/* commented out by Dalibor Topic <robilad@yahoo.com>
 * because gcc doesn't like it when building the library with
 * libtool support.
 */
/*
 *
 * #include "../common/debug.h"
 *
 * static void*	asound_dl_handle = NULL;
 */
/*
void _init(void)
{
	if (debug_flag) { (void) fprintf(debug_file, "_init(): begin\n"); }
	asound_dl_handle = dlopen("libasound.so", RTLD_LAZY | RTLD_GLOBAL);
	if (debug_flag) { (void) fprintf(debug_file, "_init(): result: %p\n", asound_dl_handle); }
	if (debug_flag) { (void) fprintf(debug_file, "_init(): end\n"); }
}



void _fini(void)
{
	if (debug_flag) { (void) fprintf(debug_file, "_fini(): begin\n"); }
	if (asound_dl_handle != NULL)
	{
		if (debug_flag) { (void) fprintf(debug_file, "_fini(): closeing handle\n"); }
		(void) dlclose(asound_dl_handle);
	}
	if (debug_flag) { (void) fprintf(debug_file, "_fini(): end\n"); }
}
*/


/*** init.c ***/
