/* Copyright (C) 1999, 2002 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Philip Blundell <philb@gnu.org>, 1999.

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

#define SIGCONTEXT int _a2, int _a3, int _a4, struct sigcontext
#define SIGCONTEXT_EXTRA_ARGS _a2, _a3, _a4,

/* The sigcontext structure changed between 2.0 and 2.1 kernels.  On any
   modern system we should be able to assume that the "new" format will be
   in use.  */

/* Kaffe supports just the new format. If you are stuck on an arm where you
   get compile errors, post an e-mail to the mailing list, and we'll try
   to come up with a configure test top automatically detect such beasts.
*/

#define GET_PC(ctx)	((void *) ctx.arm_pc)
#define GET_FRAME(ctx)	ADVANCE_STACK_FRAME ((void *) ctx.arm_fp)
#define GET_STACK(ctx)	((void *) ctx.arm_sp)

#define ADVANCE_STACK_FRAME(frm)	\
			((struct layout *)frm - 1)

#define CALL_SIGHANDLER(handler, signo, ctx) \
  (handler)((signo), SIGCONTEXT_EXTRA_ARGS (ctx))
