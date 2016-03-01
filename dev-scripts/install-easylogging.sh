#!/bin/sh

if test ! -d $PROJECT_ROOT; then
    echo "Need PROJECT_ROOT env"
    exit 1
fi

ME=$(readlink -f $0)
MY_DIR=$(dirname $ME)
EXTERNDIR=$(readlink -f "$MY_DIR/../extern")
INCLUDEDIR="$PROJECT_ROOT/include"

mkdir -p $INCLUDEDIR
cp $EXTERNDIR/repos/easyloggingcpp/src/easylogging++.h $INCLUDEDIR
