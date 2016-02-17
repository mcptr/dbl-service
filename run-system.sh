#!/bin/sh

./bin/dnsblocker \
    -v \
    -f \
    --dns-proxy=unbound \
    $@



