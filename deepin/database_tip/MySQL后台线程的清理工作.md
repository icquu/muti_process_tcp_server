# [MySQL后台线程的清理工作](https://www.cnblogs.com/geaozhang/p/7225340.html)

> 后台清理工作：脏页刷盘、undo回收
>
> 　　1、page cleaner thread：刷新脏页
>
> 　　2、purge thread：清空undo页、清理“deleted”page

**一、innodb_page_cleaners**

　　page cleaner线程从buffer pool中刷脏页的线程数量。

1、5.7新特性

　　1、5.6版本以前，脏页的清理工作交由master线程的；

　　2、Page cleaner thread是5.6.2引入的一个新线程（单线程），从master线程中卸下buffer pool刷脏页的工作独立出来的线程(默认是启一个线程)；

　　3、5.7开始支持多线程刷脏页；

2、发起脏页的写请求

　　清理因为触发需要脏页回收的脏页（脏很久了、冷脏页……）

　　注意：真正干活的，将dirty page写入磁盘的是innodb_write_io_threads

3、如何调整这个参数

```
mysql> show variables like 'i%cleaners';
+----------------------+-------+
| Variable_name        | Value |
+----------------------+-------+
| innodb_page_cleaners | 1     |
+----------------------+-------+
1 row in set (0.05 sec)
```

　　1、配置文件my.cnf中添加innodb_page_cleaners=num值

　　2、默认是1；最大可以是64，也就是会有64个page cleaner线程并发工作清理脏页

4、如何判断是否要修改增加innodb_page_cleaners

```
mysql> show global status like '%wait_free';
+------------------------------+-------+
| Variable_name                | Value |
+------------------------------+-------+
| Innodb_buffer_pool_wait_free | 0     |
+------------------------------+-------+
1 row in set (0.01 sec)
```

　　Innodb_buffer_pool_wait_free：标志着脏页有没有成为系统的性能瓶颈；如果值很大，则需要增加innodb_page_cleaners值，同时增加写线程。

 ![img](https://images2015.cnblogs.com/blog/1113510/201707/1113510-20170723165627080-1184970362.jpg)

　　1、通常，对于buffer pool的写发生在后台，当innodb需要读或创建一个数据页，但是没有干净的可用页，innodb就会为等待的操作能完成先将一些脏页刷入磁盘。

　　2、Innodb_buffer_pool_wait_free就是这等待操作的实例数。如果innodb_buffer_pool_size的大小设置适当，这个值就会很小，甚至为0。

 

**二、innodb_purge_threads**

　　purge线程，后台线程，致力于innodb清理，资源回收操作。

1、清理操作

　　1、清理undo页

　　　　undo记录修改前的数据用于回滚，已提交的时候，不再回滚，即可清理该undo信息。

　　2、清理page里面的有“deleted”标签的数据行

　　　　1、当我们delete数据行时，是对数据页中要删除的数据行做标记“deleted”，事务提交(速度快)；

　　　　2、后台线程purge线程对数据页中有“deleted”标签的数据行进行真正的删除。

2、调整依据

　　1、系统存在大量的delete、对主键的update

```
mysql> show global status like '%rows%d%ted';
+---------------------+-------+
| Variable_name       | Value |
+---------------------+-------+
| Innodb_rows_deleted | 0     |
| Innodb_rows_updated | 5     |
+---------------------+-------+
2 rows in set (0.01 sec)
```

　　2、mysql> show engine innodb status \G

```
Trx id counter 1159171　　#事务计数
Purge done for trx's n:o < 1157813　　#事务清空位置
　　　　#1159171-1157813表示有待清空的事务量

undo n:o < 0　　#当前清理事务undo位置
state: running but idle　　#启动但是闲置
History list length 1029　　#当前undo数据页的总量1029*16K
```

3、调整：innodb_purge_threads默认值是1，OLTP系统一般会修改为4

```
mysql> show variables like '%purge_t%';
+----------------------+-------+
| Variable_name        | Value |
+----------------------+-------+
| innodb_purge_threads | 4     |
+----------------------+-------+
1 row in set (0.01 sec)
```

@author：http://www.cnblogs.com/geaozhang/