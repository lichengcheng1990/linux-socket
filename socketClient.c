/*************************************************************************
	> File Name: socketClient.c
	> Created Time: Mon 16 Jan 2017 02:30:27 PM CST
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>

#include<sys/types.h>
#include<sys/socket.h>
#include<sys/un.h>

int main()
{
    unlink("client");
    struct sockaddr_un client_addr;
    bzero(&client_addr, sizeof(client_addr));
    client_addr.sun_family = AF_UNIX;
    strncpy(client_addr.sun_path, "client", sizeof(client_addr.sun_path) - 1);

    int client_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_socket < 0){
        printf("create client_socket failed!,errno=%d, %s \n", errno, strerror(errno ));
        close(client_socket);
    }
    printf("funtion:%s, line: %d, client_socket =%d \n", __func__, __LINE__, client_socket);
    if (bind(client_socket, (struct sockaddr*)&client_addr, sizeof(client_addr)) < 0)
    {
        printf("bind port failed:errno=%d, %s\n", errno, strerror(errno ));
        unlink("client");
        close(client_socket);
    }

    struct sockaddr_un service_addr;
    bzero(&service_addr, sizeof(service_addr));
    service_addr.sun_family = AF_UNIX;
    strncpy(service_addr.sun_path, "service", sizeof(service_addr.sun_path) - 1);
    printf("funtion:%s, line: %d,service_addr.sun_path =%s  \n", __func__, __LINE__, service_addr.sun_path);
    if (connect(client_socket, (struct sockaddr*)&service_addr, sizeof(service_addr)) < 0) {
        printf("connect failed:errno=%d, %s\n", errno, strerror(errno ));
        unlink("client");
        close(client_socket);
    }

    {
        int result = write(client_socket, "ATTACH", 6);
        printf("funtion:%s, line: %d ret = %d \n", __func__, __LINE__, result);
        char buf[1024];
        bzero(buf, sizeof(buf));
        int cnt = read(client_socket, buf, sizeof(buf));
        printf("funtion:%s, line: %d,cnt =%d \n", __func__, __LINE__, cnt);
        printf("client recv: %s \n", buf);
    }


    close(client_socket);
    unlink("client");
    return 0;
}
