# Only build the peerless AWT libraries if kaffe is configured to
# build the native libraries as well.

if COND_ZLIB_ZIP
  MAYBE_ZLIB_ZIP=zlib-zip
endif

SUBDIRS= \
	vmspecific \
	$(MAYBE_ZLIB_ZIP)

install-data-local: installdirs
	$(LN_S) -f $(TARGET_CLASSPATH_CLASSES) $(DESTDIR)${jrelibdir}/rt.jar

uninstall-local:
	$(RM) $(DESTDIR)${jrelibdir}/rt.jar
