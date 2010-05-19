#! /bin/sh
aclocal
autoheader
automake --add-missing --copy
autoconf
##./configure
echo "You are now ready to run './configure'"


