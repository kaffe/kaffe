# I kicked out all the Antlr version checking. It requires a Java runtime
# to be installed, so it messes up bootstrapping. dalibor, feb 2005.
AC_DEFUN([AC_PROG_ANTLR],[
  AC_MSG_CHECKING([for antlr $1.$2.$3 or better])
  for antlr_lib_home in `ls -d /usr/local/share/antlr* 2> /dev/null` /usr/share/java; do
	if test -z $ANTLR_JAR; then
                      ANTLR_JAR=$antlr_lib_home/antlr.jar
	fi
  done
  test -z $ANTLR_JAR && \
        AC_MSG_ERROR(no suitable antlr.jar found for version $1.$2.$3)
  AC_MSG_RESULT([yes])
  AC_SUBST(ANTLR_JAR)
  AC_PROVIDE([$0])dnl
])
