#!/bin/bash

while [[ true ]]; do
	rsync -avzP /var/lib/mongo /mnt
	sleep 300
done
