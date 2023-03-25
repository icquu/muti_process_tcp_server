# [MySQL buffer pool中的三种链](https://www.cnblogs.com/geaozhang/p/7276802.html)

> [三种page](https://www.cnblogs.com/geaozhang/p/7276802.html#page)、[三种list](https://www.cnblogs.com/geaozhang/p/7276802.html#list)、[LRU控制调优](https://www.cnblogs.com/geaozhang/p/7276802.html#tiaoyou)

**一、innodb buffer pool中的三种页**

1、free page：从未用过的页

2、clean page：干净的页，数据页的数据和磁盘一致

3、dirty page：脏页

SQL执行需求：

　　1、找free页

　　2、刷新脏页

　　　　1、这个页不是热的数据页(刷冷页)

　　　　2、这个页最早修改时间(刷修改时间比较早的页，有可能是热页)，方便日志文件的覆盖

　　3、覆盖冷的clean页

> 为了实现上述需求，innodb用到链表技术(每种链表一种作用，链的存在意义是为了遍历)。

 

**二、innodb buffer pool中的三种链**

1、free list：将free数据页使用链表链起来

　　数据库刚启动的时候，lru列表为空，此时需要用到的时候直接将free列表中的页删除，在lru列表中增加相应的页，维持页数守恒。

2、lru list：根据冷热将clean、dirty链起来

　　least recent used(最近最少使用)

　　1、“中点插入策略”

　　2、回写尽量回写冷的脏块

　　3、覆盖尽量覆盖冷的脏块

> LRU标准算法：
>
> 　　1）3/8的list信息是作为old list，这些信息是被驱逐的对象。
>
> 　　2）list的中点就是我们所谓的old list头部和new list尾部的连接点，相当于一个界限。
>
> 　　3）新数据的读入首先会插入到old list的头部。
>
> 　　4）如果是old list的数据被访问到了，这个页信息就会变成new list，变成young page，就会将数据页信息移动到new sublist的头部。
>
> 　　5）在数据库的buffer pool里面，不管是new sublist还是old sublist的数据如果不会被访问到，最后都会被移动到list的尾部作为牺牲者。

3、flush list：将页按照最早脏时间链起来

　　flush list中的也全都是脏页，刷盘即将flush list中的脏页刷新回磁盘中。

　　1、将非常旧的脏块回写到磁盘，按照新旧回写数据页；

　　2、因为是从最早脏的块开始刷，这样logfile里的对应的日志就可以被覆盖了。

![img](https://images2017.cnblogs.com/blog/1113510/201708/1113510-20170802203306006-1039539798.png)

Q：为什么需要这三种链 ？
A：

　　因为在innodb 缓冲池中，内存管理如下：
　　1、需要经常找 free 空闲数据块：free list。
　　2、需要经常找冷的数据块：lru list（最近最少使用，根据冷热链起来）。
　　3、需要知道哪些数据块是比较早脏的，flush list：我们要覆盖旧的 logfile，就需要系统将这些 logfile 对应的脏块，即将 flush list 链上的脏页往磁盘上刷，（批量往磁盘写的时候，不如刷脏页）。

 

**三、LRU冷热区控制及调整**

1、设置冷热分界线：innodb_old_blocks_pct

2、成为热块的所需时间：innodb_old_blocks_time

```
mysql> show variables like '%old_blocks%';
+------------------------+-------+
| Variable_name          | Value |
+------------------------+-------+
| innodb_old_blocks_pct  | 37    |
| innodb_old_blocks_time | 1000  |
+------------------------+-------+
2 rows in set (0.01 sec)
```

　　通过 innodb_old_blocks_pct 参数值得设定分为两部分：一是存放长时间未被访问的数据页，二是存放最近被访问的数据页。靠近 LRU 链表头部的数据页表示最近经常被访问，靠近尾部表示数据页长期未被访问，这两个部分的交汇处称为 midpoint，即 innodb_old_blocks_pct这个点的设置。默认是37%，最小是5，最大是95；如果内存比较大的话，可以将这个数值调低，通常会调成20，也就是说20%的是冷数据块。目的是为了保护热区数据不被刷出内存。

　　通过innodb_old_blocks_time参数来控制成为热数据的所需时间，默认是1000ms，也就是1s，也就是数据在1s内没有被刷走，就调入热区。

3、LRU冷热数据的监控

```
mysql> show engine innnodb status\G
……
Pages made young 0, not young 0
0.00 youngs/s, 0.00 non-youngs/s
```

　　1、数据页从冷到热，称为young；not young就是数据在没有成为热数据情况下就被刷走的量(累计值)。

　　2、non-youngs/s，这个数值如果很高，一般情况下就是系统存在严重的全表扫描，自然意味着很高的物理读。

　　3、youngs/s，如果这个值相对较高，最好增加一个innodb_old_blocks_time，降低innodb_old_blocks_pct，保护热数据。