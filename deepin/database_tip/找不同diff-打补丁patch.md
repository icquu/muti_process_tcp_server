# [找不同diff-打补丁patch](https://www.cnblogs.com/geaozhang/p/6885359.html)

Q：为什么要找不同，为什么要打补丁？

A：

　　在Linux应用中，作为DBA，我们知道MySQL跑在Linux系统之上，数据库最重要的追求就是性能，“稳”是重中之重，所以不能动不动就是换系统或是换这换那的，这个时候除非是万不得已，要不然都是在原有基础上改改就行了，也就是给内核及下载的一些源码打补丁或者说是升级，那么在Linux下使用diff制作补丁以及如何使用patch打补丁显得尤为重要。

 

一、找不同：diff命令（differences）

　　-- compare files line by line

　　一行一行的比较文本文件

作用：

　　比较两个文件之间的差异，输出结果为两个文件的不同之处。

　　使用diff命令制作补丁。

格式：

> diff  [OPTION]...  FILES

选项：

　　-u：会将不同的地方放在一起，紧凑易读

　　　　diff -u test1 test2 > test.patch   （利用diff命令生成补丁patch）

　　-r：递归比较目录下的所有文件（比较文件夹时候一定要接-r）

1、diff命令：找不同

```
shell> cp fruit.txt shuiguo.txt
shell> diff fruit.txt shuiguo.txt 
　　//因为是复制的文件，所以文件内容没有差异，也就没有输出结果

shell> echo "banana" >>fruit.txt 
shell> diff fruit.txt shuiguo.txt 
9d8
< banana
　　//diff命令后面，第一个文件有9行，第二个文件有8行，<表示右边文件内容缺失

shell> echo "cherry" >>shuiguo.txt 
shell> diff fruit.txt shuiguo.txt 
9c9
< banana
---
> cherry
　　//diff命令后面，两个文件都是9行，<右边文件缺失banana，>左边文件缺失cherry
```

2、diff命令：制作补丁文件

```
shell> cat ni.txt 
jinan
changqing
linux
chinaitsoft
shell> cp ni.txt wo.txt
shell> diff ni.txt wo.txt 
shell> diff -u ni.txt wo.txt 
　　//copy文件没有内容差异

shell> echo "zhangjiacai" >>wo.txt 
shell> diff -u ni.txt wo.txt 
--- ni.txt    2016-11-02 16:11:35.253976996 +0800
+++ wo.txt    2016-11-02 16:13:50.037971397 +0800
@@ -2,3 +2,4 @@
 changqing
 linux
 chinaitsoft
+zhangjiacai

shell> vim ni.txt 
shell> cat ni.txt 
jinan
linux
chinaitsoft

shell> diff -u ni.txt wo.txt 
--- ni.txt    2016-11-02 16:16:32.930978061 +0800
+++ wo.txt    2016-11-02 16:13:50.037971397 +0800
@@ -1,3 +1,5 @@
 jinan
+changqing
 linux
 chinaitsoft
+zhangjiacai
```

解析：

　　@@ 代表一段范围

　　- 代表ni.txt

　　+ 代表wo.txt

 使用 > 输出重定向生成补丁文件ni-to-wo.patch

```
shell> diff -u ni.txt wo.txt > ni-to-wo.patch
shell> cat ni-to-wo.patch 
--- ni.txt    2016-11-02 16:16:32.930978061 +0800
+++ wo.txt    2016-11-02 16:13:50.037971397 +0800
@@ -1,3 +1,5 @@
 jinan
+changqing
 linux
 chinaitsoft
+zhangjiacai
```

如此，我们就做好了一个补丁文件。

 

 

二、打补丁：patch命令

　　--- apply a diff file to an original.

用途：

　　用来打补丁---修补文件

格式：

> patch  [选项]  原始文件 < 补丁文件
>
> 　　-pN：N表示忽略N层路径
>
> 　　-R： 还原到老版本

注意事项：

　　①如果打多个补丁，注意先后顺序；

　　②打补丁前不要修改源文件；

1、文件和文件的比较

```
shell> diff ni.txt wo.txt 
1a2
> changqing
3a5
> zhangjiacai
shell> diff ni.txt wo.txt >ni-to-wo.patch　　//生成补丁文件

shell> patch ni.txt <ni-to-wo.patch　　//打补丁
patching file ni.txt
shell> diff ni.txt wo.txt　　//打补丁成功 

shell> patch -R ni.txt <ni-to-wo.patch　　//还原到原来的版本（撤销打补丁）
patching file ni.txt

shell> diff ni.txt wo.txt 
1a2
> changqing
3a5
> zhangjiacai
```

2、目录和目录的比较

```
[root@localhost test]# tree qq-v1
qq-v1
├── hosts
└── image
    └── 1.txt

[root@localhost test]# tree qq-v2
qq-v2
├── hosts
├── image
│   └── 1.txt
├── passwd
└── sound
    └── 3.txt

[root@localhost test]# diff -ur qq-v1 qq-v2 
Only in qq-v2: passwd
Only in qq-v2/sound: 3.txt

[root@localhost test]# diff -Nur qq-v1 qq-v2 
diff -Nru qq-v1/passwd qq-v2/passwd
--- qq-v1/passwd    1970-01-01 08:00:00.000000000 +0800
+++ qq-v2/passwd    2016-11-02 17:07:47.664980339 +0800
@@ -0,0 +1,31 @@
+root:x:0:0:root:/root:/bin/bash
+bin:x:1:1:bin:/bin:/sbin/nologin
```

解析：

　　-N --new-file（Treat absent files as empty）如果没有文件，就拿一个空文件和别的目录里的文件比较

 

制作补丁文件进行对目录的打补丁

[root@localhost test]# diff -Nur qq-v1 qq-v2 >patch-v2.txt　　#比较文件夹生成补丁文件--备用：补丁文件patch-v2.txt在test目录下

> -pnum  or  --strip=num
>
> ​     Strip the smallest prefix containing num leading slashes from each file name 
>
> found in the patch file.  
>
> 例如：/a/b/c/d/e/f/g
>
> 　　-p3 的效果就是去掉第3个/前面的内容，效果：c/d/e/f/g
>
> 　　-p4 的效果就是去掉第4个/前面的内容，效果：d/e/f/g

1> 内层打补丁

```
[root@localhost test]# cd qq-v1　　#进入qq目录，进去里面进行打补丁
[root@localhost qq-v1]# patch -p1 <../patch-v2.txt 
patching file passwd
patching file sound/3.txt

[root@localhost qq-v1]# cd ..
[root@localhost test]# diff -Nru qq-v1 qq-v2
　　//没有输出结果说明打补丁成功 

[root@localhost test]# cd qq-v1
[root@localhost qq-v1]# patch -R -p1 <../patch-v2.txt　　//撤销补丁
patching file passwd
patching file sound/3.txt

[root@localhost qq-v1]# cd ..
[root@localhost test]# diff -Nru qq-v1 qq-v2
diff -Nru qq-v1/passwd qq-v2/passwd
--- qq-v1/passwd    1970-01-01 08:00:00.000000000 +0800
+++ qq-v2/passwd    2016-11-02 17:07:47.664980339 +0800
@@ -0,0 +1,31 @@
+root:x:0:0:root:/root:/bin/bash
+bin:x:1:1:bin:/bin:/sbin/nologin
```

2> 外层打补丁

```
//如果qq-v1和qq-v2在相同目录下，就不需要去掉一层路径
[root@localhost test]# patch -p0 <patch-v2.txt 
patching file qq-v1/passwd
patching file qq-v1/sound/3.txt
```

墙裂建议：

　　任何操作前，记得对文件、目录做好备份，防止操作失败导致数据丢失。