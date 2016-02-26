ME=$(readlink -f $0)
MY_DIR=$(dirname $ME)
EXTERNDIR=$(readlink -f "$MY_DIR/../extern")
ROOTDIR=$(readlink -f "$EXTERNDIR/root")
BUILD_DIR="$ROOTDIR/tmp/jsoncpp-build"


mkdir -p $BUILD_DIR
cd $BUILD_DIR

#-DCMAKE_BUILD_TYPE=debug \

cmake  -G "Unix Makefiles" \
    -DBUILD_STATIC_LIBS=OFF \
    -DBUILD_SHARED_LIBS=ON \
    -DCMAKE_INSTALL_PREFIX=$ROOTDIR \
    $EXTERNDIR/repos/jsoncpp

make -j4
make install
