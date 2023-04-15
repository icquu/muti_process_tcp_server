//这段代码使用了MySQL C API来管理连接池。在这个示例中，我们定义了一个最大连接池大小（MAX_POOL_SIZE），并使用mysql_init()函数初始化连接池中的每个连接。在初始化连接时，我们使用mysql_real_connect()函数连接到数据库，并检查连接是否成功。如果连接失败，我们将输出错误消息并退出程序。
//在我们需要使用数据库连接时，我们可以调用get_connection()函数从连接池中获取一个连接。如果连接池为空，则会输出一条错误消息。如果连接池不为空，则会返回连接池中的最后一个连接，并将其从连接池中删除。
//当我们完成使用数据库连接时，我们可以调用release_connection()函数将连接返回到连接池中。如果连接池已满，则会输出一条错误消息。
//最后，我们可以调用destroy_pool()函数来销毁连接池中的所有连接，并关闭与数据库的所有连接。
//请注意，这只是一个简单的示例代码，并且可能需要根据您的具体需求进行修改和扩展。

#include <stdio.h>
#include <stdlib.h>
#include <mysql.h>

#define MAX_POOL_SIZE 10

MYSQL *connection_pool[MAX_POOL_SIZE];
int pool_size = 0;

void init_pool(char *host, char *user, char *password, char *database) {
    int i;
    for (i = 0; i < MAX_POOL_SIZE; i++) {
        connection_pool[i] = mysql_init(NULL);
        if (!mysql_real_connect(connection_pool[i], host, user, password, database, 0, NULL, 0)) {
            printf("Error connecting to database: %s\n", mysql_error(connection_pool[i]));
            exit(1);
        }
        pool_size++;
    }
}

MYSQL *get_connection() {
    if (pool_size == 0) {
        printf("No connections available in the pool.\n");
        return NULL;
    }
    MYSQL *connection = connection_pool[pool_size - 1];
    pool_size--;
    return connection;
}

void release_connection(MYSQL *connection) {
    if (pool_size == MAX_POOL_SIZE) {
        printf("Connection pool is full. Cannot release connection.\n");
        return;
    }
    connection_pool[pool_size] = connection;
    pool_size++;
}

void destroy_pool() {
    int i;
    for (i = 0; i < pool_size; i++) {
        mysql_close(connection_pool[i]);
    }
}

int main() {
    char *host = "localhost";
    char *user = "root";
    char *password = "password";
    char *database = "test";
    
    init_pool(host, user, password, database);
    
    MYSQL *connection1 = get_connection();
    MYSQL *connection2 = get_connection();
    MYSQL *connection3 = get_connection();
    
    // Perform database operations using the connections
    
    release_connection(connection1);
    release_connection(connection2);
    release_connection(connection3);
    
    destroy_pool();
    
    return 0;
}
