# [MySQL 5.7 新备份工具mysqlpump 使用说明 - 运维小结](https://www.cnblogs.com/kevingrace/p/9760185.html)

 

之前详细介绍了[Mysqldump备份工具](https://www.cnblogs.com/kevingrace/p/9403353.html)使用，下面说下MySQL5.7之后新添加的备份工具mysqlpump。mysqlpump是mysqldump的一个衍生，mysqldump备份功能这里就不多说了，现在看看mysqlpump到底有了哪些提升，详细可以查看[官网文档](https://dev.mysql.com/doc/refman/5.7/en/mysqlpump.html)。mysqlpump和mysqldump一样，属于逻辑备份，备份以SQL形式的文本保存。逻辑备份相对物理备份好处是不关心log的大小，直接备份数据即可。

**Mysqlpump主要特点**
-  并行备份数据库和数据库中的对象的，加快备份过程。
-  更好的控制数据库和数据库对象（表，存储过程，用户帐户）的备份。
-  备份用户账号作为帐户管理语句（CREATE USER，GRANT），而不是直接插入到MySQL的系统数据库。
-  备份出来直接生成压缩后的备份文件。
-  备份进度指示（估计值）。
-  重新加载（还原）备份文件，先建表后插入数据最后建立索引，减少了索引维护开销，加快了还原速度。
-  备份可以排除或则指定数据库。

**Mysqlpump缺点**
-  只能并行到表级别,如果表特别大,开多线程和单线程是一样的,并行度不如mydumper；
-  无法获取当前备份对应的binlog位置；
-  MySQL5.7.11之前的版本不要使用,并行导出和single-transaction是互斥的；

**参数说明：**Mysqlpump绝大部分参数使用和Mysqldump一致，下面顺便重温一下。注意对于mysqlpump 专有参数会用背景色       标记出来。

1) **--add-drop-database:**   **在建立库之前先执行删库操作**

2) **--add-drop-table**：**在建表之前先执行删表操作**

3) **--add-drop-user**：**在CREATE USER语句之前增加DROP USER。** **注意**：这个参数需要和--users一起使用，否者不生效。

4) **--add-locks**：**备份表时，使用LOCK TABLES和UNLOCK TABLES**。**注意**：这个参数不支持并行备份，需要关闭并行备份功能：--default-parallelism=0

5) **--all-databases**：**备份所有库**，即 -A。

6) **--bind-address**：**指定通过哪个网络接口来连接Mysql服务器（一台服务器可能有多个IP），防止同一个网卡出去影响业务**。

7) **--complete-insert**：**dump出包含所有列的完整insert语句**。

8) **--compress**： **压缩客户端和服务器传输的所有的数据**，即 -C。

9) **--compress-output**：**默认不压缩输出，目前可以使用的压缩算法有LZ4和ZLIB**

10) **--databases**：**手动指定要备份的库，支持多个数据库，用空格分隔，**即-B。

11) **--default-character-set**：**指定备份的字符集**。

12) **--default-parallelism**：**指定并行线程数，默认是2，如果设置成0，表示不使用并行备份**。注意：每个线程的备份步骤是：先create table但不建立二级索引（主键会在create table时候建立），再写入数据，最后建立二级索引。

13) **--defer-table-indexes**：**延迟创建索引，直到所有数据都加载完之后，再创建索引，默认开启**。若关闭则会和mysqldump一样：先创建一个表和所有索引，再导入数据，因为在加载还原数据的时候要维护二级索引的开销，导致效率比较低。关闭使用参数：--skip--defer-table-indexes。

14) **--events**：**备份数据库的事件，默认开启，关闭使用--skip-events参数。**

15) **--exclude-databases**：**备份排除该参数指定的数据库，多个用逗号分隔。**类似的还有--exclude-events、--exclude-routines、--exclude-tables、--exclude-triggers、--exclude-users

**注意：**要是只备份数据库的账号，需要添加参数[--users](https://dev.mysql.com/doc/refman/5.7/en/mysqlpump.html#option_mysqlpump_users)，并且需要过滤掉所有的数据库，如

16) **--include-databases**：**指定备份数据库，多个用逗号分隔**，类似的还有--include-events、--include-routines、--include-tables、--include-triggers、--include-users，大致方法使用同15。

17) **--insert-ignore**：**备份用insert ignore语句代替insert语句**。

18) **--log-error-file**：**备份出现的warnings和erros信息输出到一个指定的文件**。

19) **--max-allowed-packet**：**备份时用于client/server直接通信的最大buffer包的大小**。

20) **--net-buffer-length**：**备份时用于client/server通信的初始buffer大小，当创建多行插入语句的时候，mysqlpump 创建行到N个字节长**。

21) **--no-create-db**：**备份不写CREATE DATABASE语句**。要是备份多个库，需要使用参数-B，而使用-B的时候会出现create database语句，该参数可以屏蔽create database 语句。

22) **--no-create-info**：**备份不写建表语句，即不备份表结构，只备份数据**，即 -t。

23) **--hex-blob**： **备份binary字段的时候使用十六进制计数法**，受影响的字段类型有BINARY、VARBINARY、BLOB、BIT。

24) **--host** ：**备份指定的数据库地址**，即 -h。

25) **--parallel-schemas=[N:]db_list**：**指定并行备份的库，多个库用逗号分隔**，如果指定了N，将使用N个线程的地队列，如果N不指定，将由 --default-parallelism才确认N的值，可以设置多个--parallel-schemas

26) **--password**：**备份需要的密码**。

27) **--port** ：**备份数据库的端口**。

28) **--protocol={TCP|SOCKET|PIPE|MEMORY}**：**指定连接服务器的协议**。

29) **--replace**：**备份出来replace into语句**。

30) **--routines**：**备份出来包含存储过程和函数，默认开启，需要对 mysql.proc表有查看权限**。生成的文件中会包含CREATE PROCEDURE 和 CREATE FUNCTION语句以用于恢复，关闭则需要用--skip-routines参数。

31) **--triggers**：**备份出来包含触发器，默认开启，使用--skip-triggers来关闭**。

31) **--set-charset**：**备份文件里写SET NAMES default_character_set 到输出，此参默认开启。** -- skip-set-charset禁用此参数，不会在备份文件里面写出set names...

32) **--single-transaction**：**该参数在事务隔离级别设置成Repeatable Read，并在dump之前发送start transaction 语句给服务端**。这在使用innodb时很有用，因为在发出start transaction时，保证了在不阻塞任何应用下的一致性状态。对myisam和memory等非事务表，还是会改变状态的，当使用此参的时候要确保没有其他连接在使用ALTER TABLE、CREATE TABLE、DROP TABLE、RENAME TABLE、TRUNCATE TABLE等语句，否则会出现不正确的内容或则失败。--add-locks和此参互斥，在mysql5.7.11之前，--default-parallelism大于1的时候和此参也互斥，必须使用--default-parallelism=0。5.7.11之后解决了--single-transaction和--default-parallelism的互斥问题。

33) **--skip-definer**：**忽略那些创建视图和存储过程用到的 DEFINER 和 SQL SECURITY 语句，恢复的时候，会使用默认值，否则会在还原的时候看到没有DEFINER定义时的账号而报错。**

34) **--skip-dump-rows**：**只备份表结构，不备份数据，**即-d。注意：mysqldump支持--no-data，mysqlpump不支持--no-data

35) **--socket**：**对于连接到localhost，Unix使用套接字文件，在Windows上是命名管道的名称使用**，即 -S。

36) **--ssl**：**--ssl参数将要被去除，用--ssl-mode取代**。关于ssl相关的备份。

37) **--tz-utc**：**备份时会在备份文件的最前几行添加SET TIME_ZONE='+00:00'。**注意：如果还原的服务器不在同一个时区并且还原表中的列有timestamp字段，会导致还原出来的结果不一致。默认开启该参数，用 --skip-tz-utc来关闭参数。

38) **--user**：**备份时候的用户名**，即 -u。

39) **--users**：**备份数据库用户，备份的形式是CREATE USER...，GRANT...**，只备份数据库账号可以通过如下命令

40) **--watch-progress**：**定期显示进度的完成，包括总数表、行和其他对象。该参数默认开启，用--skip-watch-progress来关闭。** 

**Mysqlpump的多线程架构图如下**

![img](https://img2018.cnblogs.com/blog/907596/201901/907596-20190115162636956-1342333863.png)

-  mysqlpump是MySQL5.7的官方工具,用于取代mysqldump,其参数与mysqldump基本一样；
-  mysqlpump是多线程备份,但只能到表级别,单表备份还是单线程；
-  mysqldump备份时,有个默认队列（default）,队列下开N个线程去备份数据库/数据库中的表；
-  支持开多个队列(对应不同库/表),然后每个队列设置不同线程,进行备份；

**Mysqlpump支持基于库和表的并行导出**，Mysqlpump的并行导出功能的架构为：队列+线程，允许有多个队列（**--parallel-schemas**），每个队列下有多个线程（N），而一个队列可以绑定1个或者多个数据库（逗号分隔）。**Mysqlpump的备份是基于表并行的，对于每张表的导出只能是单个线程的**，这里会有个限制是如果某个数据库有一张表非常大，可能大部分的时间都是消耗在这个表的备份上面，并行备份的效果可能就不明显。这里可以利用**Mydumper**其是以chunk的方式批量导出，即**Mydumper支持一张表多个线程以chunk的方式批量导出**。但相对于Mysqldump有很大提升。

​                                                        **对比测试如下**            

`mysqlpump压缩备份kevin数据库 三个并发线程备份，消耗时间：222s`

`[root@localhost ~]``# mysqlpump -uroot -p123456 -h172.16.60.211 --single-transaction --default-character-set=utf8 --compress-output=LZ4 --default-parallelism=3 -B kevin > /data/db_backup/kevin_db.sql.lz4` 

`mysqldump备份压缩kevin数据库 单个线程备份，消耗时间：900s，``gzip``的压缩率比LZ4的高`

`[root@localhost ~]``# mysqldump -uroot -p123456 -h172.16.60.211 --default-character-set=utf8 -P3306 --skip-opt --add-drop-table --create-options  --quick --extended-insert --single-transaction -B kevin | gzip > /data/db_backup/kevin.sql.gz`

 `mydumper备份kevin数据库 三个并发线程备份，消耗时间：300s，``gzip``的压缩率比LZ4的高`

`[root@localhost ~]``# mydumper -u root -p123456  -h 172.16.60.211 -P 3306 -t 3 -c -l 3600 -s 10000000 -B kevin -o /data/db_backup/kevin/`

`mydumper备份kevin数据库，五个并发线程备份，并且开启对一张表多个线程以chunk的方式批量导出，-r。消耗时间：180s`

`[root@localhost ~]``# mydumper -u root -p123456  -h 172.16.60.211 -P 3306 -t 5 -c -r 300000 -l 3600 -s 10000000 -B kevin -o /data/db_backup/kevin/`

 

`注意： 如果是开启了GTID功能的数据库，备份时还需要添加``"--set-gtid-purged=off"``参数，否则可能会报错！`                                         

从上面看出，mysqlpump的备份效率是最快的，mydumper次之，mysqldump最差。所以在IO允许的情况下，能用多线程就别用单线程备份。并且mysqlpump还支持多数据库的并行备份，而mydumper要么备份一个库，要么就备份所有库。可以看出，在mysql数据库备份方面，mysqlpump比mysqldump的测试结果要好。由于实际情况不同，测试给出的速度提升只是参考。到底开启多少个并行备份的线程，这个看磁盘IO的承受能力，若该服务器只进行备份任务，可以最大限制的来利用磁盘。

测试中发现mysqlpump和mysqldump对比:
\- mysqldump默认是不会有建库命令， 但是默认会有drop table的命令;
\- mysqlpump默认是有建库命令，但是不会有drop table的命令，所以mysqlpump恢复的时候不要直接< file.sql ;
\- mysqldump恢复时会先创建表及其所有索引，然后再导入数据；mysqlpump恢复时会先创建表，然后再导入数据，最后建索引;
\- mysqlpump可以指定多线程并发备份，默认是2个；备份时会有进度指示，虽然只是估计值，但不会再想mysqldump备份时那么枯燥，看不到过程.

**注意：****mysqlpump备份的几个重要参数**

`--default-parallelism   指定线程数,默认开2个线程进行并发备份`

`--parallel-schemas      指定哪些数据库进行并发备份`

`--``set``-gtid-purged=OFF   这个是5.7.18版本后加入的参数,`

 `--``set``-gtid-purged=OFF这个参数很重要，如果备份命令里不加上，则备份可能会报错:`

`Warning: A partial dump from a server that has GTIDs will by default include the GTIDs of all transactions, even those that changed`

`suppressed parts of the database. If you don't want to restore GTIDs, pass --``set``-gtid-purged=OFF. To ``make` `a complete dump, pass`

`--all-databases --triggers --routines --events.`

​                                                                   **备份演示如下**                                                                

**-   备份命令如下** 

`[root@localhost ~]``# mysqlpump --single-transaction --set-gtid-purged=OFF --parallel-schemas=2:kevin --parallel-schemas=4:dbt3 -B kevin dbt3 -p123456 > /tmp/backup.sql`

`mysqlpump: [Warning] Using a password on the ``command` `line interface can be insecure.`

`Dump progress: 1``/5` `tables, 0``/7559817` `rows`

`Dump progress: 3``/15` `tables, 286750``/12022332` `rows`

`Dump progress: 3``/15` `tables, 686750``/12022332` `rows`

`Dump progress: 3``/15` `tables, 1042250``/12022332` `rows`

`...`

`Dump completed ``in` `43732 milliseconds`

 

`接着另外打开一个终端会话，登录mysql看下情况`

`(root@172.16.0.10) [(none)]> show processlist;`

`+--------+------+------------------+------+---------+------+-------------------+------------------------------------------------------------------------------------------------------+`

`| Id     | User | Host             | db   | Command | Time | State             | Info                                                                                                 |`

`+--------+------+------------------+------+---------+------+-------------------+------------------------------------------------------------------------------------------------------+`

`| 138199 | root | 172.16.60.50:39238 | NULL | Query   |    0 | starting          | show processlist                                                                                     |`

`| 138267 | root | 172.16.60.50:39776 | NULL | Sleep   |    2 |                   | NULL                                                                                                 |`

`| 138268 | root | 172.16.60.50:39778 | NULL | Query   |    2 | Sending to client | SELECT SQL_NO_CACHE `emp_no`,`dept_no`,`from_date`,`to_date`  FROM `kevin`.`dept_emp`            |`

`| 138269 | root | 172.16.60.50:39780 | NULL | Query   |    2 | Sending to client | SELECT SQL_NO_CACHE `emp_no`,`birth_date`,`first_name`,`last_name`,`gender`,`hire_date`  FROM `emplo |`

`| 138270 | root | 172.16.60.50:39782 | NULL | Query   |    2 | Sending to client | SELECT SQL_NO_CACHE `o_orderkey`,`o_custkey`,`o_orderstatus`,`o_totalprice`,`o_orderDATE`,`o_orderpr |`

`| 138271 | root | 172.16.60.50:39784 | NULL | Query   |    2 | Sending to client | SELECT SQL_NO_CACHE `p_partkey`,`p_name`,`p_mfgr`,`p_brand`,`p_type`,`p_size`,`p_container`,`p_retai |`

`| 138272 | root | 172.16.60.50:39786 | NULL | Query   |    2 | Sending data      | SELECT SQL_NO_CACHE `l_orderkey`,`l_partkey`,`l_suppkey`,`l_linenumber`,`l_quantity`,`l_extendedpric |`

`| 138273 | root | 172.16.60.50:39788 | NULL | Query   |    2 | Sending to client | SELECT SQL_NO_CACHE `c_custkey`,`c_name`,`c_address`,`c_nationkey`,`c_phone`,`c_acctbal`,`c_mktsegme |`

`| 138274 | root | 172.16.60.50:39790 | NULL | Sleep   |    2 |                   | NULL                                                                                                 |`

`| 138275 | root | 172.16.60.50:39792 | NULL | Sleep   |    1 |                   | NULL                                                                                                 |`

`+--------+------+------------------+------+---------+------+-------------------+------------------------------------------------------------------------------------------------------+`

`10 rows ``in` `set` `(0.00 sec)`

 

`可以看到138268和138269在备份kevin库,138270,138271,138272,138273在备份dbt3,这里没打印全。`

**-   备份过程如下:**

`终端会话1:`

`(root@localhost) [(none)]> truncate mysql.general_log;`

`Query OK, 0 rows affected (0.10 sec)`

 

`(root@localhost) [(none)]> ``set` `global log_output = ``'table'``;`

`Query OK, 0 rows affected (0.00 sec)`

 

`(root@localhost) [(none)]> ``set` `global general_log = 1;`

`Query OK, 0 rows affected (0.03 sec)`

 

 

`终端会话2:`

`[root@VM_0_5_centos ~]``# mysqlpump --single-transaction kevin --set-gtid-purged=OFF -p123456> /tmp/backup.sql`

`Dump completed ``in` `592 milliseconds`

 

`(root@localhost) [(none)]> ``select` `thread_id,left(argument, 64) from mysql.general_log order by event_time;`

`................`

`................`

`+-----------+------------------------------------------------------------------+`

`|         7 | root@localhost on  using Socket                                  |`

`|         7 | FLUSH TABLES WITH READ LOCK                                      |`

`|         7 | SHOW WARNINGS                                                    |`

`|         7 | SET SESSION TRANSACTION ISOLATION LEVEL REPEATABLE READ          |`

`|         7 | SHOW WARNINGS                                                    |`

`|         7 | START TRANSACTION WITH CONSISTENT SNAPSHOT                       |`

`|         7 | SHOW WARNINGS                                                    |`

`|         8 | root@localhost on  using Socket                                  |`

`|         8 | SET SESSION TRANSACTION ISOLATION LEVEL REPEATABLE READ          |`

`|         8 | SHOW WARNINGS                                                    |`

`|         8 | START TRANSACTION WITH CONSISTENT SNAPSHOT                       |`

`|         8 | SHOW WARNINGS                                                    |`

`|         9 | root@localhost on  using Socket                                  |`

`|         9 | SET SESSION TRANSACTION ISOLATION LEVEL REPEATABLE READ          |`

`|         9 | SHOW WARNINGS                                                    |`

`|         9 | START TRANSACTION WITH CONSISTENT SNAPSHOT                       |`

`|         9 | SHOW WARNINGS                                                    |`

`|         7 | UNLOCK TABLES                                                    |`

`|         7 | SHOW WARNINGS                                                    |`

`|         9 | SET SQL_QUOTE_SHOW_CREATE= 1                                     |`

`|         9 | SHOW WARNINGS                                                    |`

`|         9 | SET TIME_ZONE=``'+00:00'`                                           `|`

`|         8 | SET SQL_QUOTE_SHOW_CREATE= 1                                     |`

`|         8 | SHOW WARNINGS                                                    |`

`|         8 | SET TIME_ZONE=``'+00:00'`                                           `|`

`|         3 | ``set` `global general_log = 0                                       |`

`+-----------+------------------------------------------------------------------+`

 

`根据上面信息，可以看出：`

`-  线程7 进行 FLUSH TABLES WITH READ LOCK 。对表加一个读锁`

`-  线程7、8、9分别开启一个事务（RR隔离级别）去备份数据,由于之前锁表了,所以这三个线程备份出的数据是具有一致性的`

`-  线程7 解锁 UNLOCK TABLE`

`-  整个过程都没有获取二进制位置点`

**-   compress-output**
mysqlpump支持压缩输出,支持LZ4和ZLIB（ZLIB压缩比相对较高,但是速度较慢）

`[root@localhost tmp]``# mysqlpump --single-transaction --compress-output=lz4 kevin --set-gtid-purged=OFF -p123456 > /tmp/backup_kevin.sql`

`Dump completed ``in` `511 milliseconds`

**-   备份恢复** 

未压缩的备份

`mysql < ``source` `/tmp/backup``.sql;`

压缩过的备份

`先解压`

`[root@localhost ~]``# lz4_decompress /tmp/backup_kevin.sql /tmp/kevin.sql`

 

`再导入`

`mysql < ``source` `/tmp/kevin``.sql;`

可以看出来,这个导入是单线程。**mysqlpump备份的数据恢复时会先插入数据, 再建索引, 而mysqldump备份的数据恢复是在建立表的时候就把索引加上了, 所以前者备份的数据恢复时速度要快一点！**

总体来说mysqlpump还是很好用的，尤其是多数据库表的备份。不过如果有一张表格外大，那么备份的大部分时间还是要消耗在这张表上，因为mysqlpump的备份是基于表并行的，对于每张表的导出只能是单个线程的。另外**注意mysqlpump备份时并发线程的数量还是要看自身服务器的IO负载能力，并不是说一味的增加并发线程数量就可以加快速度。**mysqldump和mysqlpump的使用方法绝大部分一致，mysqlpump新的参数文章上已经标明，到底用那种工具备份数据库这个要在具体的环境下才能做出选择，有些时候可能用物理备份更好（xtrabackup），总之根据需要进行测试，最后再决定使用哪种备份工具进行备份。

*************** 当你发现自己的才华撑不起野心时，就请安静下来学习吧！***************