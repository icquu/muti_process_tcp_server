#!/bin/sh

logfile=/database/mongo
days=7
/bin/kill -SIGUSR1 `ps -ef | grep mongodb.conf | grep -v grep | awk '{print $2}'`
sleep 10
#rm -f $logfile/mongod.log.*
find $logfile/ -name mongodb.log.* -mtime +$days -delete
