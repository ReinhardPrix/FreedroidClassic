#!/bin/bash

### Change to where you have installed the toolchain:
GCW0TOOLCHAINROOT="/opt/gcw0-toolchain/"

### These should not be changed for the default automatic cretion of an OPK...
GCW0WORKDIR="`pwd`"/"GCWZeroInstall"
export DESTDIR="${GCW0WORKDIR}"/opkroot
FREEDROIDSQUASHROOT="$DESTDIR"/mnt/freedroid
FREEDROIDDESKTOPENTRY=`pwd`/gcw0/default.gcw0.desktop
FREEDROIDGCW0MISSIONFILE=`pwd`/gcw0/Paradroid.mission
FREEDROIDMANUAL=`pwd`/gcw0/FreeDroid-gcw0-manualandlicense.txt
FREEDROIDICON=`pwd`/gcw0/paraicon.png
FREEDROIDOPK="${GCW0WORKDIR}"/freedroid.opk

OLD=I/usr/include
NEW=I/opt/gcw0-toolchain/usr/mipsel-gcw0-linux-uclibc/sysroot/usr/include

./autogen.sh

[[ ! -e "$DESTDIR" ]] && mkdir -p "$DESTDIR"

if [ "$1" != noconf ] ; then
./configure --includedir=${GCW0TOOLCHAINROOT}/usr/mipsel-gcw0-linux-uclibc/sysroot/usr/include/ \
            --oldincludedir=${GCW0TOOLCHAINROOT}/usr/mipsel-gcw0-linux-uclibc/sysroot/usr/include/ \
            --with-sdl-prefix=${GCW0TOOLCHAINROOT}/usr/mipsel-gcw0-linux-uclibc/sysroot/usr \
            --host=mipsel-linux \
            --prefix=/mnt/freedroid \
            CFLAGS="-O2 -G0 -march=mips32 -mtune=mips32 -pipe -fomit-frame-pointer -fexpensive-optimizations -frename-registers"
fi

### this could probably be made automatic... not sure how!
echo "#define GCW0" >> config.h

#             CXXFLAGS="-O2 -G0 -march=mips32 -mtune=mips32 -pipe -fomit-frame-pointer -fexpensive-optimizations -frename-registers"\

sed -i "s|"$OLD"|"$NEW"|g" Makefile
sed -i "s|"$OLD"|"$NEW"|g" src/Makefile


make && make install && ( 
  
  echo "****************************************************************************"
  echo "** FreeDroid for GCW-0 compiled and installed into:                       **"
  echo "** `printf %-71s $DESTDIR`**"
  echo "** Now copying some files for the .opk...                                 **"
  echo "****************************************************************************"
  echo

  [[ -e "$FREEDROIDDESKTOPENTRY" ]] && cp "$FREEDROIDDESKTOPENTRY" "$FREEDROIDSQUASHROOT" || NODESKTOP=1 
  [[ -e "$FREEDROIDICON" ]] && cp "$FREEDROIDICON" "$FREEDROIDSQUASHROOT"/share/freedroid/graphics/ || NOICON=1
  cp "${FREEDROIDGCW0MISSIONFILE}" "$FREEDROIDSQUASHROOT"/share/freedroid/map/Paradroid.mission
  cp "${FREEDROIDMANUAL}" "$FREEDROIDSQUASHROOT"/.

  echo "****************************************************************************"
  echo "* Now making .opk...                                                       *"
  echo "****************************************************************************"

  [[ -e "${FREEDROIDOPK}" ]] && mv "${FREEDROIDOPK}" "${FREEDROIDOPK}".old 
  [[ `which mksquashfs` ]] &&  mksquashfs "$FREEDROIDSQUASHROOT" ${FREEDROIDOPK} || NOMKSQUASFS=1

  echo
  echo "****************************************************************************"
  [[ $NODESKTOP || $NOIMAGEMAGICK || $NOICON || $NOSQUASHFS ]] && echo "** ERRORS:                                                              **"

  if [ $NODESKTOP ] ; then 
        echo "**** Could not find .desktop File! Resultin .opk is unusable!             **" ; 
  fi
  [[ $NOICON ]] && echo " **** Could not create Icon file!                                         **"
  [[ $NOSQUASHFS ]] && echo " **** Could not find mksquashfs, no .opk was created                      **"
  echo "** All Done!                                                              **"
  echo "****************************************************************************"
) || ( echo ; echo "make (install) failed!" )

