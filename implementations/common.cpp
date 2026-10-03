#include "./headers.h"
using namespace std;

class Message{
public:
    uint8_t type;
    std::vector<uint8_t> payload;
};

uint32_t decodelength(uint32_t length){
    return ntohl(length);
}

uint32_t encodelength(uint32_t length){
    return htonl(length);
}

vector<uint8_t> createHeader(Message &msg){
  vector<uint8_t> buffer(5);
  buffer[0] = msg.type;
  if(msg.payload.size() > 4096){return {};}
  uint32_t payloadlen = encodelength(msg.payload.size());
  memcpy(buffer.data()+1, &payloadlen, 4);
  return buffer;
};



bool sendall(int socket, uint8_t* data, size_t length){
    size_t senttotal = 0;
    while(senttotal < length){
        ssize_t bytessent = send(socket, data+ senttotal, length-senttotal, 0);
        if(bytessent == -1){
            if(errno == EINTR){
                continue;
            }//syscall interruption
            return false;
        }
        senttotal += bytessent;
    }
 return true;
}

bool recvall(int socket, uint8_t* data, size_t length){
    size_t recvdtotal = 0;
    while(recvdtotal < length){
        ssize_t bytesrecvd = recv(socket, data+recvdtotal, length - recvdtotal, 0);
        if(bytesrecvd == 0){return false;}
        else if(bytesrecvd == -1){
            if(errno == EINTR){
                continue;
            }
            return false;
        }
        recvdtotal += bytesrecvd;
    }
    return true;
}

bool recvMessage(int socket, Message& msg){
    uint8_t header[5];
    if(!recvall(socket,header,5)){
        return false;
    }

    msg.type = header[0];
    uint32_t ntwrklen;
    memcpy(&ntwrklen, header+1, 4);
    uint32_t payloadlen = decodelength(ntwrklen);

    if(payloadlen > 4096){
        return false;
    }else{
        msg.payload.resize(payloadlen);
        if(!recvall(socket, msg.payload.data(), payloadlen)){return false;};
    }
    return true;
}

bool sendMessage(int socket, Message& msg){
    vector<uint8_t> header = createHeader(msg);
    if(header.size() != 5){return false;}
    if(!sendall(socket, header.data(), header.size())){return false;} //send header first
    if(!sendall(socket, msg.payload.data(), msg.payload.size())){return false;} //send paylaod next
    return true;

}

