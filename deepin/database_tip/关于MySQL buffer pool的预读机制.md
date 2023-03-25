# [关于MySQL buffer pool的预读机制](https://www.cnblogs.com/geaozhang/p/7397699.html)

> [预读机制](https://www.cnblogs.com/geaozhang/p/7397699.html#yi)
>
> [两种预读算法](https://www.cnblogs.com/geaozhang/p/7397699.html#er)
>
> 　　1、[线性预读](https://www.cnblogs.com/geaozhang/p/7397699.html#xianxing)
>
> 　　2、[随机预读](https://www.cnblogs.com/geaozhang/p/7397699.html#suiji)
>
> [对预读的监控](https://www.cnblogs.com/geaozhang/p/7397699.html#jiankong)

**一、预读机制**　　

　　InnoDB在I/O的优化上有个比较重要的特性为预读，预读请求是一个i/o请求，它会异步地在缓冲池中预先回迁多个页面，预计很快就会需要这些页面，这些请求在一个范围内引入所有页面。InnoDB以64个page为一个extent，那么InnoDB的预读是以page为单位还是以extent？

 ![img](https://images2017.cnblogs.com/blog/1113510/201708/1113510-20170819191007209-586174344.png)

　　数据库请求数据的时候，会将读请求交给文件系统，放入请求队列中；相关进程从请求队列中将读请求取出，根据需求到相关数据区(内存、磁盘)读取数据；取出的数据，放入响应队列中，最后数据库就会从响应队列中将数据取走，完成一次数据读操作过程。

　　接着进程继续处理请求队列，(如果数据库是全表扫描的话，数据读请求将会占满请求队列)，判断后面几个数据读请求的数据是否相邻，再根据自身系统IO带宽处理量，进行预读，进行读请求的合并处理，一次性读取多块数据放入响应队列中，再被数据库取走。(如此，一次物理读操作，实现多页数据读取，rrqm>0（# iostat -x），假设是4个读请求合并，则rrqm参数显示的就是4)

 

**二、两种预读算法**

　　InnoDB使用两种预读算法来提高I/O性能：线性预读（linear read-ahead）和随机预读（randomread-ahead）

　　为了区分这两种预读的方式，我们可以把线性预读放到以extent为单位，而随机预读放到以extent中的page为单位。线性预读着眼于将下一个extent提前读取到buffer pool中，而随机预读着眼于将当前extent中的剩余的page提前读取到buffer pool中。

1、线性预读（linear read-ahead）

　　线性预读方式有一个很重要的变量控制是否将下一个extent预读到buffer pool中，通过使用配置参数innodb_read_ahead_threshold，控制触发innodb执行预读操作的时间。

　　如果一个extent中的被顺序读取的page超过或者等于该参数变量时，Innodb将会异步的将下一个extent读取到buffer pool中，innodb_read_ahead_threshold可以设置为0-64的任何值(因为一个extent中也就只有64页)，默认值为56，值越高，访问模式检查越严格。

```
mysql> show variables like 'innodb_read_ahead_threshold';
+-----------------------------+-------+
| Variable_name               | Value |
+-----------------------------+-------+
| innodb_read_ahead_threshold | 56    |
+-----------------------------+-------+
```

　　例如，如果将值设置为48，则InnoDB只有在顺序访问当前extent中的48个pages时才触发线性预读请求，将下一个extent读到内存中。如果值为8，InnoDB触发异步预读，即使程序段中只有8页被顺序访问。

　　可以在MySQL配置文件中设置此参数的值，或者使用SET GLOBAL需要该SUPER权限的命令动态更改该参数。

　　在没有该变量之前，当访问到extent的最后一个page的时候，innodb会决定是否将下一个extent放入到buffer pool中。

2、随机预读（randomread-ahead）

　　随机预读方式则是表示当同一个extent中的一些page在buffer pool中发现时，Innodb会将该extent中的剩余page一并读到buffer pool中。

```
mysql> show variables like 'innodb_random_read_ahead';
+--------------------------+-------+
| Variable_name            | Value |
+--------------------------+-------+
| innodb_random_read_ahead | OFF   |
+--------------------------+-------+
```

　　由于随机预读方式给innodb code带来了一些不必要的复杂性，同时在性能也存在不稳定性，在5.5中已经将这种预读方式废弃，默认是OFF。若要启用此功能，即将配置变量设置innodb_random_read_ahead为ON。

 

**三、监控Innodb的预读**

1、可以通过show engine innodb status\G显示统计信息

```
mysql> show engine innodb status\G
----------------------
BUFFER POOL AND MEMORY
----------------------
……
Pages read ahead 0.00/s, evicted without access 0.00/s, Random read ahead 0.00/s
……
```

　　1、Pages read ahead：表示每秒读入的pages；

　　2、evicted without access：表示每秒读出的pages；

　　3、一般随机预读都是关闭的，也就是0。

2、通过两个状态值，评估预读算法的有效性

```
mysql> show global status like '%read_ahead%';
+---------------------------------------+-------+
| Variable_name                         | Value |
+---------------------------------------+-------+
| Innodb_buffer_pool_read_ahead_rnd     | 0     |
| Innodb_buffer_pool_read_ahead         | 2303  |
| Innodb_buffer_pool_read_ahead_evicted | 0     |
+---------------------------------------+-------+
3 rows in set (0.01 sec)
```

　　1、Innodb_buffer_pool_read_ahead：通过预读(后台线程)读入innodb buffer pool中数据页数

　　2、Innodb_buffer_pool_read_ahead_evicted：通过预读来的数据页没有被查询访问就被清理的pages，无效预读页数