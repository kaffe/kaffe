/*
 * This file was originally written for the Boehm-GC under the name
 * private/darwin_semaphores.h. It has been modified to be included in
 * libreplace.
 *
 * Copyright 1988, 1989 Hans-J. Boehm, Alan J. Demers
 * Copyright (c) 1991-1995 by Xerox Corporation.  All rights reserved.
 * Copyright (c) 1996-1999 by Silicon Graphics.  All rights reserved.
 * Copyright (c) 1999-2001 by Hewlett-Packard. All rights reserved.
 *
 * Copyright (c) 2004  Guilhem Lavaux.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#ifndef __REP_SEMAPHORE_H

#include "config.h"

#if !defined(NEED_REPLACEMENT_SEMAPHORES) && defined(HAVE_SEMAPHORE_H)
#include <semaphore.h>

#define repsem_init sem_init
#define repsem_wait sem_wait
#define repsem_getvalue sem_getvalue
#define repsem_post sem_post
#define repsem_destroy sem_destroy
#define repsem_t sem_t

#elif defined(HAVE_PTHREAD_H)

#include <pthread.h>

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int value;
} repsem_t;

int repsem_init(repsem_t *sem, int pshared, int value);
int repsem_post(repsem_t *sem);
int repsem_wait(repsem_t *sem);
int repsem_destroy(repsem_t *sem);
int repsem_getvalue(repsem_t *sem, int *sval);
int repsem_trywait(repsem_t *sem);

#endif

#endif
