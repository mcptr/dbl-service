#!/bin/sh

DNS_PROXY=unbound
#DNS_PROXY_INCLUDE_DIR=/etc/dnsmasq.d
DNS_PROXY_INCLUDE_DIR=/etc/unbound/unbound.conf.d
PIDFILE=/var/run/dnsblocker/service.pid

display_usage()
{
    echo "Usage: $0 start|stop|restart|status"
}

if test $# -lt 1; then
    display_usage
    exit 1
fi

COMMAND=$1
shift

start_server()
{
    ./bin/dnsblocker \
	--dns-proxy=$DNS_PROXY \
	--dns-proxy-include-dir=$DNS_PROXY_INCLUDE_DIR \
	--dns-proxy-generate-config \
	--pidfile  $PIDFILE \
	--service-port 7654 \
	--http-responder-enable \
	$@
}

stop_server()
{
    if [ -f $PIDFILE ]; then
	kill `cat $PIDFILE`
    fi
}

case "$COMMAND" in
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
	exit 1
esac
