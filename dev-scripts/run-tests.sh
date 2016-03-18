#!/bin/sh

ME=$(readlink -f $0)
MY_DIR=$(dirname $ME)
BIN_DIR=$MY_DIR/../bin

LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$MY_DIR/../lib"

nosetests --nologcapture --logging-level=DEBUG -v -s $MY_DIR/../test/

for cxx_test in $BIN_DIR/test*.bin; do
    echo "### CXX: $cxx_test";
    $cxx_test -v;
done
