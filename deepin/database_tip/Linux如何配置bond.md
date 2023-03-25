# [Linux如何配置bond](https://www.cnblogs.com/geaozhang/p/6763876.html)

Q：什么是BOND？

A：

　　将多块网卡虚拟成为一块网卡的技术，通过bond技术让多块网卡看起来是一个单独的以太网接口设备并具有相同的ip地址。

 

Q：为什么要配置bond？

A：

　　在linux下配置bond，通过网卡绑定技术既能增加服务器的可靠性，又增加了可用网络宽带，为用户提供不间断的网络服务。

 

原理：

　　网卡工作在混杂（promisc）模式，接收到达网卡的所有数据包，tcpdump工作用的也是混杂模式（promisc），将两块网卡的 MAC地址修改为相同接收特定MAC的数据帧，然后把相应的数据帧传送给bond驱动程序进行处理。

 

------

　　mode=1：在主备模式下，只有主网卡eth0工作，eth1作为备份网卡是不工作的，只有当一个网络接口失效时（例如交换机掉电等），为了不会出现网络中断，系统会按照配置指定的网卡顺序启动工作，保证机器仍能对外服务，起到了失效保护功能。

　　mode=0：在负载均衡模式下，两块网卡都工作，提供两倍带宽。

------

 

Q：如何配置BOND

A：配置bond的步骤

 

1、选择2个自己需要的网口，查看有哪些网口

[root@studying ~]# ifconfig  -a

 

2、编辑网口的配置文件

[root@studying ~]# cd  /etc/sysconfig/network-scripts

[root@studying network-scripts]# ls  

以ifcfg-开头

例如：eth0的配置文件名是 ifcfg-eth0

 

[root@studying network-scripts]# vim  ifcfg-eth0

[root@studying network-scripts]# cat  ifcfg-eth0

DEVICE=eth0               #网口名eth0

TYPE=Ethernet            #以太网接口

ONBOOT=yes              #LINUX在启动时激活该网口

BOOTPROTO=none    #使用什么协议激活网口

​                                               none表示不使用任何协议

​                                               static表示设置静态IP

​                                               dhcp表示动态获取IP

MASTER=bond0         #指定虚拟网口的名字（主人）

SLAVE=yes                  #备用（从设备）

 

[root@studying network-scripts]# vim  ifcfg-eth1

[root@studying network-scripts]# cat  ifcfg-eth1

DEVICE=eth1               #网口名eth1

TYPE=Ethernet            #以太网接口

ONBOOT=yes              #LINUX在启动时激活该网口

BOOTPROTO=none    #使用什么协议激活网口

​                                              none表示不使用任何协议

​                                              static表示设置静态IP

​                                              dhcp表示动态获取IP

MASTER=bond0         #指定虚拟网口的名字

SLAVE=yes                  #备用

备注：eth0、eth1的MASTER（主人）是bond0，本身是属于从设备作为bond0的SLAVE（备用）

 

[root@studying network-scripts]# vim  ifcfg-bond0

[root@studying network-scripts]# cat  ifcfg-bond0

DEVICE=bond0

TYPE=Ethernet

ONBOOT=yes

BOOTPROTO=static

IPADDR=172.16.11.99

NETMASK=255.255.0.0

GATEWAY=172.16.1.1

 

3、配置bonding

[root@studying network-scripts]# vim  /etc/modprobe.d/dist.conf

末尾增加以下两行

alias bond0 bonding

options bond0 miimon=100 mode=1

\#miimon是用来进行链路监测的：miimon=100表示系统每100ms监测一次链路连接状态，如果有一条线路不同就转入另一条线路。

\#mode=1表示属于主备模式。

 

4、在/etc/rc.d/rc.local文件中加入如下语句，是系统启动自动运行

ifenslave  bond0  eth0  eth1

\#route  add  -net 172.16.0.0  netmask  255.255.0.0  bond0---如果有需要添加该路由

 

5、reboot重启

 

6、如果查看当前哪个网口是主用网口

[root@studying ~]# cat  /proc/net/bonding/bond0

```
Ethernet Channel Bonding Driver: v3.7.1 (April 27, 2011)

Bonding Mode: fault-tolerance (active-backup)
Primary Slave: None
Currently Active Slave: eth1
MII Status: up
MII Polling Interval (ms): 100
Up Delay (ms): 0
Down Delay (ms): 0

Slave Interface: eth0
MII Status: up
Speed: 1000 Mbps
Duplex: full
Link Failure Count: 1
Permanent HW addr: 00:0c:29:9d:58:ae
Slave queue ID: 0

Slave Interface: eth1
MII Status: up
Speed: 1000 Mbps
Duplex: full
Link Failure Count: 0
Permanent HW addr: 00:0c:29:9d:58:b8
Slave queue ID: 0
```

注意：

　　①UUID是唯一的编号，每个网口唯一

　　②两个配置文件中HWADDR不能一样，可以不要，因为会自动读取

　　③网口切换后 ifconfig bond0查看到的MAC地址还是切换前的MAC地址-->为了防止IP对应MAC地址混乱。