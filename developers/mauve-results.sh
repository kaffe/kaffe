#! /bin/sh
# generate mauve result page
# needs MAUVE_SRC as mauve source directory and KAFFE_SRC as kaffe source directory
# needs path to jtidy as JTIDY_CPATH
cd $MAUVE_SRC
./configure JAVA=kaffe JAVAC=kjc
cp $KAFFE_SRC/developers/mauve-kaffe .
make check KEYS=kaffe TESTFLAGS=-verbose > kaffe-log
perl $KAFFE_SRC/developers/mauve-html-gen.pl "for kaffe from CVS HEAD" "kaffe on i686-linux using jit3 engine and unix-jthreads implementation. Tests are run with developers/mauve-kaffe file from CVS HEAD" < kaffe-log > mauve-results.html
kaffe -cp JTIDY_CPATH  org/w3c/tidy/Tidy -imu mauve-results.html
