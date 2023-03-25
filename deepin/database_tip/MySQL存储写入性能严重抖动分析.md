# [MySQL存储写入性能严重抖动分析](https://www.cnblogs.com/geaozhang/p/7184712.html)

> 案例描述：
>
> 　　通过iostat发现存储的写性能长期维持在10MB左右，而且因为写性能差已经导致数据库性能变差；
>
> 　　两个小时以后，iostat发现系统的写性能已经能够到100MB以上，数据库性能也恢复正常。
>
> 也就是说，在对系统、数据库监控中，出现了性能波谷，存储写入性能严重抖动，为什么？

**一、原理过程**

![img](https://images2015.cnblogs.com/blog/1113510/201707/1113510-20170715201850822-1469574523.jpg)

由上原理图，进行过程解析：

　　1、事务提交，修改buffer_pool中的数据形成脏页，并且同时生成redo日志，将日志写入磁盘redo log中；

　　2、事务提交成功；

假设，有三组redo log文件，

　　3、继续事务提交，修改数据，写redo log，如果innodb_log_file_size的数值很小，但是产生的redo日志信息很大，这样第一组redo日志很快就会被写满，就要进行日志文件切换；

　　4、如果三组redo log文件都被写满了，就需要进行日志覆盖；

　　5、如果需要覆盖的redo log日志文件记录的脏页信息还没有写入到磁盘中，数据库就会主动加大写的力度将脏页信息刷到磁盘里；

　　6、在将redo日志文件对应的脏页刷入磁盘的过程中，事务没有办法提交，影响业务。

 

**二、原理分析**

1、通过对redo log的修改时间查看日志文件切换时间

　　通过shell> ls -l /mydata/ib_logfile*，查看开始修改的时间和最后修改的时间，计算logfile切换时间。

2、如果redo日志文件切换时间过短，也就是切换频繁，就很容易导致写抖动

　　1、正常业务繁忙的会是10-20分钟；

　　2、如果是比一般的时间短，说明文件小，切换频繁。

 

**三、解决写抖动问题**

1、增加redo日志文件数量

```
mysql> show global variables like 'innodb_log_files_in_group';
+---------------------------+-------+
| Variable_name             | Value |
+---------------------------+-------+
| innodb_log_files_in_group | 2     |
+---------------------------+-------+
1 row in set (0.01 sec)
```

2、扩大日志文件容量

```
mysql> show global variables like 'innodb_log_file_size';
+----------------------+----------+
| Variable_name        | Value    |
+----------------------+----------+
| innodb_log_file_size | 50331648 |
+----------------------+----------+
1 row in set (0.01 sec)
```

3、提高log file的写性能：将日志文件放到写性能优质的磁盘上

```
mysql> show global variables like 'innodb_log_group_home_dir';
+---------------------------+-------+
| Variable_name             | Value |
+---------------------------+-------+
| innodb_log_group_home_dir | ./    |
+---------------------------+-------+
1 row in set (0.01 sec)
```

 注意：

　　上面的这三个参数都非动态参数，需要在配置文件/etc/my.cnf中进行修改保存，再重新启动数据库实例才能生效。