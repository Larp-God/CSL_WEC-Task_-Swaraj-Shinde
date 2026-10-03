#include "../implementations/headers.h"
#include "../implementations/common.cpp"
#include <ostream>

using namespace std;

int main(){
   int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

   sockaddr_in serveraddr{};
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port =htons(8080);

    inet_pton(AF_INET, "127.0.0.1", &serveraddr.sin_addr);

    int connect2server = connect(clientSocket, (struct sockaddr*) &serveraddr, sizeof(serveraddr));
    
    //trial testing for basic TCP server
    const char* message = "Hello from client!\n";
    send(clientSocket, message, strlen(message), 0);

    char buffer[1024] = {};
    int bytesrecv = recv(clientSocket, buffer, sizeof(buffer)-1, 0);
    cout << buffer << endl;
    
    //trial testing for custom TCP framing
    Message msg;
     msg.type = 00;
     msg.payload = {'H', 'E', 'L', 'L', 'O',' ', 'G', 'U', 'Y', 'S'};
     if(!sendMessage(clientSocket, msg)){
        return 0;
       };

    Message msgr;
    if(recvMessage(clientSocket,msgr)){
        cout << (int) msgr.type << " = Type" << endl;
        for(uint8_t c : msgr.payload){
            cout << static_cast<char>(c);
        }
        cout << endl;
    }   

    return 0;
}