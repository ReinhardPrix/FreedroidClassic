if WIN32
SUBDIRS = win32 src 
else
SUBDIRS = win32 src croppy gluem
endif

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



## Win32 specific stuff follows here

win_txts = COPYING AUTHORS README Releasetext freedroidRPG-man
win_dir = freedroidRPG-$(VERSION)-win32
dist-win32: src/freedroidRPG.exe
	mkdir -p $(win_dir)
	cp -r graphics map sound dialogs $(win_dir)
	cp src/freedroidRPG.exe $(win_dir)
	nroff -man freedroidRPG.6 > freedroidRPG-man
	for i in $(win_txts); do \
	   cp $$i $(win_dir)/$$i.txt; \
	   emacs -batch -q -l unix2dos.el --visit=$(win_dir)/$$i.txt -f unix2dos;\
	done
	zip -r $(win_dir).zip $(win_dir)

