#include "head/chatWindow.h"
#include "head/function.h"
#include <cstring>
#include <iostream>
#include <sstream>

ChatWindow::ChatWindow(int windowWidth, int windowHeight, std::wstring windowTitle,
                       float chatTextPosX, float chatTextYInterval)
    : chatTextPosX(chatTextPosX)
    , nowChatTextPosY(chatTextYInterval)
    , chatTextYInterval(chatTextYInterval)
    , fontSize(26) {

    window.create(sf::VideoMode(windowWidth, windowHeight), windowTitle, sf::Style::Default);

    inputBox.setSize(sf::Vector2f(windowWidth - 2 * chatTextPosX, chatTextYInterval));
    inputBox.setFillColor(HEX_TO_COLOR(0x2a3824, 200));
    inputBox.setOutlineThickness(2.f);
    inputBox.setOutlineColor(HEX_TO_COLOR(0xab441f, 200));
    inputBox.setPosition(
        sf::Vector2f(chatTextPosX, windowHeight - chatTextYInterval - chatTextPosX));

    if (!font.loadFromFile("resources/fonts/SourceHanSansSC-Normal-2.otf")) {
        std::cout << "字体文件缺失！" << std::endl;
        throw "字体文家缺失！";
    }
    inputText.setFont(font);
    inputText.setFillColor(HEX_TO_COLOR(0xffafab, 255));
    inputText.setCharacterSize(fontSize);
    inputText.setPosition(
        sf::Vector2f(chatTextPosX + 10, windowHeight - chatTextYInterval - chatTextPosX + 4));
}

void ChatWindow::serverInterface(NetWorkManager &netWorkManager) {
    sf::Clock clock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::BackSpace) {
                    if (!inputBuffer.empty()) {
                        inputBuffer.pop_back();
                    }
                }
            }
            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode >= 32 && event.text.unicode != 127) {
                    inputBuffer += static_cast<char>(event.text.unicode);
                }
            }
        }

        window.clear(HEX_TO_COLOR(0x538f6a, 255));
        window.draw(inputBox);

        float cursorTime = clock.getElapsedTime().asSeconds();
        std::wstring cursor = (static_cast<int>(cursorTime * 2) % 2 == 0) ? L"_" : L"";
        std::wstring inputString = inputBuffer + cursor;
        inputText.setString(inputString);
        window.draw(inputText);

        // 接受新客户端
        if (netWorkManager.acceptClients()) {
            int lastAcceptClientId = netWorkManager.getClients().rbegin()->first;
            sf::IpAddress lastAcceptClientIP =
                netWorkManager.getClients().rbegin()->second->getRemoteAddress();

            auto acceptClientText = std::make_unique<sf::Text>();
            acceptClientText->setFont(font);
            acceptClientText->setCharacterSize(fontSize);
            acceptClientText->setFillColor(HEX_TO_COLOR(0x000000, 255));

            std::wstringstream ss;
            ss << L"客户端" << " " << lastAcceptClientId << " : "
               << lastAcceptClientIP.toString().c_str() << L" 连接成功，当前在线客户端数量: "
               << netWorkManager.getClients().size();
            std::wstring acceptClientString = ss.str();
            acceptClientText->setString(acceptClientString);

            if (nowChatTextPosY >= window.getSize().y - 2 * chatTextYInterval - chatTextPosX) {
                nowChatTextPosY = chatTextYInterval;
                chatTexts.clear();
            }
            acceptClientText->setPosition(sf::Vector2f(chatTextPosX, nowChatTextPosY));
            nowChatTextPosY += chatTextYInterval;

            chatTexts.emplace_back(std::move(acceptClientText));
        }

        // 查收客户端发送的消息
        auto receviedMessages = netWorkManager.receviedMessage();
        if (!receviedMessages.empty()) {
            int count = 0;
            std::wstringstream ss;
            for (const auto &recevied : receviedMessages) {
                count++;
                ss << recevied.first << ": " << recevied.second << "\n";
            }

            auto receviedMessagesText = std::make_unique<sf::Text>();
            receviedMessagesText->setFont(font);
            receviedMessagesText->setCharacterSize(fontSize);
            receviedMessagesText->setFillColor(HEX_TO_COLOR(0x000000, 255));
            receviedMessagesText->setString(ss.str());

            if (nowChatTextPosY >=
                window.getSize().y - (1 + count) * chatTextYInterval - chatTextPosX) {
                nowChatTextPosY = chatTextYInterval;
                chatTexts.clear();
            }
            receviedMessagesText->setPosition(sf::Vector2f(chatTextPosX, nowChatTextPosY));
            nowChatTextPosY += (count * chatTextYInterval);

            chatTexts.emplace_back(std::move(receviedMessagesText));
        }

        // 给用户发送消息
        if (!inputBuffer.empty() && sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {

            auto sendText = std::make_unique<sf::Text>();
            sendText->setFont(font);
            sendText->setCharacterSize(fontSize);

            if (inputBuffer == L"exit") {
                sendText->setFillColor(HEX_TO_COLOR(0x0000ff, 255));
                sendText->setString(L"退出程序，欢迎下次使用！");
            } else if (!netWorkManager.sendMessage(inputBuffer)) {
                sendText->setFillColor(HEX_TO_COLOR(0xff0000, 255));
                sendText->setString(L"发送失败！请检查用户是否存在，消息格式:'userId message'");
            } else {
                sendText->setFillColor(HEX_TO_COLOR(0x000000, 255));
                std::wstring inputText = L"send->" + inputBuffer;
                sendText->setString(inputText);
            }

            if (nowChatTextPosY >= window.getSize().y - 2 * chatTextYInterval - chatTextPosX) {
                nowChatTextPosY = chatTextYInterval;
                chatTexts.clear();
            }
            sendText->setPosition(sf::Vector2f(chatTextPosX, nowChatTextPosY));
            nowChatTextPosY += chatTextYInterval;

            chatTexts.emplace_back(std::move(sendText));
            if (inputBuffer == L"exit") {
                for (auto &chatText : chatTexts) {
                    window.draw(*chatText);
                }
                window.display();
                sf::sleep(sf::milliseconds(2000));
                exit(0);
            }
            inputBuffer.clear();
        }

        // 最后检测断开的
        auto disconnectedIPs = netWorkManager.checkDisconnected();
        if (!disconnectedIPs.empty()) {
            std::wstringstream ss;
            ss << L"客户端断开连接:\n";
            for (const auto &ip : disconnectedIPs) {
                ss << L"  " << ip << L"号  ";
            }
            ss << L"\n当前在线客户端数量: " << netWorkManager.getClients().size();

            auto disconnectedClientsText = std::make_unique<sf::Text>();
            disconnectedClientsText->setFont(font);
            disconnectedClientsText->setCharacterSize(fontSize);
            disconnectedClientsText->setFillColor(HEX_TO_COLOR(0xff0000, 255));
            disconnectedClientsText->setString(ss.str());

            if (nowChatTextPosY >= window.getSize().y - 4 * chatTextYInterval - chatTextPosX) {
                nowChatTextPosY = chatTextYInterval;
                chatTexts.clear();
            }
            disconnectedClientsText->setPosition(sf::Vector2f(chatTextPosX, nowChatTextPosY));
            nowChatTextPosY += (3 * chatTextYInterval);

            chatTexts.emplace_back(std::move(disconnectedClientsText));
        }

        // 聊天面板消息显示
        for (auto &chatText : chatTexts) {
            window.draw(*chatText);
        }

        window.display();
    }
}
