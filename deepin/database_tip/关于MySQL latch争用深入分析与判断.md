# [关于MySQL latch争用深入分析与判断](https://www.cnblogs.com/geaozhang/p/7287108.html)

> 1、[latch锁是什么锁](https://www.cnblogs.com/geaozhang/p/7287108.html#dingyi)？
>
> 2、[latch锁是如何保护list](https://www.cnblogs.com/geaozhang/p/7287108.html#ruhebaohu)？
>
> 3、[latch争用的现象和过程](https://www.cnblogs.com/geaozhang/p/7287108.html#xianxiangguocheng)？
>
> 4、[latch什么时候会产生严重的争用](https://www.cnblogs.com/geaozhang/p/7287108.html#shihou)？
>
> 5、[如何监控latch争用情况](https://www.cnblogs.com/geaozhang/p/7287108.html#jiankong)？
>
> 6、[如何确认latch争用类型](https://www.cnblogs.com/geaozhang/p/7287108.html#queren)？
>
> 7、[如何降低latch争用](https://www.cnblogs.com/geaozhang/p/7287108.html#jiangdi)？

**一、latch锁是什么锁**

1、定义

　　latch锁是内存锁，是一个小型的在内存中保护list的内存锁结构。

2、特点

　　1、不排队

　　2、spin，一个线程想获得一个锁，但是该锁已被另一线程持有，进行spin(空转随机时间)占用cpu间接性的等待锁的释放，然后获取去进行相关操作。

　　3、os waits：sleep，spin多次仍然spin

　　4、cpu繁忙，latch争用

>  Q：什么是锁？
>
> A：
>
> 　　1、用来保护共享资源，支持并发
>
> 　　2、锁会影响并发
>
> 　　3、latch锁、lock锁

 

**二****、latch锁是如何保护list**

1、“保护”过程分析

　　1、访问页先需要访问链

　　2、修改list不等于修改页

　　3、什么时候修改list

　　　　1、物理读，将数据页挂到list上

　　　　2、内存读、修改数据页，修改链

　　4、锁，其实就是一个内存空间，有结构有数据的内存数据块

　　　　s：R共享锁

　　　　x：W排它锁

　　5、锁的兼容性

　　　　1、但凡有x锁，排它，就不兼容。

　　　　2、latch锁排它就会造成latch争用。

注：mutex互斥锁：针对并发量不是很大的资源。

2、原理图分析

 ![img](https://images2017.cnblogs.com/blog/1113510/201708/1113510-20170804202040256-1399959073.png)

 

**三、latch争用的现象和过程**

1、latch争用现象

　　1、latch争用会表现为cpu繁忙

　　2、latch争用没有排队，等一段随机的时间再回来看一看

2、latch争用过程

　　1、链上有一个链的保护机制latch，小内存结构；

　　2、这时候有读的线程A上来要读取链，这个时候这个管理就变成r(读锁)，当在链上找到数据页的时候(读)，一找到就释放读锁；

　　3、B上来也要读取，这个时候一看是r，读锁是可以共享的，它也对链进行访问读取；

　　4、C上来要修改链中的两个块的内容，一看是r，r和w是互斥的，不能同时进行，要么

　　　　1、主动要求退出cpu；

　　　　2、空占着cpu资源(执行一段空代码，loop，隔一段时间看看A和B有没有使用完(spin)，但是在这个过程中因为C没有排队等待，所以可能在等待的过程中又有其他的线程上来霸占链(不排队的坏处)，如果执行多次仍这样，可能就sleep，退出cpu了，sleep，产生os waits)。

　　5、为什么空占(害怕os看它闲着把它强行拖出去)

　　6、等(因为它知道A和B占用资源时间比较短，就是遍历一条链的时间非常短)

 

**四****、latch什么时候会产生严重的争用**

1、异常SQL：往往意味着latch争用

　　大量的物理读：修改链

　　大量的内存读：遇到修改链的冲突

2、内存访问频繁（不停找），其实也是异常SQL造成的。

3、list太长

　　链上挂10000个块，被持有的几率太大……

```
mysql> show variables like 'i%instances';
+------------------------------+-------+
| Variable_name                | Value |
+------------------------------+-------+
| innodb_buffer_pool_instances | 1     |
+------------------------------+-------+
1 row in set (0.00 sec)
```

　　所以，有时候会增加instance的数量，把大pool切成小的pool，让list链变的短一些。

 

**五****、如何监控latch争用情况**

1、对于MySQL 5.7

```
mysql> show engine innodb status\G
……
SEMAPHORES
----------
OS WAIT ARRAY INFO: reservation count 23
OS WAIT ARRAY INFO: signal count 14
RW-shared spins 0, rounds 73, OS waits 5
RW-excl spins 0, rounds 1114, OS waits 5
RW-sx spins 0, rounds 0, OS waits 0
Spin rounds per wait: 73.00 RW-shared, 1114.00 RW-excl, 0.00 RW-sx
```

　　rounds：表示spin一次空转多少圈，也就是返回来询问的次数

　　OS waits：表示sleep，当突然增长比较快时，说明latch争用比较严重

　　1、如果OS waits值比较高，说明出现latch争用，异常SQL

　　2、获取latch的代价：73.00 RW-shared, 1114.00 RW-excl

2、对于MySQL 5.6

```
mysql> show engine innodb status\G
……
SEMAPHORES
----------
OS WAIT ARRAY INFO: reservation count 29758
OS WAIT ARRAY INFO: signal count 29148
Mutex spin waits 1054508, rounds 427812, OS waits 2104
RW-shared spins 26703, rounds 800527, OS waits 26673
RW-excl spins 68, rounds 27115, OS waits 888
Spin rounds per wait: 0.41 mutex, 29.98 RW-shared, 398.75 RW-excl
```

　　Mutex spin waits：可以理解成misses，空转cpu

　　Mutex是互斥锁、RW-shared是共享锁、RW-excl是排它锁

 

**六****、确认latch争用类型**

```
mysql> show engine innodb mutex;
+--------+-----------------------------+----------+
| Type   | Name                        | Status   |
+--------+-----------------------------+----------+
| InnoDB | rwlock: dict0dict.cc:2687   | waits=1  |
| InnoDB | rwlock: dict0dict.cc:1184   | waits=13 |
| InnoDB | rwlock: log0log.cc:844      | waits=35 |
| InnoDB | sum rwlock: buf0buf.cc:1457 | waits=4  |
+--------+-----------------------------+----------+
4 rows in set (0.16 sec)
```

　　利用show engine innodb mutex;来解决latch和mutex问题。

```
[root@localhost ~]# find / -name dict0dict.cc
/usr/src/debug/percona-xtrabackup-2.4.4/storage/innobase/dict/dict0dict.cc
/usr/local/src/mysql-5.7.14/storage/innobase/dict/dict0dict.cc
[root@localhost ~]# cat /usr/local/src/mysql-5.7.14/storage/innobase/dict/dict0dict.cc　　　　#查看源码信息，看其latch争用类型具体描述
```

 

**七****、如何降低latch争用**

1、优化SQL，降低对内存读的数量，效果比较明显。

2、增加innodb_buffer_pool_instances的数量。对于具有大内存的64位系统，可以将缓冲池拆分成多个实例（默认8个），把需要缓冲的数据hash到不同的缓冲池中，这样可以并行的内存读写，以最大限度地减少并发操作中内存结构的争用。