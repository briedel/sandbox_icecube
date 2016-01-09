#!/bin/sh
set -e

# helper script to automatically download and extract safeprimes_base32.txt
# (it takes a long time to build this file..)
BASE_URL="http://icecube:skua@icecube.wisc.edu/~briedel/downloads/SNPS/spectra"
MD5_SUM_EXPECTED="622486f2ba1eada7c7a8a598f437ab6d"

FILENAME="supernovaspectra.h5"

# require exactly one argument
if [ $# -ne 1 ]; then
    echo "Usage: `basename $0` {resources_dir}"
    exit 1
fi
RESOURCES_DIR=$1

# if the file exists, exit immediately
if [ -f $RESOURCES_DIR/supernovaspectra.h5 ]; then
    # if possible check the md5 checksum to see if the file is ok
    # (if not, just assume it is ok and exit)
    
    FOUND_MD5=0
    command -v md5 >/dev/null && FOUND_MD5=1 || FOUND_MD5=0
    if [ $FOUND_MD5 -eq 1 ]; then
        MD5_SUM_FILE=`md5 -q $RESOURCES_DIR/supernovaspectra.h5 `
    else
        # exit if there is no md5 or md5sum tool available
        command -v md5sum >/dev/null || { exit 0; }
        md5=`md5sum $RESOURCES_DIR/supernovaspectra.h5 `
        MD5_SUM_FILE="${md5%% *}" # remove the first space and everything after it
    fi
    
    if [ "$MD5_SUM_FILE" != "$MD5_SUM_EXPECTED" ]; then
        echo "file $RESOURCES_DIR/supernovaspectra.h5  does exist, but checksum is not okay! removing and downloading again."
        echo "     found: $MD5_SUM_FILE"
        echo "  expected: $MD5_SUM_EXPECTED"
        
        rm $RESOURCES_DIR/supernovaspectra.h5 
    else
        # checksum is OK!
        exit 0
    fi
fi 


echo "trying to download $RESOURCES_DIR/supernovaspectra.h5 ..."

set +e
command -v curl >/dev/null && USE_CURL=1 || USE_CURL=0
if [ $USE_CURL -eq 0 ]; then
    # no curl, maybe wget?
    command -v wget >/dev/null || { echo >&2 "  this script requires either curl or wget to be installed on your system"; exit 2; }
    # USE_CURL=0 imples wget
fi
set -e

# download
echo "  downloading $BASE_URL/$FILENAME ..."
if [ $USE_CURL -eq 1 ]; then
    curl -L -o $RESOURCES_DIR/$FILENAME $BASE_URL/$FILENAME
else
    wget -O $RESOURCES_DIR/$FILENAME $BASE_URL/$FILENAME
fi

echo "  $RESOURCES_DIR/supernovaspectra.h5 downloaded."
