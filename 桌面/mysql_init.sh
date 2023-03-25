###
# 使用了自写的网络号获取函数"getnetnumber"。
###

#!/bin/bash

function getnetnumber(){
	for (( i = 1; i <= 4; i++ )); do
	        a=`ip a| grep global| awk '{print $2}' | awk -F'.' "{print $"$i"}"`
	        b=`ifconfig | grep broadcast | awk '{print $4}' | awk -F'.' "{print $"$i"}"`
	        d=`echo $(("$a" & "$b"))`
	        if [[ $i -ne 4 ]]; then
	                c=$c$d.
	        elif [[ $i -eq 4 ]]; then
	                c=$c$d/
	        fi
	done
	d=`ip a| grep global| awk '{print $2}' | awk -F'/' '{print $2}'`
	c=$c$d
	echo $c
}

timedatectl set-timezone Asia/Shanghai
yum install epel-release -y
yum install -y vim wget bc innotop mytop bind-utils bash-* net-tools iptables-services libaio-devel libaio fio lrzsz glances
#yum install chrony -y
#systemctl restart chronyd
systemctl stop firewalld
systemctl disable firewalld
systemctl mask firewalld
setenforce 0
sed -i 's/enforcing/disabled/g' /etc/selinux/config

# umount /home
# sed -i "/home/d" /etc/fstab
# lvremove /dev/almalinux/home -y
# lvextend -l +100%free /dev/almalinux/root
# xfs_growfs /dev/almalinux/root

echo > /etc/sysconfig/iptables
cat << EOF >> /etc/sysconfig/iptables
*filter
:INPUT ACCEPT [0:0]
:FORWARD ACCEPT [0:0]
:OUTPUT ACCEPT [0:0]
-A INPUT -m state --state INVALID -j DROP
-A INPUT -m state --state RELATED,ESTABLISHED -j ACCEPT
-A INPUT -p icmp -j ACCEPT
-A INPUT -i lo -j ACCEPT
-A INPUT -p tcp -m state --state NEW -m multiport --dports 22 -j ACCEPT
-A INPUT -p tcp -s 172.31.32.0/20 -m state --state NEW -m multiport --dports 9090,3306,27017 -j ACCEPT
-A INPUT -p tcp --syn -m recent --name PortScan --update --second 600 --hitcount 10 -j DROP
-A INPUT -p tcp --syn -m recent --set --name PortScan
-A INPUT -j REJECT --reject-with icmp-host-prohibited
#-A OUTPUT -p tcp -s 172.16.41.0/24 -j ACCEPT
#-A OUTPUT -m state --state NEW -j DROP
-A FORWARD -j REJECT --reject-with icmp-host-prohibited
COMMIT
EOF

# NetID=$(./getnetnumber $(ip a| grep global| awk '{print $2}'| awk -F'/' '{print $1}') $(ip a| grep global| awk '{print $2}'| awk -F'/' '{print $2}'))
NetID=$(getnetnumber)
sed -i "s/172.31.32.0\/20/$NetID/" /etc/sysconfig/iptables
systemctl restart iptables

cat << EOF >> /etc/sysctl.conf
net.ipv6.conf.all.disable_ipv6 = 1
net.ipv6.conf.default.disable_ipv6 = 1
net.ipv6.conf.lo.disable_ipv6 = 1
net.ipv4.tcp_syncookies = 1
net.ipv4.tcp_tw_reuse = 1
net.ipv4.tcp_tw_recycle = 1
net.core.rmem_max=16777216
net.core.wmem_max=16777216
net.ipv4.tcp_rmem=4096 87380 16777216
net.ipv4.tcp_wmem=4096 65536 16777216
net.ipv4.tcp_fin_timeout = 10
net.ipv4.tcp_timestamps = 0
net.ipv4.tcp_window_scaling = 0
net.ipv4.tcp_sack = 0
net.core.netdev_max_backlog = 30000
net.ipv4.tcp_no_metrics_save=1
net.ipv4.tcp_max_orphans = 500
net.ipv4.tcp_max_syn_backlog = 262144
net.ipv4.tcp_synack_retries = 2
net.ipv4.tcp_syn_retries = 2
fs.inotify.max_user_instances=655350
fs.inotify.max_user_watches=655350
EOF
sysctl -p

cat << EOF >> /etc/security/limits.conf
*               soft    nproc           256400
*               hard    nproc           256400
*               soft    nofile          256400
*               hard    nofile          256400
*               soft    memlock          unlimited
*               hard    memlock          unlimited
root            soft    nproc           256400
root            hard    nproc           256400
root            soft    nofile          256400
root            hard    nofile          256400
root            soft    memlock          unlimited
root            hard    memlock          unlimited
EOF

# yum -y install https://mirrors.aliyun.com/zabbix/zabbix/5.0/rhel/7/x86_64/zabbix-agent-5.0.7-1.el7.x86_64.rpm
# cat > /etc/zabbix/zabbix_agentd.conf  << EOF
# PidFile=/var/run/zabbix/zabbix_agentd.pid
# LogFile=/var/log/zabbix/zabbix_agentd.log
# LogFileSize=0
# Server=103.17.93.17
# ServerActive=103.17.93.17
# ListenPort=10050
# Include=/etc/zabbix/zabbix_agentd.d/*.conf
# EOF
# systemctl enable zabbix-agent.service --now
# systemctl restart zabbix-agent
# 
# fio -direct=1 -iodepth=128 -rw=randwrite -ioengine=libaio -bs=4k -size=1G -numjobs=1 -runtime=1000 -group_reporting -filename=iotest -name=Rand_Write_Testing


yum install https://repo.percona.com/yum/percona-release-latest.noarch.rpm -y
percona-release setup ps80 -y
yum install percona-server-server -y
yum install jemalloc -y
cat > /etc/sysconfig/mysql << EOF
LD_PRELOAD=/usr/lib64/libjemalloc.so.1
EOF
systemctl enable mysqld
sed -i 's/LimitNOFILE = 10000/LimitNOFILE = 256000/g' /usr/lib/systemd/system/mysqld.service
systemctl daemon-reload
cat > /etc/my.cnf << EOF
[mysqld]

mysqlx=OFF
#authentication_policy=mysql_native_password
default_authentication_plugin=mysql_native_password
innodb_numa_interleave = ON
explicit_defaults_for_timestamp = 1
max_prepared_stmt_count = 1048576
sql_mode = STRICT_TRANS_TABLES,NO_ENGINE_SUBSTITUTION
activate_all_roles_on_login = ON
innodb_dedicated_server = OFF

# 网络和连接池
bind_address = 0.0.0.0
max_connections = 102400
thread_handling = pool-of-threads          # one-thread-per-connection
thread_pool_size = 32
thread_pool_oversubscribe = 16
thread_pool_max_threads = 5120
thread_pool_stall_limit = 50 			   # 默认值500过大,会影响本可快速完成的sql,造成异常延迟.在大量执行小查询的场景下,影响非常明显.
skip_name_resolve = 1
back_log = 1024
max_connect_errors = 1000000
max_allowed_packet = 32M
interactive_timeout = 600
wait_timeout = 600
thread_stack = 512K

# 打开文件限制和编码
character-set-server = utf8              # MySQL8默认使用utf8mb4
open_files_limit = 256000                # 注意操作系统的设置和启动脚本的设置限制
innodb_open_files = 65535
table_open_cache = 4096
table_definition_cache = 1024
table_open_cache_instances = 64

# 事务和锁
kill_idle_transaction = 10
max_execution_time = 0 				# 最长执行时间,单位是毫秒
external-locking = FALSE
innodb_sync_spin_loops = 100
innodb_spin_wait_delay = 30
innodb_thread_concurrency = 0
transaction_isolation = READ-COMMITTED  # 可选 READ-COMMITTED | REPEATABLE-READ
innodb_lock_wait_timeout = 10
innodb_rollback_on_timeout = 1
innodb_print_all_deadlocks = 1

# 缓存
#sort_buffer_size = 16M
#join_buffer_size = 16M
#thread_cache_size = 768
tmp_table_size = 96M
max_heap_table_size = 96M
#innodb_additional_mem_pool_size = 16M
innodb_buffer_pool_size = 8G
innodb_buffer_pool_instances = 8
innodb_buffer_pool_load_at_startup = 1
innodb_buffer_pool_dump_at_shutdown = 1
#innodb_data_file_path = ibdata1:1G:autoextend
innodb_log_buffer_size = 32M
innodb_log_file_size = 2G
innodb_log_files_in_group = 3
innodb_max_undo_log_size = 4G

# 日志部分
log_timestamps = system
min_examined_row_limit = 100
log_slow_admin_statements = 1
log_slow_slave_statements = 1
#log-bin = /data/mysql/mybinlog  # MySQL8之后二进制默认开启，如需关闭使用disable_log_bin。
disable_log_bin
binlog_cache_size = 16M
max_binlog_cache_size = 64M
max_binlog_size = 1G
#expire_logs_days = 1
binlog_expire_logs_seconds = 28800
innodb_file_per_table = 1
innodb_online_alter_log_max_size = 4G

# 慢查日志
slow_query_log = 1
slow_query_log_file = /var/lib/mysql/slow.log
log-error = /var/lib/mysql/mysqld.log
long_query_time = 0.1
#log_queries_not_using_indexes =1
#log_throttle_queries_not_using_indexes = 60

# 预读
innodb_read_ahead_threshold = 56
innodb_random_read_ahead = OFF

# 落盘
sync_binlog = 64
innodb_flush_log_at_trx_commit = 2
innodb_flush_log_at_timeout = 6
innodb_io_capacity = 4000
innodb_io_capacity_max = 8000
innodb_flush_neighbors = 0
innodb_max_dirty_pages_pct = 25
innodb_max_dirty_pages_pct_lwm = 10
innodb_flush_method = O_DIRECT_NO_FSYNC
innodb_lru_scan_depth = 4000
innodb_checksum_algorithm = crc32
innodb_page_cleaners = 4

# 双写
innodb_doublewrite = OFF

# 并发读写和日志清理
innodb_write_io_threads = 8
innodb_read_io_threads = 8
innodb_purge_threads = 4
innodb_parallel_read_threads = 8

# 复制
server-id = 3306
slave_parallel_type = LOGICAL_CLOCK
slave_parallel_workers = 8
slave_preserve_commit_order = ON
binlog_group_commit_sync_delay = 500
binlog_group_commit_sync_no_delay_count = 10
master_info_repository = TABLE
relay_log_info_repository = TABLE

# GTID
gtid_mode = on
enforce_gtid_consistency = 1
binlog_format = row
relay_log_recovery = 1
relay-log-purge = 1
slave_skip_errors = all
#log_slave_updates
EOF

server_id=$(ip a| grep global| awk '{print $2}'|awk -F'.' '{print $3 $4}'|awk -F'/' '{print $1}')
buffer=$(free | grep -i mem| awk '{print $2*0.7/1024/1024}'|awk -F'.' '{print $1}')G

sed -i "s/server-id = 3306/server-id = $server_id/" /etc/my.cnf
sed -i "s/innodb_buffer_pool_size = 16G/innodb_buffer_pool_size = $buffer/" /etc/my.cnf


