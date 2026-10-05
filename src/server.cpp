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
    /*
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
    */
     
    //testing for DH
    Message msg3;
    recvMessage(clientSocket, msg3);
    cout << "Recieved Message type is: " << static_cast<int>(msg3.type) << endl;
    cout << "Payload Size: " << msg3.payload.size();
   
    //convert from bytes 
    BIGNUM* clientPub = BN_bin2bn(msg3.payload.data(),static_cast<int>(msg3.payload.size()),nullptr);
    if(clientPub == nullptr){
    cerr << "Failed to reconstruct public key" << endl;
    return 1;
}
    cout << "Successfully reconstructed A!" << endl;
    cout << "Size: " << BN_num_bytes(clientPub) << " bytes" << endl;

    //peer's DH public key as EVP_PKEY conversion
    EVP_PKEY_CTX* fromctx = EVP_PKEY_CTX_new_from_name(nullptr, "DH", nullptr);
    if(fromctx == nullptr){
    cerr << "Failed to create DH import context" << endl;
    BN_free(clientPub);
    return 1;
    }


    //building params
    int importInit = EVP_PKEY_fromdata_init(fromctx);
    if(importInit <= 0){
        cerr << "Failed to initialise key import" << endl;
        BN_free(clientPub);
        EVP_PKEY_CTX_free(fromctx);
        return 1;
    }

    //defining params, importing a DH public key, it's from the ffdhe2048 group, and its public value is A
    OSSL_PARAM params[3];
    params[0] = OSSL_PARAM_construct_utf8_string(OSSL_PKEY_PARAM_GROUP_NAME, const_cast<char*>("ffdhe2048"),0);

    params[1] = OSSL_PARAM_construct_BN(OSSL_PKEY_PARAM_PUB_KEY, msg3.payload.data(), msg3.payload.size());

    params[2] = OSSL_PARAM_construct_end();

    //converted raw network bytes into an OpenSSL DH public-key object
    EVP_PKEY* clientPublicKey = nullptr;

    int importResult = EVP_PKEY_fromdata(fromctx,&clientPublicKey,
                                EVP_PKEY_PUBLIC_KEY,params);
    if(importResult <= 0){
        cerr << "Failed to import client public key" << endl;
        EVP_PKEY_CTX_free(fromctx);
        return 1;
    }

    cout << "Successfully imported client public key!" << endl;

    // Generate server's own DH keypair
    vector<uint8_t> serverPubBytes;
    EVP_PKEY* serverKey = initDH(serverPubBytes);

    if(serverKey == nullptr){
        cerr << "Failed to generate server DH keypair" << endl;
        EVP_PKEY_free(clientPublicKey);
        EVP_PKEY_CTX_free(fromctx);
        BN_free(clientPub);
        return 1;
    }
    Message response;
    response.type = KEY_RESPONSE;
    response.payload = serverPubBytes;
    if(!sendMessage(clientSocket, response)){
        cerr << "Failed to send server public key" << endl;
        return 1;
    }

    //creating context for derive
    EVP_PKEY_CTX* deriveCtx = EVP_PKEY_CTX_new(serverKey, nullptr);

    if(deriveCtx == nullptr){
        cerr << "Failed to create derive context" << endl;
        return 1;
    }

    //initialise the derivation
    if(EVP_PKEY_derive_init(deriveCtx) <= 0){
        cerr << "Failed to initialise DH derivation" << endl;
        EVP_PKEY_CTX_free(deriveCtx);
        return 1;
    }

    //set peer's public key
    if(EVP_PKEY_derive_set_peer(deriveCtx, clientPublicKey) <= 0){
        cerr << "Failed to set peer public key" << endl;
        EVP_PKEY_CTX_free(deriveCtx);
        return 1;
    }


    //length calcualtion
    size_t secretLen = 0;
    if(EVP_PKEY_derive(deriveCtx, nullptr, &secretLen) <= 0){
        cerr << "Failed to get shared secret length" << endl;
        EVP_PKEY_CTX_free(deriveCtx);
        return 1;
    }

    //sercret key derivation
    vector<uint8_t> sharedSecret(secretLen);
    if(EVP_PKEY_derive(deriveCtx, sharedSecret.data(), &secretLen) <= 0){
        cerr << "Failed to derive shared secret" << endl;
        EVP_PKEY_CTX_free(deriveCtx);
        return 1;
    }
    sharedSecret.resize(secretLen);

    cout << "Server derived shared secret. Length: " << sharedSecret.size() << " bytes" << endl;
    cout << "DH shared secret successfully derived." << endl;

    //creating encryptionkey and authenthication key
    vector<uint8_t> encryptKey = deriveKey(sharedSecret, "encryption");
    vector<uint8_t> macKey = deriveKey(sharedSecret, "authentication");
    cout << "Encryption key length" << encryptKey.size() << endl;
    cout << "MAC key length" << macKey.size() << endl;

    

        EVP_PKEY_free(clientPublicKey);
        EVP_PKEY_free(serverKey);
        EVP_PKEY_CTX_free(fromctx);
        close(serverSock);
        close(clientSocket);
        serverSock = -1;
        clientSocket = -1;

        return 0;
    }






