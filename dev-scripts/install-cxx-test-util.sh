#!/bin/sh

if test ! -d $VIRTUAL_ENV; then
    echo "Need VIRTUAL_ENV env"
    exit 1
fi

ME=$(readlink -f $0)
MY_DIR=$(dirname $ME)
EXTERNDIR=$(readlink -f "$MY_DIR/../extern")
INCLUDEDIR="$VIRTUAL_ENV/include"

mkdir -p $INCLUDEDIR/cxx-test-util
cp -R $EXTERNDIR/repos/cxx-test-util/tools/* $INCLUDEDIR/cxx-test-util

