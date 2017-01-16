/*************************************************************************
	> File Name: socketdemo.c
	> Created Time: Mon 16 Jan 2017 11:59:11 AM CST
 ************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <poll.h>

int creatSocket(char* name)
{
    int SocketId = socket(AF_UNIX, SOCK_STREAM, 0);
    if (SocketId < 0) {
        printf("create SocketId failed!,errno=%d, %s \n", errno, strerror(errno ));
        close(SocketId);
        return -1;
    }

    struct sockaddr_un addr;
    bzero(&addr, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, "service", sizeof(addr.sun_path)-1);
   
    if (bind(SocketId, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        printf("bind port failed:errno=%d, %s\n", errno, strerror(errno ));
        unlink(name);
        close(SocketId);
        return -1;
    }
    printf("addr.sun_path is %s \n", addr.sun_path);
    return SocketId;
    
}

int main()
{
    unlink("service");
    int serviceSocketId =creatSocket("service");
    printf("serviceSocketId = %d \n", serviceSocketId);

    //service can connext max clientSock number cant seen cat /proc/sys/net/core/somaxconn
    listen(serviceSocketId, 128);

    struct pollfd *fds = malloc(128*sizeof(struct pollfd));
    bzero(fds, sizeof(struct pollfd)*128);



    int poll_num = 1;
    fds[0].fd = serviceSocketId;
    fds[0].events = POLLIN;



    int endFlags = 1;

    while(endFlags)
    {
        int ret = poll(fds, 2, -1);

        if (ret < 0) {
//            printf("poll failed:errno=%d, %s\n", errno, strerror(errno ));
            continue;
        }

        if (fds[0].revents & POLLIN)
        {
            struct sockaddr_un ca;
            socklen_t ca_len;
            bzero(&ca, sizeof(ca));
            int new_fd = accept(fds[0].fd, (struct sockaddr*)&ca, &ca_len);
            printf("%s conneted. new_fd =%d \n", ca.sun_path, new_fd);
            fds[poll_num].fd = new_fd;
            fds[poll_num].events = POLLIN;
            poll_num ++;
        }
 
        int i = 1;
        for (i = 1; i < poll_num; i++) {
            if (fds[i].revents & POLLIN) {
                char buf[1024];
                bzero(buf, sizeof(buf));
                int ret = read(fds[i].fd, buf, sizeof(buf));

                if (ret == 0) {
                    close(fds[i].fd);
                    endFlags = 0;
                }
                else {
                    printf("receive buf : %s,ret =%d \n", buf, ret);
                    write(fds[i].fd, "OK\n", 3);
                }
            }
        }
    } 

    close(serviceSocketId);
    unlink("service");
   
    return 0;
}

