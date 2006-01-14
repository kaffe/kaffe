dnl Process this file with autoconf to produce a configure script.
AC_PREREQ(2.59)
AC_INIT
AC_CONFIG_AUX_DIR([scripts])
AC_CONFIG_SRCDIR([jartool.h])
AM_INIT_AUTOMAKE(fastjar, 0.92-gcc)
AC_CONFIG_HEADERS([config.h])

dnl Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PATH_PROG(RM, rm, /bin/rm, $PATH:/bin:/usr/bin:/usr/local/bin)
AC_PATH_PROG(CP, cp, /bin/cp, $PATH:/bin:/usr/bin:/usr/local/bin)
AC_PATH_PROG(STRIP, strip, /usr/bin/strip, $PATH:/bin:/usr/bin:/usr/local/bin)
AC_PATH_PROG(CHMOD, chmod, /bin/chmod, $PATH:/bin:/usr/bin:/usr/local/bin)
AC_EXEEXT

AC_PROG_LIBTOOL
## Let users override the linker used by libtool, but default to $CC.
CCLD=${CCLD:-'$(CC)'}
AC_SUBST(CCLD)

AM_MAINTAINER_MODE

AX_CFLAGS_WARN_ALL

AX_CFLAGS_GCC_OPTION([-W])
AX_CFLAGS_GCC_OPTION([-pedantic])
AX_CFLAGS_GCC_OPTION([-Wstrict-prototypes])
AX_CFLAGS_GCC_OPTION([-Wmissing-prototypes])
AX_CFLAGS_GCC_OPTION([-Wwrite-strings])

dnl Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_STRUCT_TM
AC_CHECK_HEADERS(fcntl.h unistd.h sys/param.h stdlib.h limits.h)

AC_CHECK_FUNCS(ftruncate _chsize)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_OFF_T
AC_STRUCT_TM

# mkdir takes a single argument on some systems.
gcc_AC_FUNC_MKDIR_TAKES_ONE_ARG

dnl Check for type-widths
AC_COMPILE_CHECK_SIZEOF(char)
AC_COMPILE_CHECK_SIZEOF(short)
AC_COMPILE_CHECK_SIZEOF(int)
AC_COMPILE_CHECK_SIZEOF(long)
AC_CHECK_TYPES([long long],[AC_COMPILE_CHECK_SIZEOF(long long)])

dnl Check byte order
AC_C_BIGENDIAN_CROSS

AC_ARG_WITH([system-zlib],
        AS_HELP_STRING([--with-system-zlib],
                       [Use installed zlib on the system instead of Kaffe's copy]),
        [],
        [with_system_zlib=no]
)

if test x"$with_system_zlib" != x"no" ; then
  # Check for libraries for zip library.
  AC_CHECK_LIB(z,deflate)
fi

AM_CONDITIONAL(COND_SYSTEM_ZLIB, test x"$with_system_zlib" != x"no")

# GCC LOCAL CHANGE
# We would like to our source tree to be readonly.  However when releases or
# pre-releases are generated, the man pages need to be included as they are
# converted from the texi files via perl which we don't require end users to
# have installed.
# Therefore we have --enable-generated-files-in-srcdir to do just that.

AC_MSG_CHECKING([whether to place generated files in the source directory])
  dnl generated-files-in-srcdir is disabled by default
  AC_ARG_ENABLE(generated-files-in-srcdir, 
[  --enable-generated-files-in-srcdir
                          put copies of generated files in source dir
                          intended for creating source tarballs for users
                          without texinfo, perl, bison or flex.],
      generated_files_in_srcdir=$enableval,
      generated_files_in_srcdir=no)

AC_MSG_RESULT($generated_files_in_srcdir)
AM_CONDITIONAL(GENINSRC, test x$generated_files_in_srcdir = xyes)

gl_GETOPT

#TL_AC_GCC_VERSION([$srcdir/..])

AC_CONFIG_FILES([Makefile install-defs.sh])
AC_OUTPUT
