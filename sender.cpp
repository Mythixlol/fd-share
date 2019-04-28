#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

#define UDS_FILE "/tmp/sock.uds"

void sendIt(int a, int b);

int main(void)
{

    int fd = open("file.txt", O_WRONLY | O_CREAT);

    int create_socket, new_socket;
    socklen_t addrlen;
    struct sockaddr_un address;
    if ((create_socket = socket(AF_LOCAL, SOCK_STREAM, 0)) > 0)
        printf("Socket wurde angelegt\n");
    unlink(UDS_FILE);
    address.sun_family = AF_LOCAL;
    strcpy(address.sun_path, UDS_FILE);
    if (bind(create_socket,
             (struct sockaddr *)&address,
             sizeof(address)) != 0)
    {
        printf("Der Port ist nicht frei – belegt!\n");
    }
    listen(create_socket, 5);
    addrlen = sizeof(struct sockaddr_in);
    while (1)
    {
        new_socket = accept(create_socket,
                            (struct sockaddr *)&address,
                            &addrlen);
        if (new_socket > 0)
        {
            printf("Ein Client ist verbunden ...\n");
            sendIt(new_socket, fd);
            close(new_socket);
            break;
        }
    }
    close(create_socket);
    return EXIT_SUCCESS;
}

void sendIt(int socket, int fd) // send fd by socket
{
    struct msghdr msg = {0};
    char buf[CMSG_SPACE(sizeof(fd))];
    memset(buf, '\0', sizeof(buf));
    struct iovec io = {.iov_base = (void *)"ABC", .iov_len = 3};

    msg.msg_iov = &io;
    msg.msg_iovlen = 1;
    msg.msg_control = buf;
    msg.msg_controllen = sizeof(buf);

    struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    cmsg->cmsg_len = CMSG_LEN(sizeof(fd));

    *((int *)CMSG_DATA(cmsg)) = fd;

    msg.msg_controllen = CMSG_SPACE(sizeof(fd));
}
