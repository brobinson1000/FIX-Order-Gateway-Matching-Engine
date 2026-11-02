#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>


class TCPClient {
public:

    TCPClient(const char* FilePath) : TestingFilePath(FilePath) {}
    
    ~TCPClient() { disconnect(); }

    TCPClient(const TCPClient&) = delete;
    TCPClient& operator = (const TCPClient&) = delete;
    
    void disconnect() {
        if (clientSocket != -1) {
            close(clientSocket);
            clientSocket = -1;
        }

        isConnected = false;
    }

    bool ConnectToServer(const std::string& ipAddress, uint16_t port) {
        if (isConnected) {
            std::clog << "Can not connect to port " << port << ". Already Connected!\n";
            return false;
        }

        clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket == -1) {
            std::cerr << "Failed to create a socket\n";
            return false;
        }

        sockaddr_in serverAddress{};
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(port);
        
        if (inet_pton(AF_INET, ipAddress.c_str(), &serverAddress.sin_addr) <= 0) {
            std::cerr << "Not a valid IPV4 address\n";
            close(clientSocket);
            clientSocket = -1;
            return false;
        }

        isConnect = true;
        return true;
    }




private:
    int clientSocket;
    bool isConnected;
};
