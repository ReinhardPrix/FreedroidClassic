#! /bin/sh
if (aclocal -I m4 && autoheader && automake --add-missing && autoconf); then
    echo "You are ready to run ./configure now!"
else
    echo "Something failed, no valid configure file could be created"
fi




