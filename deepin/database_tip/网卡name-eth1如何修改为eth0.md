# [网卡name-eth1如何修改为eth0](https://www.cnblogs.com/geaozhang/p/6782408.html)

　　正常来说，Linux在识别网卡时第一张会是eth0，第二张才是eth1。

　　有时候我们使用虚拟机克隆技术后网卡的信息就会改变，新克隆出来的虚拟主机网卡名字可能变为eth1.无论我们怎么修改都无法改变，这就对我们使用N台虚拟机进行HA-heartbeat实验时造成了困扰。注： 高可用性(High-Availability,HA)

　　在这里出现这个情况是因为复制系统的过程中复制的文件已经有一个网卡在/etc/udev/rules.d/70-persistent-net.rules被识别成了eth0，而虚拟机中的识别成了eth1。

解决方法：

1、编辑/etc/udev/rules.d/70-persistent-net.rules

[root@studying ~]# vim  /etc/udev/rules.d/70-persistent-net.rules 

![img](https://images2015.cnblogs.com/blog/1113510/201704/1113510-20170428165616365-277577152.png)

//注释掉或是删除原来的eth0那部分，将NAME=eth1部分改为NAME=eth0

 

2、编辑/etc/sysconfig/network-scripts/ifcfg-eth0，把MAC改为正确的，把UUID删掉

[root@studying ~]# cd  /etc/sysconfig/network-scripts/

[root@studying network-scripts]# vim  ifcfg-eth0

 

3、重启生效reboot

当然，在生产环境中，服务器一般都是很少重启，更多的是临时修改了，然后再改配置文件。

临时修改接口的名字：

[root@studying ~]# ip  link  set   eth1  name  eth0　　//将eth1接口的名字改为eth0  

RTNETLINK answers: Device or resource busy　　#如果修改不了，先禁用接口，再修改

```
[root@studying ~]# ifdown  eth1
[root@studying ~]# ip  link  set  eth1  name  eth0
[root@studying ~]# cd   /etc/sysconfig/network-scripts
[root@studying network-scripts]# vim  ifcfg-eth1
[root@studying network-scripts]# mv  ifcfg-eth1  ifcfg-eth0
[root@studying network-scripts]# cat  ifcfg-eth0
DEVICE=eth0
TYPE=Ethernet
ONBOOT=yes
BOOTPROTO=none
IPADDR=192.168.1.100
NETMASK=255.255.255.0
[root@studying network-scripts]# ifup eth0
```