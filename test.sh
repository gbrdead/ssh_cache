#!/bin/sh

PORT=8022


getPid()
{
    netstat -p -n -a --inet --inet6 2>&1 | grep LISTEN | grep ${1} | awk '{print $7}' | sed 's|/.*$||'
}

PID1=$(getPid ":::${PORT}")
PID2=$(getPid "0.0.0.0:${PORT}")

if test -z "${PID1}"
then
    PID="${PID2}"
elif test -z "${PID2}"
then
    PID="${PID1}"
else
    if test "${PID1}" != "${PID2}"
    then
        echo "Two different processes are listening on IPv4 and IPv6 om port ${PORT}."
        exit 1
    fi
    PID="${PID1}"
fi

if test -z "${PID}"
then
    echo "No process is listening on port ${PORT}."
    exit 2
fi

kill -TERM ${PID}

#sudo sysctl net.ipv6.bindv6only=0
