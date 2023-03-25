# [使用expect的自动化交互](https://www.cnblogs.com/geaozhang/p/6865969.html)

Q：利用shell脚本实现ssh自动登录远程服务器？

A：[expect命令](https://www.cnblogs.com/geaozhang/p/6865969.html#expect)

```
#!/usr/bin/expect
spawn ssh root@172.16.11.99
expect "*password:"
send "rootzhang\r"
expect "*#"
interact
```

\#!/usr/bin/expect　　//告诉操作系统，此脚本里的代码用expect这个shell来执行（类似与bash）

shell> expect 脚本　　//执行expect脚本

 

1、使用expect -c的嵌套调用

　　如果需要在shell脚本中嵌套expect代码，就要使用expect -c "expect代码"

```
expect -c "
　　spawn ssh $user_name@$ip_addr df -P
　　expect {
　　　　\"*(yes/no)?\" {send \"yes\r\" ; exp_continue}
　　　　\"*password:\" {send \"$user_pwd\r\" ; exp_continue}
　　　　#退出
　　}
"
```

格式：spawn ssh登录远程主机 在该远程主机上要执行的命令(只能执行一条)

注意：在expect -c里面的代码，双引号要用\转义字符。

 

2、使用here document的嵌套调用

```
#!/bin/bash
echo "123"
/usr/bin/expect <<EOF　　#利用here document的expect代码嵌套

spawn ssh root@172.16.11.99
expect "*password:"
send "rootzhang\r"
expect "*#"
send "touch zhangjiacai\r"
expect "*#"
send "exit\r"
expect eof　　#捕获结束

EOF  
```

 

expect详解-- programmed dialogue with interactive programs

　　是一个工具，是一个用来处理交互的命令。

借助Expect，我们可以将交互过程写在一个脚本上，使之自动化完成。

形象的说，ssh登录，ftp登录等都符合交互的定义。可以根据用户设定的规则和系统进程进行自动化交互，例如远程登陆的密码输入、自动化的执行远程命令。

------

expect中最关键的四个命令是spawn、expect、send、interact

spawn：启动新的进程，后面可接shell命令

expect：从进程接收字符串

send：用于向进程发送字符串 interact：允许用户交互

 

1、spawn命令

　　spawn命令就是用来启动新的进程的。

　　spawn后的send和expect命令都是和spawn打开的进程进行交互的。

```
set timeout 30　　//设置超时时间，单位是：秒
spawn ftp 172.16.1.1　　//打开新的进程，该进程用户连接远程ftp服务器
expect "Name"　　//进程返回Name时
send "ftp\r"　　//向进程输入ftp\r
expect "Password:"　　//进程返回Password:时
send "123456\r"　　//向进程输入123456\r
expect "ftp> "　　//进程返回ftp>时
send "mirror xiang\r"　　//向进程输入mirror xiang\r #下载xiang文件夹
expect "ftp> "
send "exit\r"　　//向进程输入exit\r #退出
```

 

2、send命令

　　send命令接收一个字符串参数，并将该参数发送到进程。（有点像here document）

 

3、expect命令

　　expect通常是用来等待一个进程的反馈，expect可以接收一个字符串参数，也可以接收正则表达式参数。

　　和上文的send命令结合，实现简单的交互式。

模式-动作：

\> 单一分支模式语法：

　　expect "hi" {send "You said hi"}　　#匹配到hi后，会输出"you said hi"给进程，作为标准输入

\> 多分支模式语法：

　　#匹配到hi，hello，bye任意一个字符串时，执行相应的输出。

```
expect {
　　"hi" { send "You said hi\n"; exp_continue}
　　"hello" { send "Hello yourself\n"; exp_continue}
　　"bye" { send "That was unexpected\n"}
}
```

 

4、interact

　　利用spawn、expect、send自动化完成部分操作。

　　如果想在适当的时候干预这个过程---就用到了interact（互相影响 互相作用）

比如下载完ftp文件时，仍然可以停留在ftp命令行状态，以便手动的执行后续命令。interact可以达到这些目的，在自动登录ftp后，允许用户交互。

```
spawn ftp 172.16.1.1
expect "Name"
send "ftp\r"
expect "Password:"
send "123456\r"
interact　　//留在ftp中手动执行后续命令操作
```

\> 执行完成后保持交互状态，把控制权交给控制台，这个时候就可以手工操作了；

\> 如果没有这一句登录完成后会退出，而不是留在远程终端上。

 

总结：

　　expect工具在日常的运维中非常有用，可以用在多机器服务重启、远程copy、多机器日志查看、ftp文件操作、telnet等多种场景。shell中有些操作会受限于密码输入的人工操作，expect工具可以代替人工来完成一些交互性工作。