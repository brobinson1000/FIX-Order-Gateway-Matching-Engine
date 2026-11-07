#pragma once
#include <string>

class TCPClient {
public:

    TCPClient()
    
    ~TCPClient();

    TCPClient(const TCPClient&) = delete;
    TCPClient& operator = (const TCPClient&) = delete;
    
    void disconnect();

    bool ConnectToServer(const std::string& ipAddress, uint16_t port);
    
    bool SendFIXMsg(const std::string& message);

private:
    int clientSocket;
    bool isConnected;
};

