/*
 * jsignal.h
 * Interface for handling signals.
 *
 * Copyright (c) 1999
 * 	University of Utah, CSL.  All rights reserved.
 *
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * This file is licensed under the terms of the GNU Public License.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file.
 *
 * Written by Patrick Tullmann <tullmann@cs.utah.edu> and
 *            Godmar Back <gback@cs.utah.edu>
 */
/*
 * Signal handling API
 *
 * This files exports unblockAsyncSignals/blockAsyncSignals, for use by the VM.
 */

#ifndef __oskit_threads_jsignal_h
#define __oskit_threads_jsignal_h

void unblockAsyncSignals(void);
void blockAsyncSignals(void);

#endif /* __oskit_threads_jsignal_h */
