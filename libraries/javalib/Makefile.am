# Only build the peerless AWT libraries if kaffe is configured to
# build the native libraries as well.
if COND_KAFFE_QT_AWT
MAYBE_PEERLESS_KAFFE_AWT=awt-implementations
else
if COND_KAFFE_X_AWT
MAYBE_PEERLESS_KAFFE_AWT=awt-implementations
else
if COND_KAFFE_NANOX_AWT
MAYBE_PEERLESS_KAFFE_AWT=awt-implementations
else
if COND_KAFFE_XYNTH_AWT
MAYBE_PEERLESS_KAFFE_AWT=awt-implementations
endif
endif
endif
endif

# Only build gmp java.math libraries if kaffe is configured
# to build the native libraries as well.
if COND_PURE_JAVA_MATH
else
MAYBE_GMP_JAVA_MATH=gmp-math
endif

if COND_NATIVE_SOUND
  MAYBE_TRITONUS_SOUND=tritonus-sound
endif

if COND_ZLIB_ZIP
  MAYBE_ZLIB_ZIP=zlib-zip
endif

if USE_PRECOMPILED_RT_JAR
stamp-glibj:
	@echo Importing glibj.zip from $(GLIBJ_ZIP)
	@cp $(GLIBJ_ZIP) external/classpath/lib/glibj.zip
	@$(LN_S) external/classpath/lib/glibj.zip glibj.zip
	@touch stamp-glibj

MAYBE_GLIBJ_ZIP= glibj.zip

glibj.zip: stamp-glibj
else
stamp-glibj:
	@touch stamp-glibj
endif

clean-local:
	rm -rf stamp-glibj glibj.zip

SUBDIRS= \
	. \
	external/classpath \
	vmspecific \
	gnu-inet \
	dnsjava \
	$(MAYBE_PEERLESS_KAFFE_AWT) \
	$(MAYBE_GMP_JAVA_MATH) \
	$(MAYBE_TRITONUS_SOUND) \
	$(MAYBE_ZLIB_ZIP)

jrelib_DATA= \
	     logging.properties \
	     $(MAYBE_GLIBJ_ZIP)

export JAVA_COMPILER

install-data-local: installdirs
	if test -f $(DESTDIR)${jrelibdir}/glibj.zip ; then \
		if ! test -f $(DESTDIR)${jrelibdir}/rt.jar ; then \
			$(LN_S) $(DESTDIR)${jrelibdir}/glibj.zip $(DESTDIR)${jrelibdir}/rt.jar ; \
		fi ; \
	fi

uninstall-local: 
	$(RM) $(DESTDIR)${jrelibdir}/rt.jar

EXTRA_DIST= \
	    logging.properties 
