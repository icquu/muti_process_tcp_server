## [Linux LVM逻辑卷](https://www.cnblogs.com/chenmh/p/5107901.html)

2016-01-08 09:08  [pursuer.chen](https://www.cnblogs.com/chenmh/)  阅读(6866)  评论(4)  [编辑](https://i.cnblogs.com/EditPosts.aspx?postid=5107901)  [收藏](javascript:void(0))

## **概述**  

​      LVM的产生是因为传统的分区一旦分区好后就无法在线扩充空间，也存在一些工具能实现在线扩充空间但是还是会面临数据损坏的风险；传统的分区当分区空间不足时，一般的解决办法是再创建一个更大的分区将原分区卸载然后将数据拷贝到新分区，但是在企业的生产系统往往不允许停机或者允许停机的时间很短，LVM就能很好的解决在线扩充空间的问题，而且不会对数据造成影响，LVM还能通过快照在备份的过程中保证日志文件和表空间文件在同一时间点的一致性；本篇文章不会对概念知识做太详细的介绍，主要讲述LVM的扩容、收缩、移除的方法。

 

## **原理**  

![img](https://images2015.cnblogs.com/blog/135426/201601/135426-20160106234932778-1133957840.png)

**PE(Physical Extend):**卷的最小单位，默认4M大小，就像我们的数据是以页的形式存储一样，卷就是以PE的形式存储。

**PV(Physical Volume)**:物理卷，如果要使用逻辑卷，首先第一步操作就是将磁盘格式化成PV，从上图可以看出PV是保护PE的，PV内PE的数量取决于这块磁盘的容量/4M.

**VG(Volume Group)**:卷组，VG就是将很多PE组合在一起生成一个卷组，当然这里的PE是可以跨磁盘的，如果当前服务器磁盘空间不足就可以增加一个新磁盘对当前系统不会产生任何影响。

**LV(Logical Volume):**逻辑卷，逻辑卷最终是给用户使用的，前面几个都是为创建逻辑卷做的准备，创建逻辑卷的大小只要不超过VG剩余空间就可以。

**步骤：**

**1.PV:格式化磁盘**

**2.VG:创建卷组**

**3.LV:创建逻辑卷**

**4.mkfs:格式化逻辑卷**

**5.mount:挂载逻辑卷**

## **创建LV**  

**1.PV(格式化磁盘):pvcreate命令**      

 第一步就是对磁盘格式化，不要对现有已分区的磁盘做这个操作。

 先查看当前系统磁盘信息，我电脑通过虚拟机刚添加了两块块20G的磁盘，分别是sdb、sdc，使用fdisk -l查看

```
fdisk -l
```

![img](https://images2015.cnblogs.com/blog/135426/201601/135426-20160107143346434-532322874.png)

格式化/dev/sdb

```
pvcreate /dev/sdb /dev/sdc
```

![img](https://images2015.cnblogs.com/blog/135426/201601/135426-20160107143900746-724278211.png)

使用pvdisplay或者pvs查看pv信息

![img](https://images2015.cnblogs.com/blog/135426/201601/135426-20160107143918903-1474595770.png)

从上图可以的信息可以得知PV的大小和剩余空间信息。

**2.VG(创建卷组)：vgcreate命令**

```
vgcreate vggroup /dev/sdb /dev/sdc
```

vggroup是卷组名称

![img](https://images2015.cnblogs.com/blog/135426/201601/135426-20160107143933590-915452268.png)

使用vgdisplay和vgs查看vg信息

![img](https://images2015.cnblogs.com/blog/135426/201601/135426-20160107143941356-1588038822.png)

 

通过vgdisplay可以查看vg的大小和PE的大小，从上图可以看到单个PE的默认大小是4M,当前VG总共有10238个PE,当前vg总共使用了两个pv。

**3.LV(创建逻辑卷):lvcreate -n 逻辑卷名称 -L 逻辑卷大小 卷组名**

记住创建逻辑卷大小不能超过卷组的剩余空间大小，我这里创建30G大小的逻辑卷。

```
lvcreate -n lv -L 30G vggroup
```

![img](https://images2015.cnblogs.com/blog/135426/201601/135426-20160107145039418-1978607993.png)

使用lvdisplay和lvs查看lv的信息

 ![img](https://images2015.cnblogs.com/blog/135426/201601/135426-20160107145633543-1005987807.png)

lvdisplay可以获取到的lv信息包括，lv的名称和路径、lv对应的vg组的名称、lv的大小、当前lv包含的PE数量。

lv的文件路径保存在vg下面。

**4.mkfs(格式化lv)**

```
mkfs.ext4 /dev/vggroup/lv
```

![img](https://images2015.cnblogs.com/blog/135426/201601/135426-20160107150220465-1309510851.png)

**5.mount(挂载LV)**

```
mount -t ext4 /dev/vggroup/lv /mnt
```

![img](https://images2015.cnblogs.com/blog/135426/201601/135426-20160107150755231-737996327.png)

使用df -h查看挂载信息

![img](https://images2015.cnblogs.com/blog/135426/201601/135426-20160107151113590-1562590786.png)

 

注意：这里的挂载只是临时挂载，当服务器重启后挂载会没有，需要永久挂载需要修改配置文件，方法参考：http://www.cnblogs.com/chenmh/p/5097530.html

## **扩充逻辑卷（LV）**  

 在对LV(逻辑卷)进行容量扩充之前先查看VG（卷组）中剩余空间有多少，扩充的大小不能超过VG剩余的空间大小

```
vgdisplay
```

![img](https://images2015.cnblogs.com/blog/135426/201601/135426-20160107182319325-1919821709.png)

当前VG剩余空间9.99G

**1.lvextend(扩充逻辑卷)**

为了演示在线扩充对文件不造成影响，我先在/mnt目录下创建了一个text测试文件，内容为“1”

```
lvextend -L +9.99G /dev/vggroup/lv
```

这里将VG所有的剩余空间全部划分给LV

![img](https://images2015.cnblogs.com/blog/135426/201601/135426-20160107183118403-258143490.png)

![img](https://images2015.cnblogs.com/blog/135426/201601/135426-20160107183125762-1790681262.png)

vg的剩余空间变成了0

![img](https://images2015.cnblogs.com/blog/135426/201601/135426-20160107183131371-1007672024.png)

LV的大小变成了39.99G

**2.resize2fs:更新文件系统** 

```
resize2fs  /dev/vggroup/lv
```

![img](https://images2015.cnblogs.com/blog/135426/201601/135426-20160107183531762-498401950.png)

查看/mnt目录下的text文件是否受影响

![img](https://images2015.cnblogs.com/blog/135426/201601/135426-20160107184426871-2115218774.png)

 文件没有受影响。

## **扩充卷组（VG）**  

 当前我系统VG已经没有剩余空间了，如果这个时候系统空间不够了，这个时候就要增加一块磁盘来增加VG的大小，在虚拟机我增加一个10G的新磁盘sdd

**1.格式化新磁盘（pvcreate）**

```
pvcreate /dev/sdd
```

**2.将格式化的PV添加到VG中去（vgextend）**

```
vgextend vggroup /dev/sdd
```

**3.查看当前vg的大小（vgdisplay）**

**![img](https://images2015.cnblogs.com/blog/135426/201601/135426-20160107185317434-1548987964.png)**

 

当前VG的大小变成了50G,剩余空间也变成10G，当前VG的PV数变成了3

## **收缩逻辑卷（LG）**  

 注意：对逻辑卷进行收缩操作之前必须先卸载逻辑卷，再缩小文件系统，最后才是缩小逻辑卷，而且收缩的大小也不能超过剩余空间大小。

**1.卸载逻辑卷（unmount）**

```
umount /mnt
```

![img](https://images2015.cnblogs.com/blog/135426/201601/135426-20160107190049746-2048268984.png)

**2.收缩文件系统**

这步一定要操作，而且是在收缩LV之前操作

先检查文件系统（e2fsck）

```
e2fsck -f /dev/vggroup/lv
```

![img](https://images2015.cnblogs.com/blog/135426/201601/135426-20160107193917637-242294372.png)

缩小文件系统（resize2fs）

将文件系统缩小到30G，刚才如果细心注意会发现，使用lvdisplay查看文件系统是39.9G，而通过df -h查看是40G，这里以df -h的大小为准，因为下一步收缩LV的时候是通过减操作，所以要数据一致性下一步就是-10G

```
--将文件系统缩小为30G
resize2fs /dev/vggroup/lv 30G
```

**![img](https://images2015.cnblogs.com/blog/135426/201601/135426-20160107193928309-656999164.png)**

**3.缩小LV(lvreduce)**

```
lvreduce -L -10G /dev/vggroup/lv
```

![img](https://images2015.cnblogs.com/blog/135426/201601/135426-20160107193936653-1383413642.png)

注意：这三步操作顺序不能弄乱。

接下来再挂载lV卷看看空间是否修改

```
mount /dev/vggroup/lv /mnt/
```

![img](https://images2015.cnblogs.com/blog/135426/201601/135426-20160107193943278-560417959.png)

可以看到重新挂载后text文件还是存在，lv的大小也变成了30G

 

虽然可以使用收缩LV但是还是不建议这样做，特别是企业的系统，因为收缩后的结果会存在很多的未知可能。

## **收缩卷组（VG）**  

 收缩卷组只能对那些未使用pv进行操作，如果你的当前PV已经已经在逻辑卷中被使用，那么该PV是不能去除的，比如刚才新增加的sdd磁盘未被使用就可以将其从VG中去除。

通过PVS查看那些PV已被使用

```
pvs
```

![img](https://images2015.cnblogs.com/blog/135426/201601/135426-20160107194954606-1330080736.png)

当前只有/dev/sdd未被使用

**1.卸载逻辑卷**

逻辑卷对应的是卷组而不是PV，虽然sdd在前面的操作并没有被添加到LV当中，但是这里还是需要先将LV卸载

```
umount /mnt
```

**2.移除PV(vgreduce)**

```
vgreduce vggroup /dev/sdd
```

![img](https://images2015.cnblogs.com/blog/135426/201601/135426-20160107195645246-1591341560.png)

**3.删除pv**

```
pvremove /dev/sdd
```

![img](https://images2015.cnblogs.com/blog/135426/201601/135426-20160107195929528-1609454221.png)

可以看到/dev/sdd已经被移除

 

## **删除lv、vg、pv**  

1.同样删除前先卸载所有lv,在当前系统只有一个lv

```
umount /mnt
```

 

2.lvremove,vgreomve,pvremove

[![复制代码](https://common.cnblogs.com/images/copycode.gif)](javascript:void(0);)

```
--删除LV
lvremove /dev/vggroup/lv

--删除VG
vgremove vggroup

--删除PV
pvremove /dev/sdb /dev/sdc
```

[![复制代码](https://common.cnblogs.com/images/copycode.gif)](javascript:void(0);)

![img](https://images2015.cnblogs.com/blog/135426/201601/135426-20160107201153762-1364096637.png)

这些删除操作都确保未被使用，比如删除LV在删除前先将数据拷贝出去然后再删除，删除VG确保VG组中不存在LV，删除PV刚才已经说过了，PV需要没有被使用。

 

**重设一个lV的大小**

删除LV

```
卸载LV
umount /home
删除lv
lvremove /dev/vg_mingyu001/lv_home
```

 

创建LV

[![复制代码](https://common.cnblogs.com/images/copycode.gif)](javascript:void(0);)

```
创建LV
lvcreate -n lv_home -L 100G vg_mingyu001
格式化lv
mkfs.ext4 /dev/vg_mingyu001/lv_home
挂载lv
mount -t ext4 /dev/vg_mingyu001/lv_home  /home
```

[![复制代码](https://common.cnblogs.com/images/copycode.gif)](javascript:void(0);)

查看

```
查看挂载
df -h
```

```

```

mount挂载相关内容参考： <http://www.cnblogs.com/chenmh/p/5097530.html>

分区相关内容参考：<http://www.cnblogs.com/chenmh/p/5096592.html>

## **修改逻辑卷名**

 **1.查看逻辑卷名**

```
lvscan
```

```
[root@monitor ~]# lvscan
  ACTIVE            '/dev/vggroup/lv' [19.00 GiB] inherit
```

**2.修改逻辑卷名**

```
lvrename /dev/vggroup/lv /dev/vggroup/lv_data
```

将逻辑卷名“lv”改为“lv_data”

**3.修改分区文件**

```
vim /etc/fstab
```

这里同样将lv改成lv_data

**4.修改引导**

```
vim /etc/grub.conf 
```

引导只针对boot分区，如果不是针对boot分区的逻辑卷进行改名那么也就不修改修改引导的逻辑卷卷名了。

**5.重启** 

```
reboot
```

 

 

 

20161410补充

![img](https://images2015.cnblogs.com/blog/135426/201604/135426-20160410215523687-1102171838.png)

## **总结**

LVM带来的好处不仅仅是在线扩容，在备份的过程中为了保证日志文件和表空间文件一致性问题也用到了LVM，在工作当中可能会遇到这种情况，当前的逻辑空间全部分配到了一个不合理的挂载上了，现在需要将这个挂载的空间调小，这种情况下需要提前将这个挂载内的文件拷贝到别的地方，然后卸载挂载删除lv，再创建一个小的LV，将原先的文件拷贝回来。对于特别重要的数据可以先用一个新盘挂载分配好逻辑LV将数据拷贝到新的LV上然后卸载旧的LV，将新的LV挂载到挂载到旧的LV的挂载点上确保新盘无误后可以删除旧的数据。