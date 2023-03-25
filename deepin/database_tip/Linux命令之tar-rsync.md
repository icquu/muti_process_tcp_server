# [Linux命令之tar-rsync](https://www.cnblogs.com/geaozhang/p/6719109.html)

一、tar命令

​          可以对文件和目录进行打包压缩(相较于zip、gzip、bzip2不能对目录进行压缩，tar是一大优势)

用途：制作归档文件、释放归档文件

基本格式：

　　压缩---> tar  [选项]...  归档文件名  源文件或目录

​       解压---> tar  [选项]...  归档文件名  [-C 目标目录]

常用命令选项：

　　-c：创建 .tar 格式的包文件 --create

　　-x：解开 .tar 格式的包文件 --extract

　　-v：输出详细信息 --verbose

　　-f：表示使用归档文件 --file 

　　-t：列表查看包内的文件 --list（list the contents of an archive） 

　　-p：保持原文件的原来属性 --preserve-permissions 

　　-P：保持原文件的绝对路径 --absolute-names

​                 （ don't  strip  leading  ‘/’s  from  file  names）

=======================================================

打包（接上面选项）并压缩（接下面选项调用压缩）

　　-j --调用bzip2

　　-z --调用gzip

　　-J --调用xz

 

所以：创建压缩包基本语法

　　tar  -cjf  归档文件名  源目录或文件  ---> tar.bz2

　　tar  -czf  ……  ---> tar.gz

　　tar  -cJf  ……  ---> tar.xz （目前效果是最好的）www.kernel.org里的压缩包都是tar.xz

\###########压缩效果：xz-->bzip2-->gz

 

======查看：tar  tf  归档文件名

======解压：tar  xf  归档文件名

 

 具体示例：

1、压缩-----创建压缩文件的时候，使用绝对路径指定创建压缩文件的路径

[root@wulimengzhu lianxi]# ls

messages  passwd

[root@wulimengzhu lianxi]# tar -czvf messages_passwd.tar.gz  messages  passwd                     

messages

passwd

//同时将2个文件打包成messages_passwd.tar.gz ,此文件存放在当前目录下

[root@wulimengzhu lianxi]# ls

messages  messages_passwd.tar.gz  passwd

 

[root@wulimengzhu lianxi]# tar -tf messages_passwd.tar.gz　　//利用-tf选项查看压缩包里的内容

messages

passwd

（再次打包文件，如果文件名相同，存放的路径也相同，会替换原来的文件）

 

[root@wulimengzhu lianxi]# tar  -czf  /backup/messages_passwd.tar.gz  messages  passwd

//指定压缩打包文件存放的路径在/backup目录下，不指定默认情况下在当前目录

 

==========保持原文件的绝对路径 -P(不建议使用)=================

 

[root@studying tmp]# tar -czf p_h.tar.gz  -P /etc/hosts /etc/passwd

[root@studying tmp]# ls

p_h.tar.gz

[root@studying tmp]# tar -tf p_h.tar.gz 

tar: Removing leading `/' from member names

/etc/hosts

/etc/passwd

 

此时解压缩，备份数据会覆盖原来/etc下面的数据

[root@studying tmp]# rm -f p_h.tar.gz 

[root@studying tmp]# tar -czf p_h.tar.gz /etc/hosts /etc/passwd

tar: Removing leading `/' from member names

[root@studying tmp]# ls

p_h.tar.gz

[root@studying tmp]# tar -tf p_h.tar.gz 

etc/hosts

etc/passwd

[root@studying tmp]# 

 

建议:

　　不要加-P这个选项来备份，毕竟很多时候，备份是为了要未来追踪问题用的，倒不一定需要还原回原来的系统中，remove根目录后，备份数据会比较安全。

==============================

​    

2、解压-----可以解压.tar.gz  .tar.bz2 .tar.xz 的压缩包

　　（解压文件的时候，解压出来的文件默认情况下放在当前目录，如果当前目录下有相同的文件，后来解压出来的文件会替换原来的文件）

 示例：

[root@studying bak]# ls

messages.tar.bz2

[root@studying bak]# tar xf messages.tar.bz2

//解压messages.tar.bz2文件，存放在当前目录下

[root@studying bak]# tar xvf messages.tar.bz2 

messages

[root@studying bak]# ls

messages  messages.tar.bz2

\#######################

默认是解压在当前工作目录下

使用-C 指定解压目录

\#######################

[root@studying bak]# tar xf messages.tar.bz2  -C  /linshi/

//将当前目录下的messages.tar.bz2解压到/linshi目录下

[root@studying bak]# ls /linshi

messages

  

=================================================================

打包备份文件的时候，使用绝对路径会有相关提醒，例如：tar从成员名中删除开头的“/”。

shell> tar czf  /lianxi/bak/passwd_hosts.tar.gz  /etc/passwd   /etc/hosts

tar: Removing leading `/' from member names

shell> tar tf passwd_hosts.tar.gz

etc/passwd

etc/hosts

shell> tar xf passwd_hosts.tar.gz 

shell> ls etc/

hosts  passwd

因为解压的时候，不指定路径，就直接解压到当前目录，如果因为打包压缩时带了选项P的话，带着/，解压的时候，显示原来文件的绝对路径，可能的替换和误删除误操作，会导致原来的文件丢失，不是我们希望看到的。

=================================================================

 

3、排除某一或多文件的打包备份

​              使用--exclude 选项，不包含某个文件（include 包含）

示例：

[root@wulimengzhu lianxi]# ls 

bak   messages   passwd   boot

[root@wulimengzhu lianxi]# tar czvf  /back/no_bak.tar.gz  /lianxi/   --exclude=/lianxi/bak

//将/lianxi目录下的bak目录排除不备份到/back/no_bak.tar.gz目录里

 

\##如果，排除多个文件使用{}括起来，中间使用逗号分隔

[root@wulimengzhu lianxi]# tar czvf  /back/no_bak-boot.tar.gz  /lianxi/  --exclude=/lianxi/{bak，boot}

 

tar命令的生产实践 ：

编写一个脚本实现备份/var/log目录下的所有文件到/bak目录下，要求文件名是包含当天日期，文件名例如：2015-12-27-16_30-log.tar.gz，同时要求删除七天前的备份文件，只保留最近7天的

需求分析：

​    ①备份 ---> tar

​    ②tar  czvf  /bak/log.tar.gz  /var/log 

​    ③date +%F-%H_%M_%S

​    ④find  /bak  -mtime +7 -exec  rm -rf {} \;

```
1 #!/bin/bash
2 
3 [ -d /bak ]&&echo "/bak is existing"||mkdir /bak
4 
5 b_time=`date +%F-%H_%M_%S`
6 
7 tar  czf /bak/$b_time-log.tar.gz  /var/log  &>/dev/null&&echo "backup ok"||echo "backup failed"
8 
9 find  /bak  -mtime +7  -exec rm -rf {} \;
```

 

 

二、rsync命令

​         ---a fast, versatile, remote (and local) file-copying tool（同步工具）

常见选项：

　　-a      归档并且是递归

　　-z      压缩，可以传输快速，也省流量

　　--delete    将目标目录中存在的，但是不存在源目录中的文件删除

　　-v      显示详细信息

　　-c      检查校验码

基本格式：

　　rsync  -az  源目录/*  目标目录/      

\#########将源目录下的文件同步到目标目录下

示例：

[root@studying ~]# rsync -az /lianxi/* /tmp/

 

配置Rsync备份服务器

第一步：在配置文件/etc/rsyncd.conf里添加nfsbackup新模块

```
[root@backup ~]# vim /etc/rsyncd.conf
```

```
[nfsbackup]
path = /data/        # 使用目录
ignore errors        # 有错误时忽略
read only = false        # 可读可写（true或false）
list = false        # 阻止远程列表（不让通过远程方式看服务端有啥）
hosts allow = 172.16.0.0/16        # 允许IP
hosts deny = 0.0.0.0/32        # 禁止IP
auth users = rsync_backup        # 虚拟用户
secrets file = /etc/rsync.password        # 存放用户和密码的文件
```

第二步：启动rsync服务

```
[root@backup ~]# rsync --daemon
[root@backup ~]# ss -antup | grep rsync
tcp    LISTEN     0      5          :::873        :::*      users:(("rsync",7098,5))
tcp    LISTEN     0      5          *:873        *:*      users:(("rsync",7098,4))
```