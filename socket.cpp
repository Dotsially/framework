#ifndef SOCKET_WINDOWS
#define SOCKET_WINDOWS

#include "common.cpp"

struct ServerSocket{
    WSADATA wsa;
    SOCKET socket;
    sockaddr_in address;
    uint16_t size;
};

int ServerSocketCreate(ServerSocket* server, uint16_t serverSize, int PORT){
    if (WSAStartup(MAKEWORD(2,2), &server->wsa) != 0){
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
    ioctlsocket(server->socket, FIONBIO, &mode);
    std::cout << "Server created" << std::endl;

    return 0;
}


int ServerHandleConnections(ServerSocket* server){
    // TODO 
    // client_socket = accept(server->socket, (sockaddr*)&client_address, &client_length);
    // if (client_socket == INVALID_SOCKET){
    //     printf("Accept failed: %d", WSAGetLastError());
    //     closesocket(server_socket);
    //     WSACleanup();
    //     return 1;
    // }


    return 0;
} 

int ServerCloseConnection(ServerSocket* server){
    return 0;
}


void ServerClose(ServerSocket* server){
    closesocket(server->socket);
    WSACleanup();
}

#endif