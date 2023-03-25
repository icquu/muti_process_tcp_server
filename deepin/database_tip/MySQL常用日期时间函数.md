# [MySQL常用日期时间函数](https://www.cnblogs.com/geaozhang/p/6740457.html)

日期和时间函数

可能的需求：

　　当前时间是多少、下个月的今天是星期几、统计截止到当前日期前 3 天的收入总和……

上述需求就需要使用日期和时间函数来实现：

![img](https://images2015.cnblogs.com/blog/1113510/201704/1113510-20170420184722368-1301712247.png)

MySQL服务器中的三种时区设置：

　　①系统时区---保存在系统变量system_time_zone

　　②服务器时区---保存在全局系统变量global.time_zone

　　③每个客户端连接的时区---保存在会话变量session.time_zone

注意：

　　客户端时区的设置会影响一些日期函数返回值的显示，例如：now()、curtime()、curdate()，也影响timestamp列值的显示。

　　默认情况下，客户端和服务器的时区相同，其值为SYSTEM，表示使用系统时区。 

```
mysql> select @@global.time_zone,@@session.time_zone;
+--------------------+---------------------+
| @@global.time_zone | @@session.time_zone |
+--------------------+---------------------+
| SYSTEM             | SYSTEM              |
+--------------------+---------------------+
1 row in set (0.00 sec)

mysql> show variables like 'system_time_zone';
+------------------+-------+
| Variable_name    | Value |
+------------------+-------+
| system_time_zone | CST   |
+------------------+-------+
1 row in set (0.28 sec)
```

 

 

1、NOW([fsp])：返回服务器的当前日期和时间(fsp指定小数秒的精度，取值0--6)

格式：

　　‘YYYY-MM-DD HH:MM:SS’或者‘YYYYMMDDHHMMSS’

```
now()的显示格式是‘YYYY-MM-DD HH:MM:SS’
now()+0的显示格式是‘YYYYMMDDHHMMSS’
```

------

```
mysql> select now();
+---------------------+
| now()               |
+---------------------+
| 2017-03-24 13:53:34 |
+---------------------+

mysql> select now()+0;
+----------------+
| now()+0        |
+----------------+
| 20170324135428 |
+----------------+
```

　mysql> select now(6);　　//指定小数秒的精度
　+----------------------------+
　| now(6) |
　+----------------------------+
　| 2017-04-19 19:55:46.658198 |
　+----------------------------+

now()函数的同义词有：CURRENT_TIMESTAMP 、 CURRENT_TIMESTAMP()、LOCALTIMESTAMP 、 LOCALTIMESTAMP()、LOCALTIME 、 LOCALTIME()

注意：

　　SYSDATE( )：返回服务器的当前日期和时间

与now的不同点：(一般使用NOW而不用SYSDATE)

　　①SYSDATE()返回的是函数执行时的时间

　　②now()返回的是语句执行时的时间

```
mysql> select now(),sleep(2),now();
+---------------------+----------+---------------------+
| now()               | sleep(2) | now()               |
+---------------------+----------+---------------------+
| 2017-04-19 20:01:39 |        0 | 2017-04-19 20:01:39 |
+---------------------+----------+---------------------+
1 row in set (2.00 sec)

mysql> select sysdate(),sleep(2),sysdate();
+---------------------+----------+---------------------+
| sysdate()           | sleep(2) | sysdate()           |
+---------------------+----------+---------------------+
| 2017-04-19 20:02:01 |        0 | 2017-04-19 20:02:03 |
+---------------------+----------+---------------------+
1 row in set (2.05 sec)
```

 

 

2、CURTIME([fsp])：返回当前时间，只包含时分秒(fsp指定小数秒的精度，取值0--6)

格式：

　　‘YYYY-MM-DD HH:MM:SS’或者‘YYYYMMDDHHMMSS’

```
mysql> select curtime(),curtime(2);
+-----------+-------------+
| curtime() | curtime(2)  |
+-----------+-------------+
| 14:35:23  | 14:35:23.90 |
+-----------+-------------+
```

同义词有：CURRENT_TIME 、 CURRENT_TIME() 

 

 

3、CURDATE()：返回当前日期，只包含年月日

格式：

　　‘YYYY-MM-DD’或者‘YYYYMMDD’

```
mysql> select curdate(),curdate()+2;
+------------+-------------+
| curdate()  | curdate()+2 |
+------------+-------------+
| 2017-03-24 |    20170326 |
+------------+-------------+

mysql> select curdate(),curdate()+0;
+------------+-------------+
| curdate()  | curdate()+0 |
+------------+-------------+
| 2017-03-24 |    20170324 |
+------------+-------------+
```

同义词有： CURRENT_DATE 、CURRENT_DATE()

 

 

4、TIMEDIFF(expr1, expr2)：返回两个日期相减（expr1 − expr2 ）相差的时间数（两个参数类型必须相同）

```
mysql> select timediff('18:32:59','60000');
+------------------------------+
| timediff('18:32:59','60000') |
+------------------------------+
| 12:32:59                     |
+------------------------------+


mysql> select timediff('18:32:59','2017-1-1 60000');
+---------------------------------------+
| timediff('18:32:59','2017-1-1 60000') |
+---------------------------------------+
| NULL                                  |
+---------------------------------------+
```

 

DATEDIFF(expr1, expr2)：返回两个日期相减（expr1 − expr2 ）相差的天数

```
mysql> select datediff('2017-3-24 18:32:59','2016-9-1');
+-------------------------------------------+
| datediff('2017-3-24 18:32:59','2016-9-1') |
+-------------------------------------------+
|                                       204 |
+-------------------------------------------+
```

 

 

5、日期时间运算函数：分别为给定的日期date加上(add)或减去(sub)一个时间间隔值expr

格式：

　　DATE_ADD(date, INTERVAL  expr  unit);

　　DATE_SUB(date, INTERVAL  expr  unit);

interval是间隔类型关键字

expr是一个表达式，对应后面的类型

unit是时间间隔的单位(间隔类型)（20个），如下：

| HOUR          | 小时     |
| ------------- | -------- |
| MINUTE        | 分       |
| SECOND        | 秒       |
| MICROSECOND   | 毫秒     |
| YEAR          | 年       |
| MONTH         | 月       |
| DAY           | 日       |
| WEEK          | 周       |
| QUARTER       | 季       |
| YEAR_MONTH    | 年和月   |
| DAY_HOUR      | 日和小时 |
| DAY_MINUTE    | 日和分钟 |
| DAY_ SECOND   | 日和秒   |
| HOUR_MINUTE   | 小时和分 |
| HOUR_SECOND   | 小时和秒 |
| MINUTE_SECOND | 分钟和秒 |

```
mysql> select now(),date_add(now(),interval 1 day);　　#加一天
+---------------------+--------------------------------+
| now()               | date_add(now(),interval 1 day) |
+---------------------+--------------------------------+
| 2017-03-24 14:53:08 | 2017-03-25 14:53:08            |
+---------------------+--------------------------------+

mysql> SELECT date_sub('2005-01-01 00:00:00',INTERVAL '1 1:1:1' DAY_SECOND);　　#减1天1小时1分1秒
+---------------------------------------------------------------+
| date_sub('2005-01-01 00:00:00',INTERVAL '1 1:1:1' DAY_SECOND) |
+---------------------------------------------------------------+
| 2004-12-30 22:58:59                                           |
+---------------------------------------------------------------+
```

 

不使用函数，也可以写表达式进行日期的加减：

　　date  + INTERVAL  expr  unit

　　date  - INTERVAL  expr  unit

```
mysql> SELECT '2008-12-31 23:59:59' + INTERVAL 1 SECOND;
+-------------------------------------------+
| '2008-12-31 23:59:59' + INTERVAL 1 SECOND |
+-------------------------------------------+
| 2009-01-01 00:00:00                       |
+-------------------------------------------+
1 row in set (0.00 sec)

mysql> SELECT '2005-01-01' - INTERVAL 1 SECOND;
+----------------------------------+
| '2005-01-01' - INTERVAL 1 SECOND |
+----------------------------------+
| 2004-12-31 23:59:59              |
+----------------------------------+
1 row in set (0.00 sec)
```

 

 

6、选取日期时间的各个部分：日期、时间、年、季度、月、日、小时、分钟、秒、微秒（常用）

SELECT now(),date(now()); -- 日期

SELECT now(),time(now()); -- 时间

SELECT now(),year(now()); -- 年

SELECT now(),quarter(now()); -- 季度

SELECT now(),month(now()); -- 月

SELECT now(),week(now()); -- 周

SELECT now(),day(now()); -- 日

SELECT now(),hour(now()); -- 小时

SELECT now(),minute(now()); -- 分钟

SELECT now(),second(now()); -- 秒

SELECT now(),microsecond(now()); -- 微秒

 

EXTRACT(unit  FROM  date)：从日期中抽取出某个单独的部分或组合

SELECT now(),extract(YEAR FROM now()); -- 年

SELECT now(),extract(QUARTER FROM now()); -- 季度

SELECT now(),extract(MONTH FROM now()); -- 月

SELECT now(),extract(WEEK FROM now()); -- 周

SELECT now(),extract(DAY FROM now()); -- 日

SELECT now(),extract(HOUR FROM now()); -- 小时

SELECT now(),extract(MINUTE FROM now()); -- 分钟

SELECT now(),extract(SECOND FROM now()); -- 秒

SELECT now(),extract(YEAR_MONTH FROM now()); -- 年月

SELECT now(),extract(HOUR_MINUTE FROM now()); -- 时分

  

 

7、个性化显示时间日期

　　dayofweek(date)

　　dayofmonth(date)

　　dayofyear(date)

\##分别返回日期在一周、一月、一年中是第几天

```
mysql> SELECT now(),dayofweek(now());
+---------------------+------------------+
| now()               | dayofweek(now()) |
+---------------------+------------------+
| 2017-04-19 20:25:41 |                4 |
+---------------------+------------------+
1 row in set (0.00 sec)

mysql> SELECT now(),dayofmonth(now());
+---------------------+-------------------+
| now()               | dayofmonth(now()) |
+---------------------+-------------------+
| 2017-04-19 20:25:51 |                19 |
+---------------------+-------------------+
1 row in set (0.03 sec)

mysql> select now(),dayofyear(now());
+---------------------+------------------+
| now()               | dayofyear(now()) |
+---------------------+------------------+
| 2017-04-19 20:26:00 |              109 |
+---------------------+------------------+
1 row in set (0.00 sec)
```

　　

　　dayname()

　　monthname()

\##分别返回日期的星期和月份名称

名称是中文or英文的由系统变量lc_time_names控制(默认值是'en_US')

```
mysql> show variables like 'lc_time_names';
+---------------+-------+
| Variable_name | Value |
+---------------+-------+
| lc_time_names | en_US |
+---------------+-------+
1 row in set (0.00 sec)

mysql> select dayname(now()),monthname(now());
+----------------+------------------+
| dayname(now()) | monthname(now()) |
+----------------+------------------+
| Wednesday      | April            |
+----------------+------------------+
1 row in set (0.00 sec)

mysql> set lc_time_names='zh_CN';
Query OK, 0 rows affected (0.00 sec)

mysql> select dayname(now()),monthname(now());
+----------------+------------------+
| dayname(now()) | monthname(now()) |
+----------------+------------------+
| 星期三         | 四月             |
+----------------+------------------+
1 row in set (0.00 sec)
```