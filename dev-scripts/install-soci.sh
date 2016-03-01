#!/bin/sh

# NOTE: You need sqlite development files, e.g. on ubuntu:
# libsqlite3-dev

if test ! -d $PROJECT_ROOT; then
    echo "Need PROJECT_ROOT env"
    exit 1
fi

ME=$(readlink -f $0)
MY_DIR=$(dirname $ME)
EXTERNDIR=$(readlink -f "$MY_DIR/../extern")
BUILD_DIR="$ROOTDIR/tmp/soci-build"

mkdir -p $BUILD_DIR
cd $BUILD_DIR

cmake -DCMAKE_INSTALL_PREFIX=$PROJECT_ROOT -G "Unix Makefiles" \
    -DWITH_BOOST=ON \
    -DWITH_SQLITE3=ON \
    -DSOCI_CXX_C11=ON \
    $EXTERNDIR/repos/soci

make -j4
make install
make clean
rm -rf $BUILD_DIR
