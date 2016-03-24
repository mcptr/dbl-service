#!/bin/sh

LIB_DEST=$(readlink -f ../../lib)
BIN_DEST=$(readlink -f ../../bin)

echo "Installing libraries in $LIB_DEST"
cp -pv build/dblclient/libdblclient* $LIB_DEST
echo "Installing binary to $BIN_DEST"
cp -pv bin/dnsblocker $BIN_DEST
