# ----------------------------------------------------------------------------
# $Id: gnu-crypto.m4,v 1.2 2004/03/22 14:25:53 dalibor Exp $
#
# Copyright (C) 2003 Free Software Foundation, Inc.
#
# This file is part of GNU Crypto.
#
# GNU Crypto is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# GNU Crypto is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING.  If not, write to the
#
#    Free Software Foundation Inc.,
#    59 Temple Place - Suite 330,
#    Boston, MA 02111-1307
#    USA
#
# Linking this library statically or dynamically with other modules is
# making a combined work based on this library.  Thus, the terms and
# conditions of the GNU General Public License cover the whole
# combination.
#
# As a special exception, the copyright holders of this library give
# you permission to link this library with independent modules to
# produce an executable, regardless of the license terms of these
# independent modules, and to copy and distribute the resulting
# executable under terms of your choice, provided that you also meet,
# for each linked independent module, the terms and conditions of the
# license of that module.  An independent module is a module which is
# not derived from or based on this library.  If you modify this
# library, you may extend this exception to your version of the
# library, but you are not obligated to do so.  If you do not wish to
# do so, delete this exception statement from your version.
# ----------------------------------------------------------------------------
#
# GNU Crypto m4 macro for use by VM Providers
#
# $Revision: 1.2 $
#

# process --with-gnu-crypto configure option.
# test if the GNU Crypto package is installed.  if no argument was specified
# with the option, this macro looks for a 'share/gnu-crypto.jar' file and
# a 'lib/lib-gnu-crypto.so' in the following places, in this order:
# /usr/local/gnu-crypto, /usr/local, /usr, /opt/gnu-crypto, and /opt.
# otherwise those two files are looked up under the designated location.
# in total, the following variables are set:
#
# USER_WANT_GNU_CRYPTO: an automake conditional; true if with-gnu-crypto is set,
# GNU_CRYPTO_HOME: the installation directory of GNU Crypto,
# GNU_CRYPTO_JAR: the fully qualified path to gnu-crypto.jar,
# JAVAX_CRYPTO_JAR: the fully qualified path to javax-crypto.jar,
# JAVAX_SECURITY_JAR: the fully qualified path to javax-security.jar.
# -----------------------------------------------------------------------------
AC_DEFUN([CHECK_GNU_CRYPTO],[
AC_ARG_WITH([gnu-crypto],
            AS_HELP_STRING([--with-gnu-crypto],
			   [path to GNU Crypto install directory.  if unspecified, /usr/local/gnu-crypto, /usr/local, /usr, /opt/gnu-crypto, and /opt are considered, in that order]),
            [if test "x${withval}" != x && test "x${withval}" != xyes && test "x${withval}" != xno; then
              AC_MSG_CHECKING([${withval}])
              _CHECK_GNU_CRYPTO_HOME(${withval})
              if test x$GNU_CRYPTO_HOME = x ; then
                AC_MSG_RESULT(no)
                AC_MSG_ERROR([cannot find designated GNU Crypto install directory])
              else
                AC_MSG_RESULT(yes)
                with_gnu_crypto=true
              fi
            elif test "x${withval}" != xno; then
              AC_MSG_NOTICE([no value supplied --with-gnu-crypto.  will look in default locations])
              _FIND_GNU_CRYPTO_HOME([/usr/local/gnu-crypto /usr/local /usr /opt/gnu-crypto /opt])
              if test x$GNU_CRYPTO_HOME = x ; then
                AC_MSG_ERROR([cannot find GNU Crypto install directory.  install it and/or specify its location using --with-gnu-crypto])
              fi
              with_gnu_crypto=true
            else
              with_gnu_crypto=false
            fi],
            [with_gnu_crypto=false])
GNU_CRYPTO_HOME=`(cd ${GNU_CRYPTO_HOME}; pwd)`
AC_SUBST(GNU_CRYPTO_HOME)
if test -r ${GNU_CRYPTO_HOME}/share/gnu-crypto.jar ; then
  GNU_CRYPTO_JAR=${GNU_CRYPTO_HOME}/share/gnu-crypto.jar
  AC_SUBST(GNU_CRYPTO_JAR)
fi
if test -r ${GNU_CRYPTO_HOME}/share/javax-crypto.jar ; then
  JAVAX_CRYPTO_JAR=${GNU_CRYPTO_HOME}/share/javax-crypto.jar
  AC_SUBST(JAVAX_CRYPTO_JAR)
fi
if test -r ${GNU_CRYPTO_HOME}/share/javax-security.jar ; then
  JAVAX_SECURITY_JAR=${GNU_CRYPTO_HOME}/share/javax-security.jar
  AC_SUBST(JAVAX_SECURITY_JAR)
fi
AM_CONDITIONAL(USER_WANT_GNU_CRYPTO, test "x${with_gnu_crypto}" = xtrue)
])# CHECK_GNU_CRYPTO


# given a list of arguments, this macro tries repeatedly and for each element
# of the input list, to (a) assert that the argument is indeed a directory,
# and (b) it contains under it both 'lib/lib-gnu-crypto.so' and
# 'share/gnu-crypto.jar' files.
# -----------------------------------------------------------------------------
AC_DEFUN([_FIND_GNU_CRYPTO_HOME],[
AC_MSG_CHECKING([for GNU Crypto installation directory])
for _F in $1 ; do
  _CHECK_GNU_CRYPTO_HOME(${_F})
  if test x$GNU_CRYPTO_HOME != x ; then
    break
  fi
done
if test x${GNU_CRYPTO_HOME} = x ; then
  AC_MSG_RESULT([not found])
else
  AC_MSG_RESULT([${GNU_CRYPTO_HOME}])
fi])# _FIND_GNU_CRYPTO_HOME


# given a directory as an input, this macro checks if the two files
# 'lib/lib-gnu-crypto.so' and 'share/gnu-crypto.jar' exist under it.  if they
# do, then GNU_CRYPTO_DIR is set to that argument.
# -----------------------------------------------------------------------------
AC_DEFUN([_CHECK_GNU_CRYPTO_HOME],
[if test -r $1/lib/lib-gnu-crypto.so || test -r $1/share/gnu-crypto.jar ; then
  GNU_CRYPTO_HOME=$1
fi])# _CHECK_GNU_CRYPTO_HOME
