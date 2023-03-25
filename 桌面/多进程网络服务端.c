//这是个用fork多进程的网络服务端程序，使用了标准的编写模式流程，我用中文详细标注下解释，供个人与大众参考，有bug地方，愿大
//家供探讨，参考steven的UNIX环境高级编程和UNIX网络编程。
//MSN: [email]wu23qing45ying@163.com[/email] E-mail: [email]carywu@yahoo.cn[/email]
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////
//头文件
//////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

//////////////////////////////////////////////////
//全局定义。
//////////////////////////////////////////////////
#define LISTENQ 5
#define BUFFER_SIZE 1024

int child_process(int fd);

//////////////////////////////////////////////////
// 主函数
//////////////////////////////////////////////////
int main(int argc, char **argv) {
    int global_connect_number = 0; //用于检测连接服务端的客户端个数。在父进程中获得数量。
    //XXX: step 1: check the argc
    if (argc < 3) //用于检测程序后的参数个数，没有IP和port就会出错退出。
    {
        fprintf(stderr, "Usage: %s<ipaddress><port>\n", strerror(errno));
        exit(1);
    }
    //XXX: step 2: create a socket
    int fd; //定义文件描述符。

    if ((fd = socket(PF_INET, SOCK_STREAM, 0)) < 0) //socket程序，成功返回文件描述符。你不知道什么是文件描述符？看UNIX网络编程去哦。
    {
        fprintf(stderr, "socket failed:%s\n", strerror(errno)); //错误处理，获得errno。
        exit(1); //确实退出不是好办法，不过这只是学习程序，原谅下。
    }

//XXX: step 3: create a bind
    struct sockaddr_in server_address; //定义bind使用的IP 地址结构。为什么用sockaddr_in而不是socketaddr? 
    memset(&server_address, 0, sizeof(server_address)); //初始化这个IP地址结构，用0填充。

    server_address.sin_family = PF_INET; //IPV4 address,
    server_address.sin_port = htons(atoi(argv[2]));//因为字节序问题，需要用htons和atoi转换下端口。
    server_address.sin_addr.s_addr = inet_addr(argv[1]);//同理。

    if (bind(fd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) //bind程序，注意类型转换成sockaddr
    {
        fprintf(stderr, "bind failed:%s\n", strerror(errno)); //出错处理。
        exit(1);// 不推荐这么使用。
    }

    //XXX: step 4: create a listen
    if (listen(fd, LISTENQ) < 0)//listen函数，监听此文件描述符对应的socket绑定IP和port。
    {
        fprintf(stderr, "listen failed:%s\n", strerror(errno));
        exit(1);
    } else {
        fprintf(stdout, "socket is listening the port:%s:%d\n", argv[1], atoi(argv[2]));// 显示成功。
    }
    //XXX: step 5: create a accept
    int new_fd;
    socklen_t address_len; //定义地址大小，socklen_t类型。
    struct sockaddr_in remote_address; //定义远程客户端的ip地址
    address_len = sizeof(remote_address); //为什么这么用？后面可以看到。

    for (;;) //for循环，
    {
        if ((new_fd = accept(fd, (struct sockaddr *) &remote_address, &address_len)) < 0) //accept语句，接收远端地址。
        {
            fprintf(stderr, "accept failed():%s\n", strerror(errno));
            exit(1);
        } else {
            fprintf(stdout, "accept a new TCP connect %d from %s:%d\n", new_fd, inet_ntoa(remote_address.sin_addr),
                    ntohs(remote_address.sin_port)); //成功显示的信息，注意字节序问题。

        }
        //XXX: step 6: create a fork
        pid_t pid;

        if ((pid = fork()) < 0) //用fork新建立子进程，用子进程处理读和写的问题。
        {
            fprintf(stderr, "fork() failed:%s\n", strerror(errno));
            exit(1);
        } else if (pid == 0) //子进程，
        {
            //CHILD PROCESS
            child_process(new_fd);// 处理函数，下面有，也可以不写成函数，但哪个好？不要偶说了。
            exit(0);
        } else //父进程。
        {
            //PARENT PROCESS
            fprintf(stdout, "%d have connection of %d\n", getpid(), global_connect_number++);//看到了吧，显示连接数量。

        }
    }

    close(fd); //记得关闭描述符哦。
    close(new_fd);
    return 0;
}


int child_process(int fd) //子程序的读写处理函数。
{
    ssize_t written; //写程序返回的写词的个数。
    char *request = "Hello, welcome to my char program!\n";//算是给客户端的返回信息了。
    again:
    if ((written = write(fd, request, strlen(request))) < 0)// 写程序，也可以用send()代替.

    {
        if (errno == EINTR) //碰到信号中断，执行again工作。
        {
            fprintf(stderr, "write was closed by signal\n");
            goto again;
        } else {
            fprintf(stderr, "write failed:%s\n", strerror(errno)); //出错信息。
            exit(1);
        }
    } else {
        fprintf(stdout, "write successd!\n"); //成功信息，这比较简单，能写多点功能更好。我懒。
    }

    char buffer[BUFFER_SIZE]; //从监控端口读到的信息存放点。
    ssize_t n; //读到的数。

    for (;;) //又一个for循环，
    {

        if ((n = read(fd, buffer, BUFFER_SIZE)) < 0) //读程序，从fd文件描述符中读数据，存到buffer中。
        {
            fprintf(stderr, "recv() failed:%s\n", strerror(errno));
            exit(1);
        } else if (n == 0) //读完了的处理。
        {
            fprintf(stderr, "connection was closed by peer\n");
            close(fd);
            break;
        } else {
            buffer[n] = '\0';//有这个才不会出现下次读信息时出现数据不匹配的问题。
            struct sockaddr_in peer_address;
            socklen_t peer_len;
            peer_len = sizeof(peer_address);

            if (getpeername(fd, (struct sockaddr *) &peer_address, &peer_len) < 0) //获得ip结构信息。
            {
                fprintf(stderr, "getpeername() failed:%s\n", strerror(errno));
                exit(1);
            } else {
                fprintf(stdout, "On socket %d(%s:%d):%s\n", fd, inet_ntoa(peer_address.sin_addr),
                        ntohs(peer_address.sin_port), buffer); //读成功时的输出，显示远程IP地址和端口。
            }


        }
    }

    close(fd);//关闭文件描述符。
//    free(buffer);//清空缓存。避免出现内存泄漏。
    return 0;
}