#!/bin/sh

./bin/adblocker-service \
    -v \
    -f \
    --basedir rootdir/etc/dnsblocker/ \
    --db rootdir/var/db/local.db \
    --dns-proxy-generate-config \
    --templates-dir templates

