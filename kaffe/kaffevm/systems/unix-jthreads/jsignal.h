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
 * Copyright (c) 2004
 *      The Kaffe.org's team.
 *
 * This file is licensed under the terms of the GNU Public License.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file.
 *
 * Written by Patrick Tullmann <tullmann@cs.utah.edu> and
 *            Godmar Back <gback@cs.utah.edu>
 *
 */
/*
 * Signal handling API
 *
 * This interface is mostly for other jthread files,
 * except for unblockAsyncSignals/blockAsyncSignals, which is 
 * exported to the VM.
 */

#ifndef UNIXJTHREAD_SIGNAL_H
#define UNIXJTHREAD_SIGNAL_H

void *registerSyncSignalHandler(int sig, void* handler);
void restoreSyncSignalHandler(int sig, void* handler);

void *registerAsyncSignalHandler(int sig, void* handler);
void restoreAsyncSignalHandler(int sig, void* handler);

void registerTerminalSignal(int sig, void* handler);

void KaffeJThread_ignoreSignal(int sig);
void clearSignal(int sig);
void unblockSignal(int sig);
void unblockAsyncSignals(void);
void blockAsyncSignals(void);

void detectStackBoundaries(jthread_t jtid, int mainThreadStackSize);

#endif /* UNIXJTHREAD_SIGNAL_H */
