
echo "This is the helper program for installation of FreedroidRPG"
echo "using the 'minimal download' package available from the project"
echo "download page."
echo " "
echo "This program will create the symlinks nescessary, that will make"
echo "the program run even without some portrait images, which means"
echo "no loss in playablility of the game."
echo " "
echo "Once that is done, the helper program will start the creation"
echo "of all the Tux images nescessary for the game.  These images"
echo "are quite numerous and might take several hours of full CPU"
echo "power to completely render.  Also, this requires, that the 'Blender'"
echo "program be installed on your machine.  Blender is a free (GPL)"
echo "3D design application.  You can get it from www.elysiun.org or just"
echo "google for it.  Version 2.27 of Blender is known to work best."
echo "Newer versions might not work as good due to changes to the python"
echo "script handling."
echo " "
echo "So now it's time to start the work..."
echo " "
echo "Creating all symlinks nescessary for the reduced package..."
echo " "
cd graphics/droids
for i in $( ls -d ??? ); do
    echo "Now entering directory" $i
    cd $i

    for j in $( ls ../../items/NONE_AVAILABLE_YET/portrai* ); do
	ln -s $j .
    done
    echo "All done in " $i ". Leaving directory " $i "..."
    cd ..
done
cd ../..
echo " "
echo "Creation of symlinks in droid dirs now complete."
echo " "
echo "Starting to create symlinks in items dirs..."
echo " "
cd graphics/items
for i in $( ls -d * ); do
    echo "Now entering directory" $i
    cd $i

    for j in $( ls ../NONE_AVAILABLE_YET/portrai* ); do
	ln -s $j .
    done
    echo "All done in " $i ". Leaving directory " $i "..."
    cd ..
done
cd ../..
echo " "
echo "Creation of all symlinks now complete."
echo "Now we come to the hardest part:  Creating the Tux motion images."
echo " "
echo "Starting to render the Tux motion images.  As mentioned above, this"
echo "can take quite a lot of time (maybe over night) and also requires"
echo "Blender 2.27 to be installed on your system.  Also, you should have"
echo "done the usual './configure' and 'make' commands already, because"
echo "the Tux motion image generation requires the 'croppy' tool from inside"
echo "this package to be installed already.  If you successfully did './configure'"
echo "and 'make', then this is the case."
echo " "
echo "So, I must ask here, is it correct, that Blender 2.27 (or similar?)"
echo "is installed in your system and in the path, so that it can be found"
echo "and have to done './configure' and 'make'"
echo "and are you ready for a lengthly session of image creation now or shall"
echo "we abort and you can run this script later when everything is prepared."
echo "(There will be no problems from running this script a second time.)"
echo " "
echo "So please enter '1' to proceed or '2' to abort the script for now."
echo " "
OPTIONS="Proceed Abort"
select opt in $OPTIONS; do
    if [ "$opt" = "Abort" ]; then
	echo "OK.  Aborted."
	exit
    elif [ "$opt" = "Proceed" ]; then
	echo "OK.  Starting image creation..."
	echo " "
	cd graphics/blender_sources
	blender -b tux_sword_motion.blend -a
	blender -b tux_gun_motion.blend -a
	cd ../..

	echo " "
	echo "Tux image creation now complete.  Everything should be ready to run."
	echo " "
	echo "To start the game, best go to the 'src' directory no and type"
	echo "'freedroidRPG' there.  You might also choose to 'make install' as root"
	echo "now, but this is not absolutely nescessary, if you run it from inside src dir."
	echo " "
	echo "In case of problems, best send mail to freedroid-discussion@lists.sourceforge.net."
	echo " "
	echo "Have fun."
	echo " " 
	exit
    else
	clear
	echo "Bad option.  Please enter 'proceed' or 'abort'."
    fi
done


