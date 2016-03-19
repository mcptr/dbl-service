#!/bin/sh

if test ! -d $VIRTUAL_ENV; then
    echo "Need VIRTUAL_ENV env"
    exit 1
fi

ME=$(readlink -f $0)
MY_DIR=$(dirname $ME)
EXTERNDIR=$(readlink -f "$MY_DIR/../extern")
INCLUDEDIR="$VIRTUAL_ENV/include"

mkdir -p $INCLUDEDIR
cp -v $EXTERNDIR/repos/easyloggingcpp/src/easylogging++.h $INCLUDEDIR
