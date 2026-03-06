#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <memory>

class NetWorkManager {
private:
    sf::TcpListener listener;
    unsigned short port;
    int clientId;
    std::map<int, std::unique_ptr<sf::TcpSocket>> clients;
    sf::SocketSelector selector;

public:
    NetWorkManager(unsigned short port);
    ~NetWorkManager();
    std::map<int, std::unique_ptr<sf::TcpSocket>> &getClients();
    sf::SocketSelector &getSocketSelector();
    bool beginListen();
    bool acceptClients();
    std::vector<int> checkDisconnected();
    std::map<int, std::wstring> receviedMessage();
    bool sendMessage(std::wstring message);
};
