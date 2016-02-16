#!/bin/sh

ME=$(readlink -f $0)
MY_DIR=$(dirname $ME)
EXTERNDIR=$(readlink -f "$MY_DIR/../extern")
ROOTDIR=$(readlink -f "$EXTERNDIR/root")
INCLUDEDIR="$ROOTDIR/include"

mkdir -p $INCLUDEDIR
cp $EXTERNDIR/repos/easyloggingcpp/src/easylogging++.h $INCLUDEDIR
