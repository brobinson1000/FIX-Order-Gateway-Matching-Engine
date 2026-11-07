#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>


class TCPClient {
public:

    TCPClient() : isConnected(false), clientSocket(-1) {}
    
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

        if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
            std::cerr << "Connection failed\n";
            close(clientSocket);
            clientSocket = -1;
            return false;
        }

        isConnected = true;
        return true;
    }

    
    bool SendFIXMsg(const std::string& message) {
        if (!isConnected || clientSocket == -1) {
            std::cerr << "Cannot connect to server\n";
            return false;
        }

        const char* ptr = message.data();
        ssize_t bytes_left = message.size();

        while(bytes_left > 0) {
            ssize_t bytes_sent = send(clientSocket, ptr, bytes_left, MSG_NOSIGNAL);
            
            if (bytes_sent < 0) {
                if (errno == EINTR) {
                    continue;
                }

                std::cerr << "Socket send error encountered\n";
                disconnect();
                return false;
            } else if (bytes_sent == 0) {
                std::cerr << "Server disconnected cleanly while sending.\n";
                disconnect();
                return false;
            }

            ptr += bytes_sent;
            bytes_left -= bytes_sent;
        }
        return true;
    }
                

private:
    int clientSocket;
    bool isConnected;
};

int main() {
    TCPClient tcpc;
    tcpc.ConnectToServer("127.0.0.1", 80);
    std::string a = "ILOVEGOD";
    tcpc.SendFIXMsg(std::move(a));
}
