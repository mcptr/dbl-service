#!/bin/sh

./bin/dnsblocker \
    -v \
    -f \
    --no-system-dns-proxy \
    --basedir etc/dnsblocker \
    --db var/db/local.db \
    --pidfile var/run/dnsblocker.pid \
    --dns-proxy-generate-config \
    --dns-proxy-config-destdir var/run/ \
    --dns-proxy-pidfile var/run/dns-proxy-pidfile.pid \
    --dns-proxy-port 11053 \
    --dns-proxy-root-key etc/dnsblocker/root.key \
    --templates etc/dnsblocker/templates \
    $@

