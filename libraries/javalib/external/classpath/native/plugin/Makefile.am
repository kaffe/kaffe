lib_LTLIBRARIES = libgcjwebplugin.la

libgcjwebplugin_la_SOURCES = gcjwebplugin.cc

libgcjwebplugin_la_CXXFLAGS = \
	-Wall -DAPPLETVIEWER_EXECUTABLE="\"$(bindir)/appletviewer\"" \
	$(MOZILLA_CFLAGS) $(GLIB_CFLAGS)

libgcjwebplugin_la_LDFLAGS = -avoid-version \
	$(GLIB_LIBS) \
	-lstdc++

install-libLTLIBRARIES: $(lib_LTLIBRARIES)
	$(INSTALL) -d -m0755 $(DESTDIR)@PLUGIN_DIR@
	$(INSTALL) .libs/libgcjwebplugin.so $(DESTDIR)@PLUGIN_DIR@

uninstall-libLTLIBRARIES:
	rm -f $(DESTDIR)@PLUGIN_DIR@/libgcjwebplugin.so
