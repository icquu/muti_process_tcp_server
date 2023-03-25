# [MySQL数据库备份和还原的常用命令](https://www.cnblogs.com/gzgccsu/archive/2012/10/19/2730490.html)

**备份MySQL数据库的命令(-h与hostname，-u与username之间可以有空格，但-p与password之间不能有空格，故一般均无需空格，-R表示同时导出存储过程和函数)**

mysqldump -hhostname -uusername -ppassword -R databasename > backupfile.sql

 

**备份MySQL数据库为带删除表的格式** 备份MySQL数据库为带删除表的格式，能够让该备份覆盖已有数据库而不需要手动删除原有数据库。

mysqldump -hhostname -uusername -ppassword --add-drop-table databasename > backupfile.sql

 

**直接将MySQL数据库压缩备份(需要服务器支持gzip，对windows而言，在系统盘systems32下要有gzip.exe文件)**

mysqldump -hhostname -uusername -ppassword databasename | gzip > backupfile.sql.gz

 

**备份MySQL数据库某个(些)表**

mysqldump -hhostname -uusername -ppassword databasename table1 table2 > backupfile.sql

 

**同时备份多个MySQL数据库**

mysqldump -hhostname -uusername -ppassword –-databases databasename1 databasename2 databasename3 > multibackupfile.sql

 

**仅仅备份数据库结构**

mysqldump -hhostname -uusername -ppassword --no-data --databases databasename1 databasename2 databasename3 > structurebackupfile.sql

 

**备份服务器上所有数据库**

mysqldump -hhostname -uusername -ppassword –all-databases > allbackupfile.sql

 

 

**备份数据库按年月日时分秒自动命名(一般采用系统计划任务来实现自动备份)**

**@echo offset h=%time:~0,2%echo %h%if %h% LSS 10 ( set h=0%time:~1,1%)**

mysqldump -hhostname -uusername -ppassword databasename> c:\databasename%date:~0,4%%date:~5,2%%date:~8,2%%h%%time:~3,2%%time:~6,2%.sql

 

**还原MySQL数据库的命令**

mysql -hhostname -uusername -ppassword databasename < backupfile.sql

 

**还原压缩的MySQL数据库**

gzip -d < backupfile.sql.gz | mysql -hhostname -uusername -ppassword databasename

 

**将数据库转移到新服务器(|后面的hostname为目标机器)**

mysqldump -hhostname -uusername -ppassword databasename | mysql –hhostname -C databasename