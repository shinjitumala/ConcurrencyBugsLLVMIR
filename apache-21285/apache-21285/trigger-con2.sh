#!/bin/sh

CNT=1
THRESHOLD=$1

while [ $CNT -ne $THRESHOLD ]; do
	echo "trigger con2: $CNT"

	GET http://127.0.0.1/cgi-bin/trigger.cgi?$CNT.000 > /dev/null

	CNT=`expr $CNT + 1`
done

