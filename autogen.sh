#! /bin/sh
aclocal
autoheader
automake --add-missing
autoconf
##./configure
echo "You are now ready to run './configure'"


