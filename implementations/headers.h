#ifndef HEADERS_H
#define HEADERS_H

#include<bits/stdc++.h>
#include<sys/socket.h>
#include<unistd.h>
#include<cerrno>
#include<cstring>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<asm-generic/socket.h>
#include <cstddef>
#include<cstdint>


enum MESSAGE_TYPE : uint8_t{
    CHAT = 1,
    ERR,
    TERMINATE,
    KEY_INIT,
    KEY_RESPONSE
};

#endif