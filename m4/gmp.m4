# -*- Autoconf -*-
# Copyright (C) 2000, 2001, 2003 Free Software Foundation, Inc.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
# 02110-1301  USA

# serial 7

m4_define([_m4_LIB_GMP],
[AC_ARG_WITH(gmp,
[  --without-gmp           don't use GNU multiple precision arithmetic library],
[use_gmp=$withval], [use_gmp=yes])

case $use_gmp:$LIBADD_GMP:$ac_cv_header_gmp_h in
  *::yes)
    AC_MSG_WARN([gmp library not found or does not appear to work
                 but `gmp.h' is present])
    m4_cv_using_lib_gmp=no
    ;;
  *:-lgmp:no)
    AC_MSG_WARN([gmp works but `gmp.h' is missing])
    m4_cv_using_lib_gmp=no
    ;;
  yes:*:yes)
    m4_cv_using_lib_gmp=yes
    ;;
  no:*)
    m4_cv_using_lib_gmp=no
    ;;
esac
])# _m4_LIB_GMP


AC_DEFUN([m4_LIB_GMP],
[AC_PREREQ(2.56)dnl We use the new compiler based header checking in 2.56
AC_CHECK_HEADERS([gmp.h], [], [], [AC_INCLUDES_DEFAULT])
m4_pattern_allow([^m4_gmp_save_LIBS$])
m4_pattern_allow([^m4_cv_using_lib_gmp])
# Some versions of gmp provide mpq_init as a macro, so we need to
# include the header file, otherwise the detection will fail.
m4_gmp_save_LIBS="$LIBS"
LIBS="$LIBS -lgmp"
AC_TRY_LINK([#if HAVE_GMP_H
#  include <gmp.h>
#endif],
  [mpq_t n; mpq_init (n);],
  [LIBADD_GMP=-lgmp])
LIBS=$m4_gmp_save_LIBS
AC_SUBST([LIBADD_GMP])

AC_CACHE_CHECK([if using GNU multiple precision arithmetic library],
               [m4_cv_using_lib_gmp],
               [_m4_LIB_GMP])

# Don't try to link in libgmp if we are not using it after the last call
if test "$m4_cv_using_lib_gmp" = yes; then
  AC_DEFINE(USE_GMP, 1,
    [Define to 1 if using the GNU multiple precision library.])
fi

AC_SUBST([USE_GMP], [$m4_cv_using_lib_gmp])
])# m4_LIB_GMP
