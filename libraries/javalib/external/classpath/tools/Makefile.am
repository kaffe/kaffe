## Input file for automake to generate the Makefile.in used by configure

GLIBJ_CLASSPATH='$(top_builddir)/lib':'$(top_builddir)/lib/glibj.zip':asm

# Setup the compiler to use the GNU Classpath library we just build
if FOUND_GCJ
JCOMPILER = $(GCJ) -encoding UTF-8 --bootclasspath $(GLIBJ_CLASSPATH) --classpath . -C
else
if FOUND_JIKES
JCOMPILER = $(JIKES) $(JIKESENCODING) -bootclasspath $(GLIBJ_CLASSPATH) -classpath $(GLIBJ_CLASSPATH):.
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

if CREATE_WRAPPERS
bin_SCRIPTS =
bin_PROGRAMS = gappletviewer gjarsigner gkeytool \
	gjar gnative2ascii gserialver gjavah grmiregistry \
	gtnameserv gorbd grmid grmic

AM_CPPFLAGS = -Wall \
	-I$(top_srcdir)/include \
	-DLIBJVM="\"$(libdir)/libjvm\"" \
	-DTOOLS_ZIP="\"$(TOOLSdir)/$(TOOLS_ZIP)\""

gappletviewer_SOURCES = toolwrapper.c
gappletviewer_CFLAGS = \
	-DTOOLPACKAGE="\"appletviewer\"" \
	-DTOOLNAME="\"gappletviewer\""

gjarsigner_SOURCES = toolwrapper.c
gjarsigner_CFLAGS = \
	-DTOOLPACKAGE="\"jarsigner\"" \
	-DTOOLNAME="\"gjarsigner\""

gkeytool_SOURCES = toolwrapper.c
gkeytool_CFLAGS = \
	-DTOOLPACKAGE="\"keytool\"" \
	-DTOOLNAME="\"gkeytool\""

gjar_SOURCES = toolwrapper.c
gjar_CFLAGS = \
	-DTOOLPACKAGE="\"jar\"" \
	-DTOOLNAME="\"gjar\""

gnative2ascii_SOURCES = toolwrapper.c
gnative2ascii_CFLAGS = \
	-DTOOLPACKAGE="\"native2ascii\"" \
	-DTOOLNAME="\"gnative2ascii\""

gserialver_SOURCES = toolwrapper.c
gserialver_CFLAGS = \
	-DTOOLPACKAGE="\"serialver\"" \
	-DTOOLNAME="\"gserialver\""

grmiregistry_SOURCES = toolwrapper.c
grmiregistry_CFLAGS = \
	-DTOOLPACKAGE="\"rmiregistry\"" \
	-DTOOLNAME="\"grmiregistry\""

gtnameserv_SOURCES = toolwrapper.c
gtnameserv_CFLAGS = \
	-DTOOLPACKAGE="\"tnameserv\"" \
	-DTOOLNAME="\"gtnameserv\""

gorbd_SOURCES = toolwrapper.c
gorbd_CFLAGS = \
	-DTOOLPACKAGE="\"orbd\"" \
	-DTOOLNAME="\"gorbd\""

grmid_SOURCES = toolwrapper.c
grmid_CFLAGS = \
	-DTOOLPACKAGE="\"rmid\"" \
	-DTOOLNAME="\"grmid\""

gjavah_SOURCES = toolwrapper.c
gjavah_CFLAGS = \
	-DTOOLPACKAGE="\"javah\"" \
	-DTOOLNAME="\"gjavah\""

grmic_SOURCES = toolwrapper.c
grmic_CFLAGS = \
	-DTOOLPACKAGE="\"rmic\"" \
	-DTOOLNAME="\"grmic\""

else
bin_SCRIPTS = gappletviewer gjarsigner gkeytool \
	gjar gnative2ascii gserialver gjavah grmiregistry \
	gtnameserv gorbd grmid grmic
bin_PROGRAMS =
endif
EXTRA_DIST = toolwrapper.c gappletviewer.in gjarsigner.in gkeytool.in \
	gjar.in gnative2ascii.in gserialver.in gjavah.in grmiregistry.in \
	gtnameserv.in gorbd.in grmid.in grmic.in

# All our example java source files
TOOLS_JAVA_FILES = $(srcdir)/gnu/classpath/tools/*/*.java \
	$(srcdir)/external/asm/org/objectweb/asm/*.java \
	$(srcdir)/external/asm/org/objectweb/asm/attrs/*.java \
	$(srcdir)/external/asm/org/objectweb/asm/commons/*.java \
	$(srcdir)/external/asm/org/objectweb/asm/optimizer/*.java \
	$(srcdir)/external/asm/org/objectweb/asm/signature/*.java \
	$(srcdir)/external/asm/org/objectweb/asm/tree/*.java \
	$(srcdir)/external/asm/org/objectweb/asm/tree/analysis/*.java \
	$(srcdir)/external/asm/org/objectweb/asm/util/*.java \
	$(srcdir)/external/asm/org/objectweb/asm/util/attrs/*.java \
	$(srcdir)/external/asm/org/objectweb/asm/xml/*.java

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
	@rm -rf classes asm
	mkdir classes asm
## Compile ASM separately as it is latin-1 encoded.
	find $(srcdir)/external/asm -name '*.java' -print > asm.lst
	AC=`echo $(JCOMPILER) | sed -e 's/UTF-8/ISO-8859-1/g'`; \
	  $$AC -d asm @asm.lst
	find $(srcdir)/gnu/classpath/tools -name '*.java' -print > classes.lst
	$(JCOMPILER) -d classes @classes.lst
## Copy over tools resource files.
	@list=`cd $(top_srcdir)/resource && find gnu/classpath/tools -name \*.properties -print`; \
	for p in $$list; do \
	  dirname=classes/`dirname $$p`; \
	  if ! test -d "$$dirname"; then mkdir -p "$$dirname"; fi; \
	  echo "  cp $(top_srcdir)/resource/$$p classes/$$p"; \
	  cp $(top_srcdir)/resource/$$p classes/$$p; \
	done
## First add classpath tools stuff.
	(cd classes; \
	if test "$(ZIP)" != ""; then $(ZIP) -r ../$(TOOLS_ZIP) .; fi; \
	if test "$(FASTJAR)" != ""; then $(FASTJAR) cf ../$(TOOLS_ZIP) .; fi; \
	cd ..)
## Now add ASM classes.
	(cd asm; \
	if test "$(ZIP)" != ""; then $(ZIP) -u -r ../$(TOOLS_ZIP) .; fi; \
	if test "$(FASTJAR)" != ""; then $(FASTJAR) uf ../$(TOOLS_ZIP) .; fi; \
	cd ..)
	rm -rf classes classes.lst

# Zip file be gone! (and make sure the classes are gone too)
clean-local:
	rm -rf $(TOOLS_ZIP) classes

# FIXME: remove this when GNU Classpath includes a bootstrap VM.
installcheck-binSCRIPTS:
	:
