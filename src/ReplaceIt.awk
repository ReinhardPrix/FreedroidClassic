#
# ATTENTION!! This is my fourth AWK file.  It may contain mistakes....
# 
# Lets see if this works...
#
BEGIN { print "WARNING!  Starting Replacement! Please pay attention! " }
! /Rahmen/ { print $0 >> FILENAME ".modif" }
/Rahmen/ { print "Line with target string encountered..."
	     print "It looks like this:" 
	     print $0 
             print "Now I'll do something to it, hehehe...."
             gsub(/Rahmen/ , "Banner" )
	     print "Now the line looks like this: "
             print $0 
	     print "I hope that matches our needs..." 
	     print "Now I add it to our NewHomepage..."
	     print $0 >> FILENAME ".modif" }
END { print " DONE.  File should have been processed.  Please check for correct output! " }
