#pragma once

#include <SFML/Network.hpp>

class NetWorkManager {
private:
    sf::TcpSocket client;
    sf::IpAddress serverIP;
    unsigned short port;

public:
    NetWorkManager(sf::IpAddress serverIP, unsigned short port);
    ~NetWorkManager();
    bool beginConnectServer();
    std::wstring receviedMessage();
    bool sendMessageToServer(std::wstring message);
};
