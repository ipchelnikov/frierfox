#pragma once

#include <iostream>
#include <ws2tcpip.h>

#include "Friersock.h"
#include "logger.h"

using namespace std;

Friersock::Friersock()
{
    listen_socket = INVALID_SOCKET;
}

int Friersock::init()
{
    int iResult = 0;

    WSADATA wsaData;

    struct addrinfo *result = NULL;
    struct addrinfo hints;

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (iResult)
    {
        TRACE << "WSAStartup failed with error: " << iResult << ENDL;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, "6666", &hints, &result);

    if (iResult != 0) {
        TRACE << "getaddrinfo failed with error: " << iResult << ENDL;
        return iResult;
    }

    listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (listen_socket == INVALID_SOCKET)
    {
        TRACE << "socket failed with error: " << WSAGetLastError() << ENDL;
        return WSAGetLastError();
    }

    iResult = bind(listen_socket, result->ai_addr, static_cast<int>(result->ai_addrlen));

    if (iResult == SOCKET_ERROR)
    {
        TRACE << "bind failed with error: " << WSAGetLastError() << ENDL;
        return WSAGetLastError();
    }

    iResult = ::listen(listen_socket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) 
    {
        TRACE << "listen failed with error: " << WSAGetLastError() << ENDL;
        return WSAGetLastError();
    }

    accept_socket = accept(listen_socket, NULL, NULL);

    if (accept_socket == INVALID_SOCKET)
    {
        TRACE << "accept failed with error: " << WSAGetLastError() << ENDL;
        return WSAGetLastError();
    }

    // No longer need server socket
    closesocket(listen_socket);

    return 0;
}

int Friersock::listen() 
{
    int iResult = recv(accept_socket, recvbuf, recvbuflen, 0);
    
    if (iResult > 0) 
    {
        TRACE << "Bytes received: " << iResult << ENDL;
    }
    else if (iResult == 0) 
    {
        TRACE << "Conection closing..." << ENDL;
    }
    else 
    {
        TRACE << "recv failed with error: " << WSAGetLastError() << ENDL;
    }

    return iResult;
}

int Friersock::send(const char* sendbuf)
{
    int iResult = ::send(accept_socket, sendbuf, (int)strlen(sendbuf), 0);

    if (iResult == SOCKET_ERROR)
    {
        TRACE << "send failed: " << WSAGetLastError() << ENDL;
        return WSAGetLastError();
    }

    return iResult;
}

Friersock::~Friersock()
{
    closesocket(accept_socket);
    WSACleanup();
}

