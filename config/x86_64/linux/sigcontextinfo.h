/* Copyright (C) 2001, 2002 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301 USA.  */

#include <sys/ucontext.h>
#include <stddef.h>

/* Indices are defined if _GNU_SOURCE is set prior to include <sys/ucontext.h> */
#define REG_INDEX(NAME)	(offsetof(struct sigcontext, NAME) / sizeof(greg_t))

#ifndef REG_RIP
#define REG_RIP		REG_INDEX(rip) /* 16 */
#endif
#ifndef REG_RBP
#define REG_RBP		REG_INDEX(rbp) /* 10 */
#endif
#ifndef REG_RSP
#define REG_RSP		REG_INDEX(rsp) /* 15 */
#endif

#define GET_PC(ctx)	((void *) (ctx).uc_mcontext.gregs[REG_RIP])
#define GET_FRAME(ctx)	((void *) (ctx).uc_mcontext.gregs[REG_RBP])
#define GET_STACK(ctx)	((void *) (ctx).uc_mcontext.gregs[REG_RSP])
