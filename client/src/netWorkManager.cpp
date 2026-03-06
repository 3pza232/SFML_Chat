#include "head/function.h"
#include "head/netWorkManager.h"
#include <cstring>
#include <iostream>

NetWorkManager::NetWorkManager(sf::IpAddress serverIP, unsigned short port)
    : serverIP(serverIP)
    , port(port) {}

NetWorkManager::~NetWorkManager() {
    client.disconnect();
    std::cout << "断开连接……" << std::endl;
}

bool NetWorkManager::beginConnectServer() {
    sf::Socket::Status status = client.connect(serverIP, port);
    if (status != sf::Socket::Status::Done) {
        return false;
    }
    client.setBlocking(false);
    return true;
}

std::wstring NetWorkManager::receviedMessage() {
    char receviedBuffer[1024];
    memset(receviedBuffer, 0, sizeof(receviedBuffer));
    std::size_t recevied = 0;

    sf::Socket::Status status = client.receive(receviedBuffer, sizeof(receviedBuffer), recevied);
    if (status == sf::Socket::Status::Done) {
        if (recevied > 1) {
            return charToWstring(receviedBuffer);
        }
    }

    return L"";
}

bool NetWorkManager::sendMessageToServer(std::wstring message) {
    client.setBlocking(true);
    sf::String sfStr(message);
    sf::Socket::Status status =
        client.send(sfStr.toAnsiString().c_str(), sfStr.toAnsiString().length());
    client.setBlocking(false);
    if (status != sf::Socket::Status::Done) {
        return false;
    }
    return true;
}
