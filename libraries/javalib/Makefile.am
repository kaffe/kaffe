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

if COND_ZLIB_ZIP
  MAYBE_ZLIB_ZIP=zlib-zip
endif

SUBDIRS= \
	vmspecific \
	$(MAYBE_PEERLESS_KAFFE_AWT) \
	$(MAYBE_ZLIB_ZIP)

jrelib_DATA= \
	     logging.properties

install-data-local: installdirs
	$(LN_S) -f $(TARGET_CLASSPATH_CLASSES) $(DESTDIR)${jrelibdir}/rt.jar

uninstall-local:
	$(RM) $(DESTDIR)${jrelibdir}/rt.jar

EXTRA_DIST= \
	    logging.properties 
