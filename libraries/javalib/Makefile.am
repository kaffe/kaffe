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

if COND_ZLIB_ZIP
  MAYBE_ZLIB_ZIP=zlib-zip
endif

SUBDIRS= \
	vmspecific \
	$(MAYBE_PEERLESS_KAFFE_AWT) \
	$(MAYBE_GMP_JAVA_MATH) \
	$(MAYBE_ZLIB_ZIP)

jrelib_DATA= \
	     logging.properties

install-data-local: installdirs
	if ! test -f $(DESTDIR)${jrelibdir}/rt.jar ; then \
		$(LN_S) $(CLASSPATH_CLASSES) $(DESTDIR)${jrelibdir}/rt.jar ; \
	fi

uninstall-local:
	$(RM) $(DESTDIR)${jrelibdir}/rt.jar

EXTRA_DIST= \
	    logging.properties 
