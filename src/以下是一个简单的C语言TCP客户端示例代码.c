//以下是一个简单的C语言TCP客户端示例代码：

//```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_IP "192.168.15.122"
#define SERVER_PORT 8080

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[1024];

    // 创建客户端socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        printf("Failed to create client socket.\n");
        return 1;
    }

    // 设置服务器地址
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // 连接服务器
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("Failed to connect to server.\n");
        close(client_socket);
        return 1;
    }

    // 发送数据到服务器
    strcpy(buffer, "Hello, server!");
    if (send(client_socket, buffer, strlen(buffer), 0) < 0) {
        printf("Failed to send data to server.\n");
        close(client_socket);
        return 1;
    }

    // 接收服务器的响应
    memset(buffer, 0, sizeof(buffer));
    if (recv(client_socket, buffer, sizeof(buffer), 0) < 0) {
        printf("Failed to receive data from server.\n");
        close(client_socket);
        return 1;
    }

    printf("Received message from server: %s\n", buffer);

    // 关闭客户端socket
    close(client_socket);

    return 0;
}
//```

//这个示例代码创建了一个TCP客户端socket，连接到指定的服务器地址和端口，并发送一条消息到服务器。然后，它接收服务器的响应并打印出来，最后关闭客户端socket。