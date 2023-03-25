#!/bin/bash
# ##
# 获取当前主机的网络号
# ##

for (( i = 1; i <= 4; i++ )); do
	a=`ip a| grep global| awk '{print $2}' | awk -F'.' "{print $"$i"}"`
	b=`ifconfig | grep broadcast | awk '{print $4}' | awk -F'.' "{print $"$i"}"`
	c=`echo $(("$a" & "$b"))`
	if [[ $i -ne 4 ]]; then
		printf $c.
	elif [[ $i -eq 4 ]]; then
		printf $c/
	fi
done
ip a| grep global| awk '{print $2}' | awk -F'/' '{print $2}'