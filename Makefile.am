SUBDIRS = src 

man_MANS = freedroid.6

EXTRA_DIST = $(man_MANS) mac-osx map graphics sound

dist-hook:
	rm -rf `find $(distdir) -name CVS`

install-data-local:
	echo "Now copying the the data-directories..."
	$(mkinstalldirs) $(DESTDIR)$(pkgdatadir)
	cp -r $(srcdir)/$(EXTRA_DIST) $(DESTDIR)$(pkgdatadir)
	echo "... and cleaning from CVS dirs.."
	rm -rf `find $(DESTDIR)$(pkgdatadir) -name CVS`
	echo "..done."


bundle_contents = FreeDroid.app/Contents
FreeDroid_bundle: src/freedroid
	mkdir -p $(bundle_contents)/MacOS
	mkdir -p $(bundle_contents)/Resources
	echo "FreeDroid Classic " > $(bundle_contents)/PkgInfo
	$(INSTALL_PROGRAM) $< $(bundle_contents)/MacOS/
