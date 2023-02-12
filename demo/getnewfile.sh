#!/bin/bash

inotifywait  -r -mq -e close_write /var/log | while read event;
do
	filename=`echo $event|awk '{print $3}'`
	cat /var/log/$filename

done
