/*
 * generic/gentypes.h
 * Generic code for atomic operations.
 *
 * Copyright (c) 2005
 *       Kaffe.org contributors. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#ifndef __KAFFE_GENTYPES_H
#define __KAFFE_GENTYPES_H

#include "config.h"

#if defined(HAVE_INTTYPES_H)
#include <inttypes.h>
#endif

#if defined(HAVE_STDINT_H)
#include <stdint.h>
#endif

#if !defined(HAVE_INT_FAST8_T)
typedef int8_t int_fast8_t;
#endif

#if !defined(HAVE_INT_FAST16_T)
typedef int16_t int_fast16_t;
#endif

#if !defined(HAVE_INT_FAST32_T)
typedef int32_t int_fast32_t;
#endif

#if !defined(HAVE_INT_FAST64_T)
typedef int64_t int_fast64_t;
#endif

#if !defined(HAVE_UINTMAX_T)
typedef uint64_t uintmax_t;
#endif

#if !defined(HAVE_UINTMAX_T)
typedef int64_t intmax_t;
#endif

#endif
