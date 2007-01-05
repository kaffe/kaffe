## Input file for automake to generate the Makefile.in used by configure

GLIBJ_CLASSPATH='$(top_builddir)/lib':'$(top_builddir)/lib/glibj.zip':$(PATH_TO_ASM)

# Setup the compiler to use the GNU Classpath library we just build
if FOUND_GCJ
JCOMPILER = $(GCJ) -encoding UTF-8 --bootclasspath $(GLIBJ_CLASSPATH) --classpath . -C
else
if FOUND_JIKES
JCOMPILER = $(JIKES) $(JIKESENCODING) -bootclasspath '' -extdirs '' -sourcepath '' --classpath $(GLIBJ_CLASSPATH):.
else
if FOUND_GCJX
JCOMPILER = $(GCJX) -encoding UTF-8 -bootclasspath '' -sourcepath '' -classpath $(GLIBJ_CLASSPATH):.
else
if FOUND_ECJ
JCOMPILER = $(ECJ) -source 1.4 -encoding UTF-8 -bootclasspath $(GLIBJ_CLASSPATH) -classpath .
else
error dunno how to setup the JCOMPILER and compile
endif
endif
endif
endif

if USE_ASM
javah = gjavah
rmic = grmic
endif

if CREATE_WRAPPERS
bin_SCRIPTS =
bin_PROGRAMS = gappletviewer gjarsigner gkeytool \
	gjar gnative2ascii gserialver $(javah) grmiregistry \
	gtnameserv gorbd grmid $(rmic)

AM_CPPFLAGS = -Wall \
	-I$(top_srcdir)/include \
	-DLIBJVM="\"$(libdir)/libjvm\"" \
	-DTOOLS_ZIP="\"$(TOOLSdir)/$(TOOLS_ZIP)\""

gappletviewer_SOURCES = toolwrapper.c
gappletviewer_CFLAGS = \
	-DTOOLPACKAGE="\"appletviewer\"" \
	-DTOOLNAME="\"gappletviewer\"" \
	-DASM_JAR=""

gjarsigner_SOURCES = toolwrapper.c
gjarsigner_CFLAGS = \
	-DTOOLPACKAGE="\"jarsigner\"" \
	-DTOOLNAME="\"gjarsigner\"" \
	-DASM_JAR=""

gkeytool_SOURCES = toolwrapper.c
gkeytool_CFLAGS = \
	-DTOOLPACKAGE="\"keytool\"" \
	-DTOOLNAME="\"gkeytool\"" \
	-DASM_JAR=""

gjar_SOURCES = toolwrapper.c
gjar_CFLAGS = \
	-DTOOLPACKAGE="\"jar\"" \
	-DTOOLNAME="\"gjar\"" \
	-DASM_JAR=""

gnative2ascii_SOURCES = toolwrapper.c
gnative2ascii_CFLAGS = \
	-DTOOLPACKAGE="\"native2ascii\"" \
	-DTOOLNAME="\"gnative2ascii\"" \
	-DASM_JAR=""

gserialver_SOURCES = toolwrapper.c
gserialver_CFLAGS = \
	-DTOOLPACKAGE="\"serialver\"" \
	-DTOOLNAME="\"gserialver\"" \
	-DASM_JAR=""

grmiregistry_SOURCES = toolwrapper.c
grmiregistry_CFLAGS = \
	-DTOOLPACKAGE="\"rmiregistry\"" \
	-DTOOLNAME="\"grmiregistry\"" \
	-DASM_JAR=""

gtnameserv_SOURCES = toolwrapper.c
gtnameserv_CFLAGS = \
	-DTOOLPACKAGE="\"tnameserv\"" \
	-DTOOLNAME="\"gtnameserv\"" \
	-DASM_JAR=""

gorbd_SOURCES = toolwrapper.c
gorbd_CFLAGS = \
	-DTOOLPACKAGE="\"orbd\"" \
	-DTOOLNAME="\"gorbd\"" \
	-DASM_JAR=""

grmid_SOURCES = toolwrapper.c
grmid_CFLAGS = \
	-DTOOLPACKAGE="\"rmid\"" \
	-DTOOLNAME="\"grmid\"" \
	-DASM_JAR=""

if USE_ASM
gjavah_SOURCES = toolwrapper.c
gjavah_CFLAGS = \
	-DTOOLPACKAGE="\"javah\"" \
	-DTOOLNAME="\"gjavah\"" \
	-DASM_JAR="\":$(PATH_TO_ASM)\""

grmic_SOURCES = toolwrapper.c
grmic_CFLAGS = \
	-DTOOLPACKAGE="\"rmic\"" \
	-DTOOLNAME="\"grmic\"" \
	-DASM_JAR="\":$(PATH_TO_ASM)\""
endif

else
bin_SCRIPTS = gappletviewer gjarsigner gkeytool \
	gjar gnative2ascii gserialver $(javah) grmiregistry \
	gtnameserv gorbd grmid $(rmic)
bin_PROGRAMS =
endif
EXTRA_DIST = toolwrapper.c gappletviewer.in gjarsigner.in gkeytool.in \
	gjar.in gnative2ascii.in gserialver.in gjavah.in grmiregistry.in \
	gtnameserv.in gorbd.in grmid.in grmic.in

# All our example java source files
TOOLS_JAVA_FILES = $(srcdir)/gnu/classpath/tools/*/*.java

# The zip files with classes we want to produce.
TOOLS_ZIP = tools.zip

# Extra objects that will not exist until configure-time
BUILT_SOURCES = $(TOOLS_ZIP)

# RMIC templates that must be included in the generated zip file.
RMIC_TEMPLATES = $(srcdir)/gnu/classpath/tools/rmic/templates/*.jav

# All the files we find "interesting"
ALL_TOOLS_FILES = $(TOOLS_JAVA_FILES) $(RMIC_TEMPLATES)

# Some architecture independent data to be installed.
TOOLS_DATA = $(TOOLS_ZIP)

# Where we want these data files installed.
TOOLSdir = $(pkgdatadir)

# Make sure everything is included in the distribution.
dist-hook:
	srcdir_cnt=`echo $(srcdir) | wc -c`; \
	for file in $(ALL_TOOLS_FILES); do \
	  f=`echo $$file | cut -c$$srcdir_cnt-`; \
	  fdir=`dirname $$f`; \
	  if test ! -d $(distdir)/$$fdir; then \
	    echo "$(makeinstalldirs) $(distdir)/$$fdir"; \
	    $(mkinstalldirs) $(distdir)/$$fdir; \
	  fi; \
	  echo "cp -p $$file $(distdir)/$$f"; \
	  cp -p $$file $(distdir)/$$f; \
	done

# To generate the example zip just depend on the sources and ignore
# the class files. Always regenerate all .class files and remove them
# immediately.  And copy the template files we use to the classes dir
# so they get also included.
$(TOOLS_ZIP): $(TOOLS_JAVA_FILES)
	mkdir -p classes/gnu/classpath/tools/appletviewer
	mkdir -p classes/gnu/classpath/tools/common
	mkdir -p classes/gnu/classpath/tools/getopt
	mkdir -p classes/gnu/classpath/tools/jar
	mkdir -p classes/gnu/classpath/tools/jarsigner
	mkdir -p classes/gnu/classpath/tools/keytool
	mkdir -p classes/gnu/classpath/tools/native2ascii
	mkdir -p classes/gnu/classpath/tools/orbd
	mkdir -p classes/gnu/classpath/tools/rmid
	mkdir -p classes/gnu/classpath/tools/rmiregistry
	mkdir -p classes/gnu/classpath/tools/serialver
	mkdir -p classes/gnu/classpath/tools/tnameserv
	find $(srcdir)/gnu/classpath/tools -name '*.java' -print > classes.lst
	if test -z "$(PATH_TO_ASM)"; then \
	  mv classes.lst classes.tmp; \
	  fgrep -v /javah/ classes.tmp > classes.lst; \
	  mv classes.lst classes.tmp; \
	  fgrep -v /rmic/ classes.tmp > classes.lst; \
	  rm -f classes.tmp;\
	else \
	  mkdir -p classes/gnu/classpath/tools/javah; \
	  mkdir -p classes/gnu/classpath/tools/rmic/templates; \
	  cp $(RMIC_TEMPLATES) classes/gnu/classpath/tools/rmic/templates; \
	fi
	$(JCOMPILER) -d classes @classes.lst
	(cd classes; \
	if test "$(ZIP)" != ""; then $(ZIP) -r ../$(TOOLS_ZIP) .; fi; \
	if test "$(FASTJAR)" != ""; then $(FASTJAR) cf ../$(TOOLS_ZIP) .; fi; \
	cd ..)
	rm -rf classes classes.lst

# Zip file be gone! (and make sure the classes are gone too)
clean-local:
	rm -rf $(TOOLS_ZIP) classes

# FIXME: remove this when GNU Classpath includes a bootstrap VM.
installcheck-binSCRIPTS:
	:
