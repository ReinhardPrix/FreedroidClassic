#!/bin/bash
#
# This file should be able to crop apart the map_blocks.png file into many single
# little files, each of size matching exactly on map tile.
#

#--------------------
# At first we delete all the existing tux motion files,
# so that there will be no questions later when renaming
# and no old dead files...
#
rm tux_motion_*.png

#--------------------
# Now we start to process each line of map blocks...
#
for i in `seq 0 10`;
  do
  echo "Now Processing row nr." $i

  for j in `seq 0 14`;
    do

    #--------------------
    # Now we can determine the new file name and make a copy of
    # the original file that will be cropped to the right chunk right
    # afterwards
    #
    OF=./tux_motion_"$i"_"$j".png
    echo "New file name will be: " $OF
    cp tux_motions.png "$OF"

    #--------------------
    # Now we can start to do the actual cropping...
    # This will be done via the mogrify and the crop command in there..
    #
    OffsetX=$(echo "$j*(130+2)" | bc -l)
    OffsetY=$(echo "$i*(130+2)" | bc -l)

    echo OffsetX = $OffsetX " and " OffsetY = $OffsetY

    mogrify -crop 130x130+$OffsetX+$OffsetY $OF

  done


done    

#--------------------
# Now we do some correction, so that the file naming is a bit more conformant with
# what the blender likes to produce...
#
mmv "tux_motion_??_?.png" "tux_motion_#1#2_0#3.png"
mmv "tux_motion_?_??.png" "tux_motion_0#1_#2#3.png"
mmv "tux_motion_?_?.png" "tux_motion_0#1_0#2.png"
