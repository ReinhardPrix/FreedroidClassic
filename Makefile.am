SUBDIRS = src 

EXTRA_DIST = map graphics sound dialogs

dist-hook:
	rm -rf `find $(distdir) -name CVS`
	rm -f  `find $(distdir) -name "*.blend"`

install-data-local:
	echo "Now copying all the data-files recursively (by hand) ..."
	cp -r $(srcdir)/$(EXTRA_DIST) $(pkgdatadir)/
	echo "... and cleaning CVS and blender-sources:"
	rm -rf `find $(pkgdatadir) -name CVS`
	rm -f  `find $(pkgdatadir) -name "*.blend"`
	echo "..done."


