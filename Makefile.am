SUBDIRS = src 

man_MANS = freedroidRPG.6

freedroid_data = map graphics sound dialogs
EXTRA_DIST = $(man_MANS) $(freedroid_data)

dist-hook:
	rm -rf `find $(distdir) -name CVS`
	rm -f  `find $(distdir) -name "*.blend"`

install-data-local:
	echo "Installing the data-files ..."
	$(mkinstalldirs) $(DESTDIR)$(pkgdatadir)
	cp -pr $(srcdir)/$(freedroid_data) $(DESTDIR)$(pkgdatadir)/
	echo "... and cleaning CVS and blender-sources etc:"
	rm -rf `find $(DESTDIR)$(pkgdatadir) -name CVS`
	rm -f  `find $(DESTDIR)$(pkgdatadir) -name "*.blend"`
	rm -f  `find $(DESTDIR)$(pkgdatadir) -name "Makefile*"`
	echo "..done."

uninstall-local:
	echo "Removing data-files ..."
	rm -rf $(DESTDIR)$(pkgdatadir)
	echo "...done."