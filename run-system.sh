#!/bin/sh


PIDFILE=/var/run/dnsblocker/service.pid

start_server()
{
    ./bin/dnsblocker \
	-v \
	--dns-proxy=unbound \
	--dns-proxy-generate-config \
	--pidfile  $PIDFILE \
	--service-port 79 \
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
	    kill -0 `cat $PIDFILE`
	    if [ $? = 0 ]; then
		echo "Running"
		exit 0
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
