#!/bin/sh

./bin/dnsblocker \
    -v \
    -f \
    --dns-proxy=unbound \
    --dns-proxy-generate-config \
    $@
