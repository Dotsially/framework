#ifndef SOCKET_WINDOWS
#define SOCKET_WINDOWS

#include "common.cpp"

struct ClientSocket{
    SOCKET socket;
    sockaddr_in address;
};

struct ServerSocket{
    SOCKET socket;
    sockaddr_in address;
    uint16_t size;
};

struct ConnectionResult{
    int code;
    ClientSocket client;
};

struct SendResult {
    bool success;
    int dataSent;
    int errorCode; // 0 on success, or WSAGetLastError() on failure
};

int SocketServerCreate(ServerSocket* server, uint16_t serverSize, int PORT, bool isSinglePlayer){
    WSADATA wsa;

    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0){
        std::cout << "Failed to init wsa: " << WSAGetLastError();
        return -1;
    }

    if((server->socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET){
        std::cout << "Could not create socket: " << WSAGetLastError();
        WSACleanup();        
        return -1;
    }

    server->address.sin_family = AF_INET;
    server->address.sin_port = htons(PORT);

    server->address.sin_addr.s_addr = INADDR_ANY;
    if (isSinglePlayer) server->address.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (bind(server->socket, (sockaddr*)&server->address, sizeof(server->address)) == SOCKET_ERROR){
        std::cout << "Could not bind socket: " << WSAGetLastError();
        closesocket(server->socket);
        WSACleanup();
        return -1;
    }

    if (listen(server->socket, serverSize) == SOCKET_ERROR){
        std::cout << "Listen failed: " << WSAGetLastError();
        closesocket(server->socket);
        WSACleanup();
        return -1;
    }

    server->size = serverSize;
    
    //Set socket to non blocking mode
    u_long mode = 1;
    if (ioctlsocket(server->socket, FIONBIO, &mode) != NO_ERROR) {
        std::cout << "Failed to set non-blocking mode: " << WSAGetLastError() << std::endl;
        closesocket(server->socket);
        WSACleanup();
        return -1;
    }

    std::cout << "Server created on port; " << PORT << std::endl;

    return 0;
}

ConnectionResult SocketServerHandleConnections(ServerSocket* server){
    ConnectionResult result = {};
    
    sockaddr_in client_address;
    int client_length = sizeof(client_address);

    SOCKET client_socket = accept(server->socket, (sockaddr*)&client_address, &client_length);
    
    if (client_socket == INVALID_SOCKET) {
        int err = WSAGetLastError();
        if (err != WSAEWOULDBLOCK) {
            std::cout << "Accept failed: " << err << std::endl;
            result.code = -1;
        }
        // No new connection right now (would block)
        return result;
    }

    ClientSocket newClient;
    newClient.socket = client_socket;
    newClient.address = client_address;

    result.client = newClient;
    result.code = 1;

    std::cout << "New client connected, socket: " << client_socket << std::endl;

    return result;
} 

SendResult SocketServerSend(ClientSocket* client, const char* data, int dataSize) {
    SendResult result = {};

    int dataSent = send(client->socket, data, dataSize, 0);
    
    if (dataSent == SOCKET_ERROR) {
        result.success = false;
        result.dataSent = 0;
        result.errorCode = WSAGetLastError(); // Only on Windows
        return result;
    }

    result.success = true;
    result.dataSent = dataSent;
    result.errorCode = 0;
    return result;
}

SendResult SocketClientSend(ClientSocket* client, const char* data, int dataSize) {
    SendResult result = {};

    int dataSent = send(client->socket, data, dataSize, 0);
    
    if (dataSent == SOCKET_ERROR) {
        result.success = false;
        result.dataSent = 0;
        result.errorCode = WSAGetLastError(); // Only on Windows
        return result;
    }

    result.success = true;
    result.dataSent = dataSent;
    result.errorCode = 0;
    return result;
}

void SocketServerReceive(ClientSocket* client, char* buffer, int bufferSize){
    //TODO add encryption later
    
}


void SocketServerCloseConnection(ClientSocket* client){
    std::cout << "Closing connection for socket: " << client->socket << std::endl;
    closesocket(client->socket);
}


void SocketServerClose(ServerSocket* server){
    closesocket(server->socket);
    WSACleanup();
}

int SocketClientConnectServer(ClientSocket* client, std::string serverIP, int serverPORT, bool isSinglePlayer)  {
    int result = 0;
    if(!isSinglePlayer){
        WSADATA wsa;
        int result = WSAStartup(MAKEWORD(2, 2), &wsa);
        if (result != 0) {
            std::cout << "WSAStartup failed: " << result << std::endl;
            return result;
        }
    }

    client->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client->socket == INVALID_SOCKET) {
        std::cout << "Socket creation failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return result;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(serverPORT);
    serverAddress.sin_addr.s_addr = inet_addr(serverIP.c_str());

    u_long mode = 1;
    ioctlsocket(client->socket, FIONBIO, &mode);

    result = connect(client->socket, (sockaddr*)&serverAddress, sizeof(serverAddress));
    if (result == SOCKET_ERROR) {
        int err = WSAGetLastError();
        if (err != WSAEWOULDBLOCK && err != WSAEINPROGRESS) {
            // Real error
            std::cout << "Connect failed: " << err << std::endl;
            closesocket(client->socket);
            WSACleanup();
            return result;
        }
    }

    return result;  
}

#endif