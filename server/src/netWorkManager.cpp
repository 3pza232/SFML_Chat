#include "head/function.h"
#include "head/netWorkManager.h"
#include <iostream>
#include <sstream>

NetWorkManager::NetWorkManager(unsigned short port) : port(port), clientId(1) {
    std::cout << "准备监听端口:" << port << std::endl;
}

NetWorkManager::~NetWorkManager() {
    listener.close();
    std::cout << "停止监听……" << std::endl;
}

std::map<int, std::unique_ptr<sf::TcpSocket>> &NetWorkManager::getClients() {
    return clients;
}

sf::SocketSelector &NetWorkManager::getSocketSelector() {
    return selector;
}

bool NetWorkManager::beginListen() {
    sf::Socket::Status status = listener.listen(port);
    if (status != sf::Socket::Status::Done) {
        return false;
    }
    listener.setBlocking(false);
    selector.add(listener);
    return true;
}

// 接收新的
bool NetWorkManager::acceptClients() {
    auto client = std::make_unique<sf::TcpSocket>();
    sf::Socket::Status status = listener.accept(*client);
    if (status == sf::Socket::Status::Done) {
        client->setBlocking(false);
        selector.add(*client);
        clients.emplace(clientId++, std::move(client));
        return true;
    }
    return false;
}

std::vector<int> NetWorkManager::checkDisconnected() {
    std::vector<int> disconnectedIds;

    for (auto it = clients.begin(); it != clients.end();) {
        auto &client = *it;

        char checkBuffer[1];
        std::size_t recevied = 0;
        sf::Socket::Status status =
            client.second->receive(checkBuffer, sizeof(checkBuffer), recevied);
        if (status == sf::Socket::Status::Disconnected) {
            disconnectedIds.push_back(client.first);
            selector.remove(*client.second);
            it = clients.erase(it);
        } else {
            ++it;
        }
    }

    return disconnectedIds;
}

std::map<int, std::wstring> NetWorkManager::receviedMessage() {
    std::map<int, std::wstring> receviedMessageMap;

    for (auto &client : clients) {
        char receviedBuffer[1024];
        std::size_t recevied = 0;
        memset(receviedBuffer, 0, sizeof(receviedBuffer));
        sf::Socket::Status status =
            client.second->receive(receviedBuffer, sizeof(receviedBuffer), recevied);
        if (status == sf::Socket::Status::Done && recevied > 1) {
            receviedMessageMap.emplace(client.first, charToWstring(receviedBuffer));
        }
    }

    return receviedMessageMap;
}

bool NetWorkManager::sendMessage(std::wstring message) {
    std::wstringstream ss(message);
    int userId;
    std::wstring toUserMessage;
    ss >> userId;
    if (ss.fail()) {
        ss.clear();
        ss.str(message);
        std::getline(ss, toUserMessage);

        sf::String sfStr(toUserMessage);
        std::string ansiMessage = sfStr.toAnsiString();

        for (auto &client : clients) {
            client.second->setBlocking(true);
            sf::Socket::Status status =
                client.second->send(ansiMessage.c_str(), ansiMessage.length());
            client.second->setBlocking(false);
            if (status == sf::Socket::Status::Done) return true;
        }
        return false;
    } else {
        std::getline(ss >> std::ws, toUserMessage);

        sf::String sfStr(toUserMessage);
        std::string ansiMessage = sfStr.toAnsiString();

        for (auto &client : clients) {
            if (userId == client.first) {
                client.second->setBlocking(true);
                sf::Socket::Status status =
                    client.second->send(ansiMessage.c_str(), ansiMessage.length());
                client.second->setBlocking(false);
                if (status == sf::Socket::Status::Done) return true;
            }
        }
        return false;
    }
}
