#include "../implementations/headers.h"
#include "../implementations/common.cpp"


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
     
    /*
    Message msg;
     msg.type = 00;
     msg.payload = {'H', 'E', 'L', 'L', 'O',' ', 'G', 'U', 'Y', 'S'};
     if(!sendMessage(clientSocket, msg)){
        return 0;
       };
    */
    //DH
    vector<uint8_t> pubBytes;
    EVP_PKEY* key = initDH(pubBytes);


    Message msg2;
    msg2.type = KEY_INIT;
    msg2.payload = pubBytes;
    sendMessage(clientSocket, msg2);

    //recieve public key B
    Message response;
    if(!recvMessage(clientSocket, response)){
        cerr << "Failed to receive server DH response" << endl;
        return 1;
    }
    if(response.type != KEY_RESPONSE){
        cerr << "Unexpected message type from server" << endl;
        return 1;
    }

    cout << "Received server public key. Size: "<< response.payload.size() << " bytes" << endl;

    //import B to EVP_PKEY
    EVP_PKEY_CTX* fromctx = EVP_PKEY_CTX_new_from_name(nullptr, "DH", nullptr);
    if(fromctx == nullptr){
        cerr << "Failed to create DH import context" << endl;
        return 1;
    }
    if(EVP_PKEY_fromdata_init(fromctx) <= 0){
        cerr << "Failed to initialise key import" << endl;
        EVP_PKEY_CTX_free(fromctx);
        return 1;
    }

    OSSL_PARAM params[3];

    params[0] = OSSL_PARAM_construct_utf8_string(OSSL_PKEY_PARAM_GROUP_NAME,const_cast<char*>("ffdhe2048"),0);

    params[1] = OSSL_PARAM_construct_BN(OSSL_PKEY_PARAM_PUB_KEY,response.payload.data(),response.payload.size());

    params[2] = OSSL_PARAM_construct_end();

    EVP_PKEY* serverPublicKey = nullptr;

    if(EVP_PKEY_fromdata(fromctx,&serverPublicKey,EVP_PKEY_PUBLIC_KEY,params) <= 0){
        cerr << "Failed to import server public key" << endl;
        EVP_PKEY_CTX_free(fromctx);
        return 1;
    }

    cout << "Successfully imported server public key!" << endl;

    //shared secret derivation
    EVP_PKEY_CTX* deriveCtx = EVP_PKEY_CTX_new(key, nullptr);

    if(deriveCtx == nullptr){
        cerr << "Failed to create derive context" << endl;
        return 1;
    }

    if(EVP_PKEY_derive_init(deriveCtx) <= 0){
        cerr << "Failed to initialise DH derivation" << endl;
        return 1;
    }

    if(EVP_PKEY_derive_set_peer(deriveCtx, serverPublicKey) <= 0){
        cerr << "Failed to set peer public key" << endl;
        return 1;
    }

    size_t secretLen = 0;

    if(EVP_PKEY_derive(deriveCtx, nullptr, &secretLen) <= 0){
        cerr << "Failed to get shared secret length" << endl;
        return 1;
    }

    vector<uint8_t> sharedSecret(secretLen);

    if(EVP_PKEY_derive(deriveCtx, sharedSecret.data(), &secretLen) <= 0){
        cerr << "Failed to derive shared secret" << endl;
        return 1;
    }

    sharedSecret.resize(secretLen);

    cout << "Client derived shared secret. Length: "<< sharedSecret.size() << " bytes" << endl;
    cout << "DH shared secret successfully derived." << endl;

     //using deriveKey 
    vector<uint8_t> encryptKey = deriveKey(sharedSecret, "encryption");
    vector<uint8_t> macKey = deriveKey(sharedSecret, "authentication");
    cout << "Encryption key length" << encryptKey.size() << endl;
    cout << "MAC key length" << macKey.size() << endl;

    //Level 4 implementation
    vector<uint8_t> transcript;

    transcript.insert(transcript.end(),pubBytes.begin(),pubBytes.end());

    transcript.insert(transcript.end(),response.payload.begin(),response.payload.end());

    //client's handshake MAC
    vector<uint8_t> clientFinished = calculateHMAC(macKey, transcript);
    if(clientFinished.empty()){
        cerr << "Failed to calculate handshake MAC" << endl;
        return 1;
    }

    //LVL 4 testing
    Message finished;
    finished.type = HANDSHAKE_FINISHED;
    finished.payload = clientFinished;

    if(!sendMessage(clientSocket, finished)){
        cerr << "Failed to send handshake confirmation" << endl;
        return 1;
    }

    //client verfying server
    Message serverFinished;

    if(!recvMessage(clientSocket, serverFinished)){
        cerr << "Failed to receive server handshake confirmation" << endl;
        return 1;
    }

    if(serverFinished.type != HANDSHAKE_FINISHED){
        cerr << "Expected handshake confirmation" << endl;
        return 1;
    }

    //
    vector<uint8_t> expectedServerMAC = calculateHMAC(macKey, transcript);
    if(serverFinished.payload != expectedServerMAC){
        cerr << "Server handshake verification FAILED!" << endl;
        return 1;
    }

    cout << "Server handshake confirmation verified!" << endl;


    /*
    Message msgr;
    if(recvMessage(clientSocket,msgr)){
        cout << (int) msgr.type << " = Type" << endl;
        for(uint8_t c : msgr.payload){
            cout << static_cast<char>(c);
        }
        cout << endl;
    }   
    */
    return 0;
}