/*
 * Copyright (c) 2001  Motoyuki Kasahara
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

/*
 * This program provides macros, types and external variables related
 * to basic IPv6 programming such as PF_INET6.  This program is useful
 * for application which supports both IPv6 and traditional IPv4 systems.
 *
 * To use this program, include the header file.
 *
 *     #include "dummyin6.h"
 *
 * This program is intended to be used in combination with GNU Autoconf.
 * (see below).
 *
 * Restriction:
 * `struct sockaddr_storage' has no ss_ or __ss_ member (e.g. ss_family)
 * since the structure is merely alias of `sturct sockaddr_in'.
 */

/*
 * This program requires the following Autoconf macros:
 *   AC_C_CONST
 *   AC_STRUCT_IN6_ADDR
 *   AC_STRUCT_SOCKADDR_IN6
 *   AX_STRUCT_SOCKADDR_STORAGE
 *   AC_DECL_IN6ADDR_ANY
 *   AC_DECL_IN6ADDR_LOOPBACK
 */
#include "dummyin6.h"

#ifndef IN6ADDR_ANY_DECLARED
const struct in6_addr in6addr_any = IN6ADDR_ANY_INIT;
#endif

#ifndef IN6ADDR_LOOPBACK_DECLARED
const struct in6_addr in6addr_loopback = IN6ADDR_LOOPBACK_INIT;
#endif
