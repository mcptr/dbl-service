#!/bin/sh

# NOTE: You need sqlite development files, e.g. on ubuntu:
# libsqlite3-dev

ME=$(readlink -f $0)
MY_DIR=$(dirname $ME)
EXTERNDIR=$(readlink -f "$MY_DIR/../extern")
ROOTDIR=$(readlink -f "$EXTERNDIR/root")
BUILD_DIR="$ROOTDIR/tmp/soci-build"

mkdir -p $BUILD_DIR
cd $BUILD_DIR

cmake -DCMAKE_INSTALL_PREFIX=$ROOTDIR -G "Unix Makefiles" \
    -DWITH_BOOST=ON \
    -DWITH_SQLITE3=ON \
    -DSOCI_CXX_C11=ON \
    $EXTERNDIR/repos/soci

make -j4
make install
make clean
rm -rf $BUILD_DIR
