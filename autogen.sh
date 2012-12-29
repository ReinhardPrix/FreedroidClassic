#! /bin/sh
if (aclocal); then
	if (autoheader); then
		if (automake --add-missing --copy); then
			if (autoconf); then
				echo "You are now ready to run './configure'"
			else
				echo "Something failed, please make sure you have autoconf installed."
			fi
		else
			echo "Something failed, please make sure you have automake installed."
		fi
	else
		echo "Something failed, please make sure you have autotools installed."
	fi
else
	echo "Something failed, please make sure you have automake installed."
fi
