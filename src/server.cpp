#include "../implementations/headers.h"
#include "../implementations/common.cpp"

using namespace std;



int main(){
       int serverSock = socket(AF_INET, SOCK_STREAM , 0);
       
        sockaddr_in serveraddr{};
        serveraddr.sin_family = AF_INET;
        int convIpAddr = inet_pton( AF_INET,"127.0.0.1" ,&serveraddr.sin_addr);
        int port = 8080;
        serveraddr.sin_port = htons(port);

        int socketbind  = bind(serverSock, (struct sockaddr*)&serveraddr, sizeof(sockaddr));
        int serverlistening = listen(serverSock, 5);

        sockaddr_in clientaddr{};
        socklen_t clientaddr_len = sizeof(clientaddr);
        int clientSocket = accept(serverSock, (struct sockaddr*) &clientaddr, &clientaddr_len);

        char buffer[1024] = {};
        int bytesrecv = recv(clientSocket, buffer, sizeof(buffer)-1, 0);
        cout << buffer << endl;
    
        const char* message = "Hello from server\n";
        send(clientSocket, message, strlen(message), 0);

    //testing for custom TCP Framing
    Message msgr;
    if(recvMessage(clientSocket,msgr)){
        cout << (int) msgr.type << " = Type" << endl;
        for(uint8_t c : msgr.payload){
            cout << static_cast<char>(c);
        }
        cout << endl;
    }
    Message msg;
     msg.type = 01;
     msg.payload = {'H', 'E', 'L', 'L', 'O',' ', 'G', 'A', 'N', 'G'};
     if(!sendMessage(clientSocket, msg)){
        return 0;
       };
     


        close(serverSock);
        close(clientSocket);
        serverSock = -1;
        clientSocket = -1;

        return 0;
    }






