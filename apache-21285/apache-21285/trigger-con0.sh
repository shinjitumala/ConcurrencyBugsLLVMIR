#!/bin/sh

CNT=1
THRESHOLD=$1

while [ $CNT -ne $THRESHOLD ]; do
	echo "trigger con0: $CNT"

	GET http://127.0.0.1/cgi-bin/trigger.cgi?$CNT > /dev/null

	CNT=`expr $CNT + 1`
done

