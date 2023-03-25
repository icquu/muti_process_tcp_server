# [SELECT中的if_case流程函数](https://www.cnblogs.com/geaozhang/p/6743722.html)

DQL中常用的流程函数if_case

　　---流程函数在一个SQL语句中实现条件选择

![img](https://images2015.cnblogs.com/blog/1113510/201704/1113510-20170421145156618-1994222761.png)

 

模拟对职员薪水进行分类：

```
mysql> create table salary_tab(userid int not null primary key,salary dec(9,2));
Query OK, 0 rows affected (0.04 sec)

mysql> insert into salary_tab values(1,1000);
mysql> insert into salary_tab values(2,2000);
mysql> insert into salary_tab values(3,3000);
mysql> insert into salary_tab(userid) values(4);
mysql> insert into salary_tab values(5,1000);

mysql> select * from salary_tab;
+--------+---------+
| userid | salary  |
+--------+---------+
|      1 | 1000.00 |
|      2 | 2000.00 |
|      3 | 3000.00 |
|      4 |    NULL |
|      5 | 1000.00 |
+--------+---------+
5 rows in set (0.00 sec)
```

 

1、IF(expr1,expr2,expr3)

　　判断第一个参数expr1为TRUE (expr1 <> 0 and expr1 <> NULL)：

　　　　成功，返回第二个参数expr2

　　　　失败，返回第三个参数expr3

返回值可以是数字、字符串、列值

```
mysql> select if(null,'非空','空');
+-------------------------+
| if(null,'非空','空')    |
+-------------------------+
| 空                      |
+-------------------------+

mysql> select userid,if(salary>2000,'high','low') from salary_tab;
+--------+------------------------------+
| userid | if(salary>2000,'high','low') |
+--------+------------------------------+
|      1 | low                          |
|      2 | low                          |
|      3 | high                         |
|      4 | low                          |
|      5 | low                          |
+--------+------------------------------+

mysql> select if(1>2,2,3),if(1<2,'yes','no');
+-------------+--------------------+
| if(1>2,2,3) | if(1<2,'yes','no') |
+-------------+--------------------+
|           3 | yes                |
+-------------+--------------------+
```

 

 

2、IFNULL(expr1,expr2)

　　判断第一个参数expr1是否为NULL：

　　　　如果expr1不为空，直接返回expr1；

　　　　如果expr1为空，返回第二个参数 expr2   

常用在算术表达式计算和组函数中，用来对null值进行转换处理(返回值是数字或者字符串)

```
mysql> select ifnull(salary,0) from salary_tab;
+------------------+
| ifnull(salary,0) |
+------------------+
|          1000.00 |
|          2000.00 |
|          3000.00 |
|             0.00 |
|          1000.00 |
+------------------+

mysql> select ifnull(1/0,'yes');
+-------------------+
| ifnull(1/0,'yes') |
+-------------------+
| yes               |
+-------------------+
```

 

NULLIF(expr1,expr2)：如果两个参数相等则返回NULL，否则返回第一个参数的值expr1

```
mysql> select nullif(1,1),nullif(123,321);
+-------------+-----------------+
| nullif(1,1) | nullif(123,321) |
+-------------+-----------------+
|        NULL |             123 |
+-------------+-----------------+
```

 

 

3、在SQL语句中实现“if-then-else”逻辑计算功能

　　有两种形式：simple case和searched case

1）simple case的语法结构：

```
CASE  value

    WHEN  [compare_value] THEN  result

    [WHEN [compare_value] THEN  result ...] 

    [ELSE  result]  END
```

语义：

　　将case后面的值value分别和每个when子句后面的值compare_value进行相等比较：

　　　　如果一旦和某个when子句后面的值相等则返回相应的then子句后面的值result；

　　　　如果和所有when子句后面的值都不相等，则返回else子句后面的值；

　　　　如果没有else部分则返回null。

注意：

　　①value可以是字面量、表达式或者列名

　　②CASE表达式的数据类型取决于跟在then或else后面的表达式的类型

类型必须相同（可尝试做隐式类型转换），否则出错

```
mysql> select userid,case salary                                             
    -> when 1000 then 'low'
    -> when 2000 then 'med'
    -> when 3000 then 'high'
    -> else '无效值' end salary_grade
    -> from salary_tab;
+--------+--------------+
| userid | salary_grade |
+--------+--------------+
|      1 | low          |
|      2 | med          |
|      3 | high         |
|      4 | 无效值        |
|      5 | low          |
+--------+--------------+
```

 

2）searched  case的语法结构：

```
CASE

    WHEN [condition] THEN result

    [WHEN [condition] THEN result ...]

    [ELSE result]  END
```

语义：

　　如果某个when子句后面的条件condition为true，则返回相应的when子句后面的值result；

　　如果所有的when子句后面的条件condition都不为true，则返回else子句后面的值；

　　如果没有else部分则返回null。

```
mysql> select userid,case
    -> when salary<=1000 then 'low'
    -> when salary=2000 then 'med'
    -> when salary>=3000 then 'high'
    -> else '无效值' end salary_grade
    -> from salary_tab;
+--------+--------------+
| userid | salary_grade |
+--------+--------------+
|      1 | low          |
|      2 | med          |
|      3 | high         |
|      4 | 无效值        |
|      5 | low          |
+--------+--------------+
```