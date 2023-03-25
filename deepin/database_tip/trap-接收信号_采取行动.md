# [trap-接收信号_采取行动](https://www.cnblogs.com/geaozhang/p/6874310.html)

trap命令用于指定在接收到信号后将要采取的动作，常见的用途是在脚本程序被中断时完成清理工作。

　　kill和trap等都可以看到信号编号及其关联的名称。

“信号”是指那些被异步发送到一个程序的事件，默认情况下，它们通常会终止一个程序的运行。

基本格式：

> trap command signal

前一部分是接收到指定信号时将要采取的行动，后一部分是要处理的信号名。

 

1、trap "commands" signal-list

当脚本收到signal-list清单内列出的信号时，trap命令执行双引号中的命令。

 

2、trap signal-list

trap不指定任何命令，接受信号的默认操作，默认操作是结束进程的运行。

 

3、trap " " signal-list

trap命令指定一个空命令串，允许忽视信号。

 

注意：

　　脚本程序通常是以从上到下的顺序解释执行的，所以必须在想保护的那部分代码以前指定trap命令。

 

信号量详细列表：trap -l 或是 kill -l

 ![img](https://images2015.cnblogs.com/blog/1113510/201705/1113510-20170518165427619-1178140563.png)

```
 1 SIGHUP 终止进程 挂起，终端线路挂断
 2 
 3 SIGINT 终止进程 中断进程
 4 
 5 SIGQUIT 建立CORE文件 退出，终止进程，并且生成core文件
 6 
 7 SIGILL 建立CORE文件 非法指令
 8 
 9 SIGTRAP 建立CORE文件 跟踪自陷
10 
11 SIGABRT 程序自己发现错误并调用abort时产生
12 
13 SIGBUS 建立CORE文件 总线错误
14 
15 SIGSEGV 建立CORE文件 段非法错误
16 
17 SIGFPE 建立CORE文件 浮点异常
18 
19 SIGIOT 建立CORE文件 执行I/O自陷
20 
21 SIGKILL 终止进程 杀死进程
22 
23 SIGPIPE 终止进程 向一个没有读进程的管道写数据
24 
25 SIGALARM 终止进程 报警，计时器到时
26 
27 SIGTERM 终止进程 软件终止信号，通常在系统关机时发送
28 
29 SIGSTOP 停止进程 非终端来的停止信号
30 
31 SIGTSTP 停止进程 终端来的停止信号
32 
33 SIGCONT 忽略信号 继续执行一个停止的进程
34 
35 SIGURG 忽略信号 I/O紧急信号
36 
37 SIGIO 忽略信号 描述符上可以进行I/O
38 
39 SIGCHLD 忽略信号 当子进程停止或退出时通知父进程
40 
41 SIGTTOU 停止进程 后台进程写终端
42 
43 SIGTTIN 停止进程 后台进程读终端
44 
45 SIGXGPU 终止进程 CPU时限超时
46 
47 SIGXFSZ 终止进程 文件长度过长
48 
49 SIGWINCH 忽略信号 窗口大小发生变化
50 
51 SIGPROF 终止进程 统计分布图用计时器到时
52 
53 SIGUSR1 终止进程 用户定义信号1
54 
55 SIGUSR2 终止进程 用户定义信号2
56 
57 SIGVTALRM 终止进程 虚拟计时器到时
```

 

杀死所有子进程

　　因为如果父进程执行完毕后，检测core的进程都没有发现core，所有子进程还存在，这就是传说中的僵尸进程。

　　所以要杀死父进程所有子进程。

shell> kill -9 0　　#杀死脚本自己及衍生出来的子进程

 

最后贴上源码示例一下：

shell> pa.script.sh

```
#! /bin/bash
#调用sub_script.sh 检测core

trap 'echo "捕获成功";exit' 3

echo "当前位置：父脚本"

echo "PID is $$"

bash sub_script.sh　　#开启子进程脚本
```

 

shell> sub_script.sh

```
#! /bin/bash
#this shell script is designed to catch core,if corehappens,will send the father to exit

echo "当前脚本：子脚本"

echo "PID is $$"

echo "PPID is $PPID"

sleep 2

kill -3 $PPID
```