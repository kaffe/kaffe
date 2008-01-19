dnl m4/classpath.m4
dnl
dnl Copyright (C) 2007 R. Grafl, A. Krall, C. Kruegel,
dnl C. Oates, R. Obermaisser, M. Platter, M. Probst, S. Ring,
dnl E. Steiner, C. Thalinger, D. Thuernbeck, P. Tomsich, C. Ullrich,
dnl J. Wenninger, Institut f. Computersprachen - TU Wien
dnl 
dnl This file is part of CACAO.
dnl 
dnl This program is free software; you can redistribute it and/or
dnl modify it under the terms of the GNU General Public License as
dnl published by the Free Software Foundation; either version 2, or (at
dnl your option) any later version.
dnl 
dnl This program is distributed in the hope that it will be useful, but
dnl WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl General Public License for more details.
dnl 
dnl You should have received a copy of the GNU General Public License
dnl along with this program; if not, write to the Free Software
dnl Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
dnl 02110-1301, USA.
dnl 

dnl dalibor cleaned it up a bit for kaffe.

dnl where is Java core library installed

AC_DEFUN([AC_CHECK_WITH_CLASSPATH_PREFIX],[
AC_MSG_CHECKING(where Java core library is installed)
AC_ARG_WITH([classpath-prefix],
            [AS_HELP_STRING(--with-classpath-prefix=<dir>,installation directory of Java core library [[default=/usr/local/classpath]])],
            [CLASSPATH_PREFIX=${withval}],
            [CLASSPATH_PREFIX=/usr/local/classpath])
AC_MSG_RESULT(${CLASSPATH_PREFIX})
AC_SUBST(CLASSPATH_PREFIX)
])


dnl where are Java core library classes installed

AC_DEFUN([AC_CHECK_WITH_CLASSPATH_CLASSES],[
AC_MSG_CHECKING(where Java core library classes are installed)
AC_ARG_WITH([classpath-classes],
            [AS_HELP_STRING(--with-classpath-classes=<path>,path to Java core library classes (includes the name of the file and may be flat) [[default=/usr/local/classpath/share/classpath/glibj.zip]])],
            [CLASSPATH_CLASSES=${withval}],
            [CLASSPATH_CLASSES=${CLASSPATH_PREFIX}/share/classpath/glibj.zip])
AC_MSG_RESULT(${CLASSPATH_CLASSES})
AC_SUBST(CLASSPATH_CLASSES)
])

dnl where are Java core library classes installed on the target

AC_DEFUN([AC_CHECK_WITH_TARGET_CLASSPATH_CLASSES],[
AC_MSG_CHECKING(where Java core library classes are installed on the target)
AC_ARG_WITH([target-classpath-classes],
            [AS_HELP_STRING(--with-target-classpath-classes=<path>,path to Java core library classes (includes the name of the file and may be flat) [[default=/usr/local/classpath/share/classpath/glibj.zip]])],
            [TARGET_CLASSPATH_CLASSES=${withval}],
            [TARGET_CLASSPATH_CLASSES=${CLASSPATH_CLASSES}])
AC_MSG_RESULT(${TARGET_CLASSPATH_CLASSES})
AC_SUBST(TARGET_CLASSPATH_CLASSES)
])

dnl where are Java core library native libraries installed

AC_DEFUN([AC_CHECK_WITH_CLASSPATH_LIBDIR],[
AC_MSG_CHECKING(where Java core library native libraries are installed)
AC_ARG_WITH([classpath-libdir],
            [AS_HELP_STRING(--with-classpath-libdir=<dir>,installation directory of Java core library native libraries [[default=/usr/local/classpath/lib]])],
            [CLASSPATH_LIBDIR=${withval}],
            [CLASSPATH_LIBDIR=${CLASSPATH_PREFIX}/lib])
AC_MSG_RESULT(${CLASSPATH_LIBDIR})

dnl expand CLASSPATH_LIBDIR to something that is usable in C code
AC_SUBST(CLASSPATH_LIBDIR)
])


dnl where are Java core library headers installed

AC_DEFUN([AC_CHECK_WITH_CLASSPATH_INCLUDEDIR],[
AC_MSG_CHECKING(where Java core library headers are installed)
AC_ARG_WITH([classpath-includedir],
            [AS_HELP_STRING(--with-classpath-includedir=<dir>,installation directory of Java core library headers [[default=/usr/local/classpath/include]])],
            [CLASSPATH_INCLUDEDIR=${withval}],
            [CLASSPATH_INCLUDEDIR=${CLASSPATH_PREFIX}/include])
AC_MSG_RESULT(${CLASSPATH_INCLUDEDIR})
])
