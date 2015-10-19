#pragma once

#include <winsock2.h>

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 1000

class Friersock
{
public:
    Friersock();
    ~Friersock();

    int init();
    int listen();
    int send(const char* sendbuf);

    char *getBuf() { return recvbuf; }

private:
    SOCKET accept_socket;
    SOCKET listen_socket;

    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
};