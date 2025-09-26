#ifndef SOCKET_WINDOWS
#define SOCKET_WINDOWS

#include "common.cpp"

struct SendResult {
    bool success;
    int dataSent;
    int errorCode; // 0 on success, or WSAGetLastError() on failure
};

struct ClientSocket{
public:    
    SOCKET m_socket;
    sockaddr_in m_address;
    
    int ConnectToServer(std::string serverIP, int serverPORT, bool isSinglePlayer)  {
        int result = 0;
        if(!isSinglePlayer){
            WSADATA wsa;
            int result = WSAStartup(MAKEWORD(2, 2), &wsa);
            if (result != 0) {
                std::cout << "WSAStartup failed: " << result << std::endl;
                return result;
            }
        }

        m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (m_socket == INVALID_SOCKET) {
            std::cout << "Socket creation failed: " << WSAGetLastError() << std::endl;
            WSACleanup();
            return result;
        }

        sockaddr_in serverAddress;
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(serverPORT);
        serverAddress.sin_addr.s_addr = inet_addr(serverIP.c_str());

        u_long mode = 1;
        ioctlsocket(m_socket, FIONBIO, &mode);

        result = connect(m_socket, (sockaddr*)&serverAddress, sizeof(serverAddress));
        if (result == SOCKET_ERROR) {
            int err = WSAGetLastError();
            if (err != WSAEWOULDBLOCK && err != WSAEINPROGRESS) {
                // Real error
                std::cout << "Connect failed: " << err << std::endl;
                closesocket(m_socket);
                WSACleanup();
                return result;
            }
        }

        return result;  
    }
    
    SendResult Send(const char* data, int dataSize) {
        SendResult result = {};

        int dataSent = send(m_socket, data, dataSize, 0);
        
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

    void Close(bool isSinglePlayer){
        closesocket(m_socket);
        if(!isSinglePlayer) WSACleanup();
    }

};

struct ConnectionResult{
    int code;
    ClientSocket socket;
};

class ServerSocket{
public:
    SOCKET m_socket;
    sockaddr_in m_address;
    uint16_t m_size;

    int Create(uint16_t serverSize, int PORT, bool isSinglePlayer){
        WSADATA wsa;

        if (WSAStartup(MAKEWORD(2,2), &wsa) != 0){
            std::cout << "Failed to init wsa: " << WSAGetLastError();
            return -1;
        }

        if((m_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET){
            std::cout << "Could not create socket: " << WSAGetLastError();
            WSACleanup();        
            return -1;
        }

        m_address.sin_family = AF_INET;
        m_address.sin_port = htons(PORT);

        m_address.sin_addr.s_addr = INADDR_ANY;
        if (isSinglePlayer) m_address.sin_addr.s_addr = inet_addr("127.0.0.1");

        if (bind(m_socket, (sockaddr*)&m_address, sizeof(m_address)) == SOCKET_ERROR){
            std::cout << "Could not bind socket: " << WSAGetLastError();
            closesocket(m_socket);
            WSACleanup();
            return -1;
        }

        if (listen(m_socket, serverSize) == SOCKET_ERROR){
            std::cout << "Listen failed: " << WSAGetLastError();
            closesocket(m_socket);
            WSACleanup();
            return -1;
        }

        m_size = serverSize;
        
        //Set socket to non blocking mode
        u_long mode = 1;
        if (ioctlsocket(m_socket, FIONBIO, &mode) != NO_ERROR) {
            std::cout << "Failed to set non-blocking mode: " << WSAGetLastError() << std::endl;
            closesocket(m_socket);
            WSACleanup();
            return -1;
        }

        std::cout << "Server created on port; " << PORT << std::endl;

        return 0;
    }

    ConnectionResult HandleConnections(){
        ConnectionResult result = {};
        
        sockaddr_in client_address;
        int client_length = sizeof(client_address);

        SOCKET client_socket = accept(m_socket, (sockaddr*)&client_address, &client_length);
        
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
        newClient.m_socket = client_socket;
        newClient.m_address = client_address;

        result.socket = newClient;
        result.code = 1;

        std::cout << "New client connected, socket: " << client_socket << std::endl;

        return result;
    } 

    SendResult Send(ClientSocket* client, const char* data, int dataSize) {
        SendResult result = {};

        int dataSent = send(client->m_socket, data, dataSize, 0);
        
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

    void Receive(ClientSocket* client, char* buffer, int bufferSize){
        //TODO add encryption later
    }

    void CloseConnection(ClientSocket* client){
        std::cout << "Closing connection for socket: " << client->m_socket << std::endl;
        closesocket(client->m_socket);
    }

    void Close(){
        closesocket(m_socket);
        WSACleanup();
    }

};

#endif