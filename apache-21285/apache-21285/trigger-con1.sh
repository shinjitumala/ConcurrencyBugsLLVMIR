#!/bin/sh

CNT=1
THRESHOLD=$1

while [ $CNT -ne $THRESHOLD ]; do
	echo "trigger con1: $CNT"

	GET http://127.0.0.1/cgi-bin/trigger.cgi?000$CNT > /dev/null

	CNT=`expr $CNT + 1`
done

