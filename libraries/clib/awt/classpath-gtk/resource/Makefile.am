## used by automake to generate Makefile.in

EXTRA_DIST = \
		font.properties

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(libdir)/awt
	$(INSTALL_DATA) $(srcdir)/font.properties $(DESTDIR)$(libdir)/awt/font.properties

uninstall-local:
	rm -f $(DESTDIR)$(libdir)/awt/font.properties

