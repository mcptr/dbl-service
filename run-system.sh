#!/bin/sh

./bin/dnsblocker \
    -v \
    --dns-proxy=unbound \
    --dns-proxy-generate-config \
    --service-port 0 \
    $@
