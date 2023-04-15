//��δ���ʹ����MySQL C API���������ӳء������ʾ���У����Ƕ�����һ��������ӳش�С��MAX_POOL_SIZE������ʹ��mysql_init()������ʼ�����ӳ��е�ÿ�����ӡ��ڳ�ʼ������ʱ������ʹ��mysql_real_connect()�������ӵ����ݿ⣬����������Ƿ�ɹ����������ʧ�ܣ����ǽ����������Ϣ���˳�����
//��������Ҫʹ�����ݿ�����ʱ�����ǿ��Ե���get_connection()���������ӳ��л�ȡһ�����ӡ�������ӳ�Ϊ�գ�������һ��������Ϣ��������ӳز�Ϊ�գ���᷵�����ӳ��е����һ�����ӣ�����������ӳ���ɾ����
//���������ʹ�����ݿ�����ʱ�����ǿ��Ե���release_connection()���������ӷ��ص����ӳ��С�������ӳ�������������һ��������Ϣ��
//������ǿ��Ե���destroy_pool()�������������ӳ��е��������ӣ����ر������ݿ���������ӡ�
//��ע�⣬��ֻ��һ���򵥵�ʾ�����룬���ҿ�����Ҫ�������ľ�����������޸ĺ���չ��

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
