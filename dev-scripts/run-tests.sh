#!/bin/sh

ME=$(readlink -f $0)
MY_DIR=$(dirname $ME)

nosetests --nologcapture --logging-level DEBUG -v -s $MY_DIR/../test/
