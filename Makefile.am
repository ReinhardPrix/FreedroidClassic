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


## Win32 specific stuff follows here
## adjust this value to your system!!

win_txts = COPYING AUTHORS README Releasetext freedroid-man
win_dir = freedroid-$(VERSION)-win32
dist-win32: src/freedroid.exe
	mkdir -p $(win_dir)
	cp -r graphics map sound $(win_dir)
	cp src/freedroid.exe $(win_dir)
	nroff -man freedroid.6 > freedroid-man
	for i in $(win_txts); do \
	   cp $$i $(win_dir)/$$i.txt; \
	   emacs -batch -q -l unix2dos.el --visit=$(win_dir)/$$i.txt -f unix2dos;\
	done
	zip -r $(win_dir).zip $(win_dir)


## MacOS X stuff follows here (experimental and unfinished)

bundle_contents = FreeDroid.app/Contents
FreeDroid_bundle: src/freedroid
	mkdir -p $(bundle_contents)/MacOS
	mkdir -p $(bundle_contents)/Resources
	echo "FreeDroid Classic " > $(bundle_contents)/PkgInfo
	$(INSTALL_PROGRAM) $< $(bundle_contents)/MacOS/

