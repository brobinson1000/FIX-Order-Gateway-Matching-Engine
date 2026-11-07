#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <string>
#include "../include/TCPServer.h"

TCPClient::TCPClient() : isConnected(false), clientSocket(-1) {}

TCPClient::~TCPClient() { disconnect(); }

void TCPClient::disconnect() {
  if (clientSocket != -1) {
    close(clientSocket);
    clientSocket = -1;
  }

  isConnected = false;
}

bool TCPClient::ConnectToServer(const std::string& ipAddress, uint16_t port) {
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

  if (connect(clientSocket, (struct sockaddr*)&serverAddress,
              sizeof(serverAddress)) < 0) {
    std::cerr << "Connection failed\n";
    close(clientSocket);
    clientSocket = -1;
    return false;
  }

  isConnected = true;
  return true;
}

bool TCPClient::SendFIXMsg(const std::string& message) {
  if (!isConnected || clientSocket == -1) {
    std::cerr << "Cannot connect to server\n";
    return false;
  }

  const char* ptr = message.data();
  ssize_t bytes_left = message.size();

  while (bytes_left > 0) {
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
