/*
 * This file was originally written for the Boehm-GC under the name
 * private/darwin_semaphores.h
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
#include "config.h"

#if defined(NEED_REPLACEMENT_SEMAPHORES)

#include <errno.h>
#include "repsemaphore.h"

#define REPSEM_VALUE_MAX 32767

int repsem_init(repsem_t *sem, int pshared, int value)
{
  int ret;
  
  if (value > REPSEM_VALUE_MAX)
    {
      errno = EINVAL;
      return -1;
    }
  
  if (pshared)
    {
      errno = ENOSYS;
      return -1;
    }

  sem->value = value;
    
  ret = pthread_mutex_init(&sem->mutex,NULL);
  if (ret < 0) 
    return -1;
  
  ret = pthread_cond_init(&sem->cond,NULL);
  if (ret < 0) 
    return -1;
  
  return 0;
}

int repsem_post(repsem_t *sem)
{
  if (pthread_mutex_lock(&sem->mutex) < 0)
    return -1;
  
  if (sem->value == REPSEM_VALUE_MAX)
    {
      errno = ERANGE;
      return -1;
    }
  sem->value++;

  if (pthread_cond_signal(&sem->cond) < 0)
    {
      pthread_mutex_unlock(&sem->mutex);
      return -1;
    }

  if (pthread_mutex_unlock(&sem->mutex) < 0)
    return -1;
  
  return 0;
}

int repsem_wait(repsem_t *sem)
{
  if (pthread_mutex_lock(&sem->mutex) < 0)
    return -1;
    
  while(sem->value == 0)
    {
      pthread_cond_wait(&sem->cond,&sem->mutex);
    }
  sem->value--;

  if (pthread_mutex_unlock(&sem->mutex) < 0)
    return -1;    

  return 0;
}

int repsem_trywait(repsem_t *sem)
{
  if (pthread_mutex_lock(&sem->mutex) < 0)
    return -1;

  if (sem->value == 0)
    {
      if (pthread_mutex_unlock(&sem->mutex) < 0)
	return -1;
      errno = EAGAIN;
      return -1;
    }
  
  sem->value--;
  
  if (pthread_mutex_unlock(&sem->mutex) < 0)
    return -1;

  return 0;
}

int repsem_getvalue(repsem_t *sem, int *sval)
{
  *sval = sem->value;
  return 0;
}

int repsem_destroy(repsem_t *sem)
{
  int ret;
  
  ret = pthread_cond_destroy(&sem->cond);
  if (ret < 0) 
    return -1;
  
  ret = pthread_mutex_destroy(&sem->mutex);
  if (ret < 0)
    return -1;
  
  return 0;
}

#endif
