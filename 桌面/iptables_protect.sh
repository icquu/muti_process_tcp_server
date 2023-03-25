#!/bin/bash

iptables -t raw -A PREROUTING -i eth0 -p tcp --tcp-flags FIN,SYN,RST,ACK SYN -j CT --notrack
iptables -I INPUT -i eth0 -p tcp -m state --state INVALID -j DROP
iptables -I INPUT -i eth0 -p tcp -m state --state INVALID,UNTRACKED -j SYNPROXY --sack-perm --timestamp --wscale 7 --mss 1460
iptables -A INPUT -p tcp ! --syn -m state --state NEW -j DROP
iptables -A INPUT -p tcp -m state --state NEW -m tcpmss ! --mss 536:65535 -j DROP
iptables -A INPUT -p tcp --tcp-flags FIN,SYN FIN,SYN -j DROP
iptables -A INPUT -p tcp --tcp-flags SYN,RST SYN,RST -j DROP
iptables -A INPUT -p tcp --tcp-flags FIN,RST FIN,RST -j DROP
iptables -A INPUT -p tcp --tcp-flags FIN,ACK FIN -j DROP
iptables -A INPUT -p tcp --tcp-flags ACK,URG URG -j DROP
iptables -A INPUT -p tcp --tcp-flags ACK,PSH PSH -j DROP
iptables -A INPUT -p tcp --tcp-flags ALL NONE -j DROP
iptables -A INPUT -s 224.0.0.0/3 -j DROP 
iptables -A INPUT -s 169.254.0.0/16 -j DROP 
iptables -A INPUT -s 172.16.0.0/12 -j DROP 
iptables -A INPUT -s 192.0.2.0/24 -j DROP 
iptables -A INPUT -s 192.168.0.0/16 -j DROP 
iptables -A INPUT -s 10.0.0.0/8 -j DROP 
iptables -A INPUT -s 0.0.0.0/8 -j DROP 
iptables -A INPUT -s 240.0.0.0/5 -j DROP 
iptables -A INPUT -s 127.0.0.0/8 ! -i lo -j DROP

/sbin/sysctl -w net/netfilter/nf_conntrack_tcp_loose=0

# Enable timestamping, because SYN cookies uses TCP options field
/sbin/sysctl -w net/ipv4/tcp_timestamps=1

# Adjusting maximum number of connection tracking entries possible
#
# Conntrack element size 288 bytes found in /proc/slabinfo
#  "nf_conntrack" <objsize> = 288
#
# 288 * 2000000 / 10^6 = 576.0 MB
/sbin/sysctl -w net/netfilter/nf_conntrack_max=2000000

# IMPORTANT: Also adjust hash bucket size for conntracks
#   net/netfilter/nf_conntrack_buckets writeable
#   via /sys/module/nf_conntrack/parameters/hashsize
#
# Hash entry 8 bytes pointer (uses struct hlist_nulls_head)
#  8 * 2000000 / 10^6 = 16 MB
sh -c 'echo 2000000 > /sys/module/nf_conntrack/parameters/hashsize'