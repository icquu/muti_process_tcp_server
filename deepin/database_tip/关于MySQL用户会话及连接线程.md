# [关于MySQL用户会话及连接线程](https://www.cnblogs.com/geaozhang/p/7137858.html)

0、概念理解：用户会话和连接线程是什么关系？

> 用户会话和用户连接线程是一一对应的关系，一个会话就一个用户连接线程。

问题描述：

　　如果系统因为执行了一个非常大的dml或者ddl操作导致系统hang住，我们想断掉这个操作，怎么办？

解决办法：

1、kill thread：杀死用户的会话

　　但是时间长，效果不佳：前滚+回滚，前提是已经进行了很长时间，回滚就需要更多的时间

2、kill mysqld进程：推荐，用这种杀进程的方式，速度快

　　kill -9 进程号（ps aux 查看进程号）

　　数据库先前滚，不主动回滚，直接可以对外进行服务了，当读到哪个未提交事务时再去慢慢回滚。

 

**一、kill用户会话及用户连接线程**

1、如何查看用户会话，如何杀掉用户会话

```
[root@localhost ~]# netstat -anp |grep 3306
tcp        0      0 :::3306                     :::*                        LISTEN      17324/mysqld        
[root@localhost ~]# ps -ef |grep 'mysql -px x'
root     20510 20483  0 15:59 pts/1    00:00:00 mysql -px x
root     20528 46646  0 15:59 pts/4    00:00:00 grep mysql -px x
root     45626 45565  0 05:44 pts/3    00:00:00 mysql -px x
[root@localhost ~]# kill -9 20510 
```

　　# kill -9 <mysql会话进程PID>　　//快速释放资源(推荐)

注意：千万不要将mysqld给kill掉了，不要杀掉LISTEN进程

2、如何查看用户连接线程，如何杀掉用户连接线程

```
mysql> show processlist;
+--------+
| Id     |
+--------+
| 194850 |
| 194851 |
+--------+
mysql> kill 194850;
mysql> show processlist;
+--------+
| Id     |
+--------+
| 194851 |
| 194852 |
+--------+
```

注意：如果将用户连接线程杀死断掉，而会话没有杀掉的话，该用户会话又会重新开启一个用户连接线程。

3、杀掉用户连接线程的工作过程、弊端风险

　　1、过程：

　　　　rollback--->释放资源--->kill线程

　　2、弊端与风险：

　　　　1、可能出现系统更加繁忙的情况(因为大量的rollback)

　　　　2、会话释放需要很长的时间

> Q：假设现在有1000个用户连接，如何快速杀掉？
>
> A：
>
> 　　1、使用concat写脚本
>
> 　　　　mysql> select concat('kill ',ID,';') into outpfile '/tmp/kill.txt' from information_schema.PROCESSLIST;
>
> 　　　　shell> cat kill.txt
>
> 　　　　kill 194850;
>
> 　　　　kill 194851;
>
> 　　然后，复制到mysql中进行执行，将用户连接线程都kill掉。
>
> 　　2、使用awk取出用户会话进程ID都kill掉
>
> 　　shell> netstat -anp|grep mysql|grep -v mysqld|awk '{print $8}'|awk -F '/' '{print $1}'|xargs kill -9

关于mysqld_safe的注意点：

　　在os层面将用户连接线程kill掉，通过pstree -p可以看到MySQL相关进程及线程ID，#kill -9 mysql任一线程，会导致该mysqld进程被kill；但是通过mysqld_safe的安全机制，又会重新启一个mysqld进程；如此原来的用户连接线程也就随原来的进程一块被干掉了。

 

**二、MySQL客户端的连接**

1、最大连接数

```
mysql> show variables like 'max_connections';
+-----------------+-------+
| Variable_name   | Value |
+-----------------+-------+
| max_connections | 151   |
+-----------------+-------+
```

由上可见，MySQL默认客户端的最大连接数是151，但是，在大并发下一百多的连接数就会不够用，需要调整最大连接数，修改并写入配置文件中：

　　max_connections = 1000

　　wait_timeout = 1000000　　#超时时间

重启MySQL服务

2、查看当前有多少连接

```
mysql> show status like '%Threads_connected%';
+-------------------+-------+
| Variable_name     | Value |
+-------------------+-------+
| Threads_connected | 1     |
+-------------------+-------+
1 row in set (0.01 sec)

mysql> show processlist;
+-------+------+-----------+------+---------+------+----------+------------------+
| Id    | User | Host      | db   | Command | Time | State    | Info             |
+-------+------+-----------+------+---------+------+----------+------------------+
| 17219 | root | localhost | NULL | Query   |    0 | starting | show processlist |
+-------+------+-----------+------+---------+------+----------+------------------+
1 row in set (0.01 sec)
```

3、最大失败连接数：max_connect_errors

```
mysql> show variables like 'max%errors';
+--------------------+-------+
| Variable_name      | Value |
+--------------------+-------+
| max_connect_errors | 100   |
+--------------------+-------+
1 row in set (0.00 sec)
```

　　是一个MySQL中与安全有关的计数器值，负责阻止过多尝试失败的客户端以暴力破解密码的情况，值的大小与性能无太大的关系。

　　默认是100，也就是说如果某一客户端尝试连接此MySQL服务器，但是失败（如密码错误等等）10次，则MySQL会无条件强制阻止此客户端连接。如果想重置计数器对某一客户端的值，则必须重启mysqld或者mysql> flush hosts;，当该客户端成功连接一次mysqld后，针对此客户端的max_connect_errors会清零。

　　如果max_connect_errors的设置过小，则网页可能提示无法连接数据库服务器。

　　一般来说建议数据库服务器不监听来自网络的连接，仅仅通过sock连接，这样可以防止绝大多数针对mysql的攻击；如果必须要开启mysql的网络连接，则最好设置此值，以防止穷举密码的攻击手段。

 

**三、关于用户工作空间**

Q：如何判断用户会话线程空间(用户工作空间)是否分配过小？

A：

　　1、sort_buffer_size：需要排序会话的缓存大小(默认256K)，是针对每一个connection的；过大的配置+高并发可能会耗尽系统内存资源。

　　2、binlog_cache_size：二进制日志缓冲区(默认32K)，基于会话；

　　一个事务做出的修改，

　　　　当小于binlog_cache_size时，所有修改内容存入binary log cache；

　　　　当大于binlog_cache_size时，内容存入磁盘临时表。

　　3、join_buffer_size：多表连接结果集缓冲区，基于会话；

　　(每次join操作都会调用my_malloc、my_free函数申请/释放join_buffer_size大小的内存)

也就是说，sort_buffer、binlog_cache、join_buffer的组成，形成用户会话线程空间。一般的，当Sort_merge_passes(磁盘排序)每秒值很大时，就说明用户工作空间分配过小，就应该考虑增加sort_buffer_size值。