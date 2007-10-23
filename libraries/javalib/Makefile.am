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

stamp-glibj:
	@echo Importing glibj.zip from $(CLASSPATH_CLASSES)
	@$(LN_S) $(CLASSPATH_CLASSES) glibj.zip
	@touch stamp-glibj

glibj.zip: stamp-glibj

# used to link the system/precompiled glibj.zip on install
link-glibj-zip:
	if ! test -f $(DESTDIR)${jrelibdir}/glibj.zip ; then \
		$(LN_S) $(CLASSPATH_CLASSES) $(DESTDIR)${jrelibdir}/glibj.zip ; \
	fi

unlink-glibj-zip: 
	if test -f $(DESTDIR)${jrelibdir}/glibj.zip ; then \
		$(RM) $(DESTDIR)${jrelibdir}/glibj.zip ; \
	fi

install-exec-local: link-glibj-zip

clean-local:
	$(RM) stamp-glibj glibj.zip

SUBDIRS= \
	vmspecific \
	gnu-inet \
	dnsjava \
	$(MAYBE_PEERLESS_KAFFE_AWT) \
	$(MAYBE_GMP_JAVA_MATH) \
	$(MAYBE_TRITONUS_SOUND) \
	$(MAYBE_ZLIB_ZIP)

jrelib_DATA= \
	     logging.properties

install-data-local: installdirs
	if test -f $(DESTDIR)${jrelibdir}/glibj.zip ; then \
		if ! test -f $(DESTDIR)${jrelibdir}/rt.jar ; then \
			$(LN_S) $(DESTDIR)${jrelibdir}/glibj.zip $(DESTDIR)${jrelibdir}/rt.jar ; \
		fi ; \
	fi

uninstall-local: unlink-glibj-zip
	$(RM) $(DESTDIR)${jrelibdir}/rt.jar

EXTRA_DIST= \
	    logging.properties 
