#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define UDS_FILE "/tmp/sock.uds"

static int recFD(int sock);

int main(int argc, char **argv)
{

    // fd before init
    int fd = -1;

    // sender-socket
    int create_socket;
    struct sockaddr_un address;
    int size;
    if ((create_socket = socket(PF_LOCAL, SOCK_STREAM, 0)) > 0)
        printf("Socket wurde angelegt\n");
    address.sun_family = AF_LOCAL;
    strcpy(address.sun_path, UDS_FILE);
    if (connect(create_socket,
                (struct sockaddr *)&address,
                sizeof(address)) == 0)
        printf("Verbindung mit dem Server hergestellt\n");

    while (fd < 0)
    {
        fd = recFD(create_socket);
    }

    printf("done\n");

    close(create_socket);
    return EXIT_SUCCESS;
}

static int recFD(int socket) // receive fd from socket
{
    struct msghdr msg = {0};

    char m_buffer[256];
    struct iovec io = {.iov_base = m_buffer, .iov_len = sizeof(m_buffer)};
    msg.msg_iov = &io;
    msg.msg_iovlen = 1;

    char c_buffer[256];
    msg.msg_control = c_buffer;
    msg.msg_controllen = sizeof(c_buffer);

    if (recvmsg(socket, &msg, 0) < 0)
        printf("%s \n", "failed");

    struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);

    unsigned char *data = CMSG_DATA(cmsg);

    printf("%s \n", "started");
    int fd = *((int *)data);
    printf("%s \n", "done");

    return fd;
}
