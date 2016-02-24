#!/bin/sh

DNS_PROXY=unbound
#DNS_PROXY_INCLUDE_DIR=/etc/dnsmasq.d
DNS_PROXY_INCLUDE_DIR=/etc/unbound/unbound.conf.d
PIDFILE=/var/run/dnsblocker/service.pid

start_server()
{
    ./bin/dnsblocker \
	-v \
	--dns-proxy=$DNS_PROXY \
	--dns-proxy-include-dir=$DNS_PROXY_INCLUDE_DIR \
	--dns-proxy-generate-config \
	--pidfile  $PIDFILE \
	--service-port 27654 \
	$@
}

stop_server()
{
    if [ -f $PIDFILE ]; then
	kill `cat $PIDFILE`
    fi
}

case "$1" in
    start)
	start_server $@
	;;
    stop)
	stop_server
	;;
    restart)
	stop_server
	start_server
	;;
    status)
	if [ -f $PIDFILE ]; then
	    PID=`cat $PIDFILE`
	    kill -0 $PID
	    if [ $? = 0 ]; then
		echo "Running (pid: $PID)"
		exit 0
	    else
		echo "Not running. You may remove $PIDFILE"
	    fi
	else
	    echo "Not running"
	    exit 1
	fi
	;;
    *)
	echo "Usage: $0 start|stop|restart|status"
	exit 1
esac
