#!/bin/sh

if test ! -d $VIRTUAL_ENV; then
    echo "Need VIRTUAL_ENV env"
    exit 1
fi

ME=$(readlink -f $0)
MY_DIR=$(dirname $ME)
EXTERNDIR=$(readlink -f "$MY_DIR/../extern")
INCLUDEDIR="$VIRTUAL_ENV/include"
LIBDIR="$VIRTUAL_ENV/lib"

rm -rf $INCLUDEDIR/cxx-test-util
cwd=`pwd`
cd $EXTERNDIR/repos/cxx-test-util/
make
cp -p $EXTERNDIR/repos/cxx-test-util/build/target/libcxxtestutil.so $LIBDIR
cp -R $EXTERNDIR/repos/cxx-test-util/src/include $INCLUDEDIR/cxx-test-util

