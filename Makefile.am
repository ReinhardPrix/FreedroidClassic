SUBDIRS = src map graphics sound

man_MANS = freedroid.6

EXTRA_DIST = autogen.sh $(man_MANS) mac-osx

dist-hook:
	rm -rf `find $(distdir)/mac-osx -name CVS`

bundle_contents = FreeDroid.app/Contents
FreeDroid_bundle: src/freedroid
	mkdir -p $(bundle_contents)/MacOS
	mkdir -p $(bundle_contents)/Resources
	echo "FreeDroid Classic " > $(bundle_contents)/PkgInfo
	$(INSTALL_PROGRAM) $< $(bundle_contents)/MacOS/
