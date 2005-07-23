dnl Checks if we need to link to an external library on solaris
dnl to get some pthread functions.
dnl
dnl Derived from Python 2.3.4 check in their GPL-comaptible configure.in.
AC_DEFUN([KAFFE_LIB_SOLARIS_PTHREAD],

[
	oldLIBS="$LIBS"
	LIBS=
	AC_SEARCH_LIBS([sem_init],[rt posix4],[PTHREAD_LIBS="$PTHREAD_LIBS $LIBS"])
	LIBS="$oldLIBS"
	case $Khost_os in
		solaris*)
			CPPFLAGS="$CPPFLAGS -D_POSIX_PTHREAD_SEMANTICS"
			;;
	esac
])


