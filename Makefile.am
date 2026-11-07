SUBDIRS = src map graphics sound

man_MANS = freedroid.6

EXTRA_DIST = $(man_MANS) mac-osx

DEBUG_SDL_SOURCE ?= $(abs_top_srcdir)/../SDL3-3.4.2
DEBUG_SDL_BUILD = $(abs_top_builddir)/.local/sdl3-debug-build
DEBUG_SDL_PREFIX = $(abs_top_builddir)/.local/sdl3-debug

.PHONY: debug strict

debug:
	cmake -S "$(DEBUG_SDL_SOURCE)" -B "$(DEBUG_SDL_BUILD)" \
		-DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_FLAGS=-DSDL_TRACK_ALLOCATION_COUNT \
		-DCMAKE_INSTALL_PREFIX="$(DEBUG_SDL_PREFIX)" -DCMAKE_INSTALL_LIBDIR=lib \
		-DSDL_SHARED=ON -DSDL_STATIC=OFF -DSDL_TESTS=OFF -DSDL_TEST_LIBRARY=OFF
	+cmake --build "$(DEBUG_SDL_BUILD)"
	cmake --install "$(DEBUG_SDL_BUILD)"
	$(MAKE) -C src freedroidDebug \
		DEBUG_SDL_LDFLAGS="-L$(DEBUG_SDL_PREFIX)/lib -Wl,-rpath,$(DEBUG_SDL_PREFIX)/lib" \
		DEBUG_SDL_LIBRARY="$(DEBUG_SDL_PREFIX)/lib/libSDL3.so"

strict:
	$(MAKE) WERROR_CFLAGS=-Werror all

## Win32 specific stuff follows here

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


android-data:
	mkdir -p AndroidData
	zip -r AndroidData/data-1.2.2.zip graphics/ sound/*.wav sound/*.ogg map/
	cp graphics/title.jpg AndroidData/logo.jpg
