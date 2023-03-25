# [MySQL运行状态及调优（一）](https://www.cnblogs.com/gzgccsu/archive/2013/01/23/2873579.html)

一、查看MySQL运行情况
SHOW STATUS;

二、查看INNODB数据库引擎运行状态
SHOW ENGINE INNODB STATUS;

三、查看当前正在进行的进程，对于有锁表等情况的排查很有用处
SHOW PROCESSLIST; 默认显示前100条
SHOW FULL PROCESSLIST; 显示所有

四、查看MySQL的配置参数
SHOW VARIABLES;

五、查看当前已经被打开的表列表
SHOW OPEN TABLES;

注意：一般情况下，打开MySQL的慢查询记录同样有利于排查，如何开启慢查询，[请点击](http://www.cnblogs.com/gzgccsu/archive/2013/01/23/2873598.html)。

附1 SHOW STATUS;关键结果释义
Aborted_clients 由于客户没有正确关闭连接已经死掉，已经放弃的连接数量。
Aborted_connects 尝试已经失败的MySQL服务器的连接的次数。
Connections 试图连接MySQL服务器的次数。
Created_tmp_tables 当执行语句时，已经被创造了的隐含临时表的数量。
Delayed_insert_threads 正在使用的延迟插入处理器线程的数量。
Delayed_writes 用INSERT DELAYED写入的行数。
Delayed_errors 用INSERT DELAYED写入的发生某些错误(可能重复键值)的行数。
Flush_commands 执行FLUSH命令的次数。
Handler_delete 请求从一张表中删除行的次数。
Handler_read_first 请求读入表中第一行的次数。
Handler_read_key 请求数字基于键读行。
Handler_read_next 请求读入基于一个键的一行的次数。
Handler_read_rnd 请求读入基于一个固定位置的一行的次数。
Handler_update 请求更新表中一行的次数。
Handler_write 请求向表中插入一行的次数。
Key_blocks_used 用于关键字缓存的块的数量。
Key_read_requests 请求从缓存读入一个键值的次数。
Key_reads 从磁盘物理读入一个键值的次数。
Key_write_requests 请求将一个关键字块写入缓存次数。
Key_writes 将一个键值块物理写入磁盘的次数。
Max_used_connections 同时使用的连接的最大数目。
Not_flushed_key_blocks 在键缓存中已经改变但是还没被清空到磁盘上的键块。
Not_flushed_delayed_rows 在INSERT DELAY队列中等待写入的行的数量。
Open_tables 打开表的数量。
Open_files 打开文件的数量。
Open_streams 打开流的数量(主要用于日志记载）
Opened_tables 已经打开的表的数量。
Questions 发往服务器的查询的数量。
Slow_queries 要花超过long_query_time时间的查询数量。
Threads_connected 当前打开的连接的数量。
Threads_running 不在睡眠的线程数量。
Uptime 服务器工作了多少秒。

提升性能的建议:
1.如果opened_tables太大,应该把my.cnf中的table_cache变大
2.如果Key_reads太大,则应该把my.cnf中key_buffer_size变大.可以用Key_reads/Key_read_requests计算出cache失败率
3.如果Handler_read_rnd太大,则你写的SQL语句里很多查询都是要扫描整个表,而没有发挥索引的键的作用
4.如果Threads_created太大,就要增加my.cnf中thread_cache_size的值.可以用Threads_created/Connections计算cache命中率
5.如果Created_tmp_disk_tables太大,就要增加my.cnf中tmp_table_size的值,用基于内存的临时表代替基于磁盘的

附2 SHOW PROCESSLIST;结果释义
id列:一个标识，你要kill 一个语句的时候很有用。
user列: 显示当前用户，如果不是root，这个命令就只显示你权限范围内的sql语句。
host列:显示这个语句是从哪个ip 的哪个端口上发出的。可用来追踪出问题语句的用户。
db列:显示这个进程目前连接的是哪个数据库。
command列:显示当前连接的执行的命令，一般就是休眠（sleep），查询（query），连接（connect）。
time列:此这个状态持续的时间，单位是秒。
state列:显示使用当前连接的sql语句的状态，很重要的列，后续会有所有的状态的描述，请注意，state只是语句执行中的某一个状态，一个sql语句，已查询为例，可能需要经过copying to tmp table，Sorting result，Sending data等状态才可以完成。
info列:显示这个sql语句，因为长度有限，所以长的sql语句就显示不全，但是一个判断问题语句的重要依据。

**state列具体释义**：
Checking table 正在检查数据表（这是自动的）。
Closing tables 正在将表中修改的数据刷新到磁盘中，同时正在关闭已经用完的表。这是一个很快的操作，如果不是这样的话，就应该确认磁盘空间是否已经满了或者磁盘是否正处于重负中。
Connect Out 复制从服务器正在连接主服务器。
Copying to tmp table on disk 由于临时结果集大于 tmp_table_size，正在将临时表从内存存储转为磁盘存储以此节省内存。
Creating tmp table 正在创建临时表以存放部分查询结果。
deleting from main table 服务器正在执行多表删除中的第一部分，刚删除第一个表。
deleting from reference tables 服务器正在执行多表删除中的第二部分，正在删除其他表的记录。
Flushing tables 正在执行 FLUSH TABLES，等待其他线程关闭数据表。
Killed 发送了一个kill请求给某线程，那么这个线程将会检查kill标志位，同时会放弃下一个kill请求。MySQL会在每次的主循环中检查kill标志位，不过有些情况下该线程可能会过一小段才能死掉。如果该线程程被其他线程锁住了，那么kill请求会在锁释放时马上生效。
Locked 被其他查询锁住了。
Sending data 正在处理 SELECT 查询的记录，同时正在把结果发送给客户端。
Sorting for group 正在为 GROUP BY 做排序。
Sorting for order 正在为 ORDER BY 做排序。
Opening tables 这个过程应该会很快，除非受到其他因素的干扰。例如，在执 ALTER TABLE 或 LOCK TABLE 语句行完以前，数据表无法被其他线程打开。 正尝试打开一个表。
Removing duplicates 正在执行一个 SELECT DISTINCT 方式的查询，但是MySQL无法在前一个阶段优化掉那些重复的记录。因此，MySQL需要再次去掉重复的记录，然后再把结果发送给客户端。
Reopen table 获得了对一个表的锁，但是必须在表结构修改之后才能获得这个锁。已经释放锁，关闭数据表，正尝试重新打开数据表。
Repair by sorting 修复指令正在排序以创建索引。
Repair with keycache 修复指令正在利用索引缓存一个一个地创建新索引。它会比 Repair by sorting 慢些。
Searching rows for update 正在讲符合条件的记录找出来以备更新。它必须在 UPDATE 要修改相关的记录之前就完成了。
Sleeping 正在等待客户端发送新请求.
System lock 正在等待取得一个外部的系统锁。如果当前没有运行多个 mysqld 服务器同时请求同一个表，那么可以通过增加 –skip-external-locking参数来禁止外部系统锁。
Upgrading lock INSERT DELAYED 正在尝试取得一个锁表以插入新记录。
Updating 正在搜索匹配的记录，并且修改它们。
User Lock 正在等待 GET_LOCK()。
Waiting for tables 该线程得到通知，数据表结构已经被修改了，需要重新打开数据表以取得新的结构。然后，为了能的重新打开数据表，必须等到所有其他线程关闭这个表。

以下几种情况下会产生这个通知：FLUSH TABLES tbl_name, ALTERTABLE, RENAME TABLE, REPAIR TABLE, ANALYZE TABLE, 或 OPTIMIZE TABLE。
waiting for handler insert INSERT DELAYED 已经处理完了所有待处理的插入操作，正在等待新的请求。 大部分状态对应很快的操作，只要有一个线程保持同一个状态好几秒钟，那么可能是有问题发生了，需要检查一下。
还有其它的状态没在上面中列出来，不过它们大部分只是在查看服务器是否有存在错误是才用得着。