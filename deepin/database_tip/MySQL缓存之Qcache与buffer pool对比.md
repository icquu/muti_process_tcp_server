# [MySQL缓存之Qcache与buffer pool对比](https://www.cnblogs.com/geaozhang/p/7147746.html)

Q：innodb buffer pool和Qcache的缓存区别？

A：

　　1、Qcacche缓存的是SQL语句及对应的结果集，缓存在内存，最简单的情况是SQL一直不重复，那Qcache的命令率肯定是0；

　　2、buffer pool中缓存的是整张表中的数据，缓存在内存，SQL再变只要数据都在内存，那么命中率就是100%。

 

**一、查询缓存（QueryCache）**

1、关于查询缓存机制

　　开启了缓存，会自动将查询语句和结果集返回到内存，下次再查直接从内存中取；

　　查询缓存会跟踪系统中每张表，若表发生变化，则和该张表相关的所有查询缓存全部失效，这是和buffer pool缓存机制很大的区别；

　　检查查询缓存时，MYSQL不会对SQL做任何处理，它精确的使用客户端传来的查询，只要字符大小写或注释有点不同，查询缓存就认为是不同的查询；

　　任何一个包含不确定的函数（比如now()、curren_date()）的查询不会被缓存。

注意：

　　查询缓存可改善性能，但是开启查询缓存对读写增加了额外开销。

　　1、对于读，在查询前需先检查缓存；

　　2、对于写，写入后需更新缓存。

　　一般情况下这些开销相对较小，因此需要根据业务权衡是否开启查询缓存。

2、Qcache参数

```
mysql> show variables like '%query_cache%';
+------------------------------+---------+
| Variable_name                | Value   |
+------------------------------+---------+
| have_query_cache             | YES     |
| query_cache_limit            | 1048576 |
| query_cache_min_res_unit     | 4096    |
| query_cache_size             | 1048576 |
| query_cache_type             | OFF     |
| query_cache_wlock_invalidate | OFF     |
+------------------------------+---------+
```

参数解析：

　　1、have_query_cache：该MySQL是否支持Query Cache；

　　2、query_cache_limit：缓存块大小，超过该大小不会被缓存

　　3、query_cache_min_res_unit：每个qcache最小的缓存空间大小

　　4、query_cache_size：分配给查询缓存的总内存

　　5、query_cache_type：是否开启查询缓存

　　6、query_cache_wlock_invalidate：控制当有锁加在表上的时候，是否先让该表相关的 Query Cache失效

3、配置查询缓存：在配置文件中修改如下参数

　　query_cache_type = on　　#开启查询缓存

　　query_cache_size = 200M　　#分配给查询缓存的总内存，一般建议不超过256M

　　query_cache_limit = 1M　　#限制MySQL存储的最大结果；如果查询的结果比limit大，那么就不会被缓存。

4、监控Qcache使用情况

```
mysql> show status like 'qcache%'; 
+-------------------------+---------+
| Variable_name           | Value   |
+-------------------------+---------+
| Qcache_free_blocks      | 1       |
| Qcache_free_memory      | 1031832 |
| Qcache_hits             | 0       |
| Qcache_inserts          | 0       |
| Qcache_lowmem_prunes    | 0       |
| Qcache_not_cached       | 5476    |
| Qcache_queries_in_cache | 0       |
| Qcache_total_blocks     | 1       |
+-------------------------+---------+
```

解析：

　　1、Qcache_free_blocks ：Query Cache中目前还有多少剩余的blocks。如果该值显示较大，则说明Query Cache中的内存碎片较多了，可能需要寻找合适的机会进行整理。处理办法：mysql> FLUSH QUERY CACHE;清理查询缓存碎片以提高内存使用性能。(该语句不从缓存中移出任何查询)

　　2、Qcache_free_memory：Query Cache 中目前剩余的内存大小

　　3、Qcache_hits：缓存命中次数

　　4、Qcache_inserts：多少次未命中然后插入

> Query Cache命中率 = Qcache_hits /(Qcache_hits + Qcache_inserts)

　　5、Qcache_lowmem_prunes：多少条Query 因为内存不足而被清除出Query

　　6、Qcache_not_cached：因为query_cache_type的设置off或者不能被cache的Query的数量

　　7、Qcache_queries_in_cache：当前Query Cache中cache的Query数量

　　8、Qcache_total_blocks：当前Query Cache中的block数量

注：

　　命中率低，说明没从内存中取，还是从磁盘取，则多走了一步。看是否sql老变还是什么问题；

　　Qcache_not_cached 数值大，开启了查询缓存没有缓存的数据则说明设置缓存的大小太小了，好多无法缓存。

 

**二、存储引擎层-innodb buffer pool**

　　buffer pool是innodb存储引擎带的一个缓存池，查询数据的时候，它首先会从内存中查询，如果内存中存在的话，直接返回，从而提高查询响应时间。Buffer pool是设置的越大越好，一般设置为服务器物理内存的70%。

1、Innodb_buffer_pool参数

```
mysql> show variables like '%innodb_buffer_pool%';
+-------------------------------------+----------------+
| Variable_name                       | Value          |
+-------------------------------------+----------------+
| innodb_buffer_pool_chunk_size       | 134217728      |
| innodb_buffer_pool_dump_at_shutdown | ON             |
| innodb_buffer_pool_dump_now         | OFF            |
| innodb_buffer_pool_dump_pct         | 25             |
| innodb_buffer_pool_filename         | ib_buffer_pool |
| innodb_buffer_pool_instances        | 1              |
| innodb_buffer_pool_load_abort       | OFF            |
| innodb_buffer_pool_load_at_startup  | ON             |
| innodb_buffer_pool_load_now         | OFF            |
| innodb_buffer_pool_size             | 134217728      |
+-------------------------------------+----------------+
```

参数解析：

　　1、innodb_buffer_pool_size：Innodb_buffer_pool的大小

　　2、innodb_buffer_pool_filename ：热数据文件名称

　　3、innodb_buffer_pool_dump_now：默认是OFF；如果开启则立刻InnoDB将InnoDB缓冲池中的热数据保存到本地硬盘。(组合innodb_buffer_pool_load_now使用)

　　4、innodb_buffer_pool_load_now：默认是OFF；如果开启则立刻通过加载数据页预热innodb缓冲池。

　　5、innodb_buffer_pool_load_at_startup：默认是OFF；如果开启该参数，启动MySQL服务时，MySQL将本地热数据加载到InnoDB缓冲池中。

　　6、innodb_buffer_pool_dump_at_shutdown：默认是OFF；如果开启该参数，停止mysq服务时是否自动保存InnoDB buffer pool中热数据。

> Q：如何快速重启使用数据库---Preloading the InnoDB Buffer Pool for Faster Restart
>
> A：
>
> 　　开启innodb_buffer_pool_load_at_startup、innodb_buffer_pool_dump_at_shutdown参数。重启数据库，停止时保存热数据，启动时加载热数据到buffer pool。

2、Innodb_buffer_pool状态

```
mysql> show status like '%innodb_buffer_pool%';
+---------------------------------------+--------------------------------------------------+
| Variable_name                         | Value                                            |
+---------------------------------------+--------------------------------------------------+
| Innodb_buffer_pool_dump_status        | Dumping of buffer pool not started               |
| Innodb_buffer_pool_load_status        | Buffer pool(s) load completed at 170430  7:07:12 |
| Innodb_buffer_pool_resize_status      |                                                  |
| Innodb_buffer_pool_pages_data         | 241                                              |
| Innodb_buffer_pool_bytes_data         | 3948544                                          |
| Innodb_buffer_pool_pages_dirty        | 0                                                |
| Innodb_buffer_pool_bytes_dirty        | 0                                                |
| Innodb_buffer_pool_pages_flushed      | 176                                              |
| Innodb_buffer_pool_pages_free         | 7951                                             |
| Innodb_buffer_pool_pages_misc         | 0                                                |
| Innodb_buffer_pool_pages_total        | 8192                                             |
| Innodb_buffer_pool_read_ahead_rnd     | 0                                                |
| Innodb_buffer_pool_read_ahead         | 0                                                |
| Innodb_buffer_pool_read_ahead_evicted | 0                                                |
| Innodb_buffer_pool_read_requests      | 53710                                            |
| Innodb_buffer_pool_reads              | 201                                              |
| Innodb_buffer_pool_wait_free          | 0                                                |
| Innodb_buffer_pool_write_requests     | 45242                                            |
+---------------------------------------+--------------------------------------------------+
```

　　Innodb_buffer_pool_read_requests　　#逻辑读(缓存读)请求次数，也是读的请求次数

　　Innodb_buffer_pool_reads　　#从物理磁盘中获取到数据的次数

注：

　　逻辑读就是从buffer pool的读，但是也会包含物理读，因为物理读也要是先将从disk中读取的数据放入buffer pool里，然后再进行逻辑读。所以：总的逻辑读也就是读的请求次数。

> 读的命中率=(Innodb_buffer_pool_read_requests- Innodb_buffer_pool_reads)/ Innodb_buffer_pool_read_requests