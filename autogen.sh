#! /bin/sh
aclocal
autoheader
automake --add-missing
autoconf

##./configure
echo "You are ready to run ./configure now!"


