#!/bin/sh

if test ! -d $VIRTUAL_ENV; then
    echo "Need VIRTUAL_ENV env"
    exit 1
fi

ME=$(readlink -f $0)
MY_DIR=$(dirname $ME)
EXTERNDIR=$(readlink -f "$MY_DIR/../extern")
BUILD_DIR="$VIRTUAL_ENV/tmp/jsoncpp-build"


mkdir -p $BUILD_DIR
cd $BUILD_DIR

#-DCMAKE_BUILD_TYPE=debug \

cmake  -G "Unix Makefiles" \
    -DBUILD_STATIC_LIBS=ON \
    -DBUILD_SHARED_LIBS=ON \
    -DCMAKE_INSTALL_PREFIX=$VIRTUAL_ENV \
    $EXTERNDIR/repos/jsoncpp

make -j4
make install
