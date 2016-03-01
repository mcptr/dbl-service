#!/bin/sh

if test ! -d $PROJECT_ROOT; then
    echo "Need PROJECT_ROOT env"
    exit 1
fi

ME=$(readlink -f $0)
MY_DIR=$(dirname $ME)
EXTERNDIR=$(readlink -f "$MY_DIR/../extern")
BUILD_DIR="$PROJECT_ROOT/tmp/jsoncpp-build"


mkdir -p $BUILD_DIR
cd $BUILD_DIR

#-DCMAKE_BUILD_TYPE=debug \

cmake  -G "Unix Makefiles" \
    -DBUILD_STATIC_LIBS=OFF \
    -DBUILD_SHARED_LIBS=ON \
    -DCMAKE_INSTALL_PREFIX=$PROJECT_ROOT \
    $EXTERNDIR/repos/jsoncpp

make -j4
make install
