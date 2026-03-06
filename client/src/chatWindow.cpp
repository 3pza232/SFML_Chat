#include "head/chatWindow.h"
#include "head/function.h"
#include <iostream>

ChatWindow::ChatWindow(int windowWidth, int windowHeight, std::wstring windowTitle, float chatTextPosX,
                       float chatTextYInterval)
    : chatTextPosX(chatTextPosX)
    , chatTextYInterval(chatTextYInterval)
    , chatTextNowPosY(chatTextYInterval)
    , fontSize(26) {
    window.create(sf::VideoMode(windowWidth, windowHeight), windowTitle, sf::Style::Default);

    if (!font.loadFromFile("resources/fonts/SourceHanSansSC-Normal-2.otf")) {
        std::cout << "字体文件缺失！" << std::endl;
        throw "字体文家缺失！";
    }

    // 输入框文字设定
    inputBoxText.setFont(font);
    inputBoxText.setCharacterSize(fontSize);
    inputBoxText.setPosition(sf::Vector2f(chatTextPosX + 5, windowHeight - chatTextYInterval - chatTextPosX));
    inputBoxText.setFillColor(HEX_TO_COLOR(0xff9d0a, 255));

    // 输入框设定
    inputBox.setSize(sf::Vector2f(windowWidth - 2 * chatTextPosX, chatTextYInterval));
    inputBox.setPosition(sf::Vector2f(chatTextPosX, windowHeight - chatTextYInterval - chatTextPosX));
    inputBox.setFillColor(HEX_TO_COLOR(0x3b1b1c, 200));
    inputBox.setOutlineThickness(2.f);
    inputBox.setOutlineColor(HEX_TO_COLOR(0xff6e0d, 255));
}

void ChatWindow::clientInterface(NetWorkManager &netWorkManager) {
    sf::Clock clock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    window.close();
                }
                if (event.key.code == sf::Keyboard::Backspace) {
                    if (!inputBuffer.empty())
                        inputBuffer.pop_back();
                }
            }
            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode >= 32 && event.text.unicode != 127) {
                    inputBuffer += static_cast<char>(event.text.unicode);
                }
            }
        }

        window.clear(HEX_TO_COLOR(0x7aa000, 255));
        window.draw(inputBox);

        float cursorTime = clock.getElapsedTime().asSeconds();
        std::wstring cursor = (static_cast<int>(cursorTime * 2) % 2 == 0) ? L"_" : L"";
        std::wstring inputString = inputBuffer + cursor;

        inputBoxText.setString(inputString);
        window.draw(inputBoxText);

        // 接收服务器消息
        auto receviedBuffer = netWorkManager.receviedMessage();
        if (!receviedBuffer.empty()) {
            auto receviedText = std::make_unique<sf::Text>();
            receviedText->setFont(font);
            receviedText->setCharacterSize(fontSize);
            receviedText->setFillColor(HEX_TO_COLOR(0x000000, 255));
            std::wstring receviedShowText = L"[服务器]: " + receviedBuffer;
            receviedText->setString(receviedShowText);

            if (chatTextNowPosY >= window.getSize().y - 2 * chatTextYInterval - chatTextPosX) {
                chatTextNowPosY = chatTextYInterval;
                chatTexts.clear();
            }
            receviedText->setPosition(sf::Vector2f(chatTextPosX, chatTextNowPosY));
            chatTextNowPosY += chatTextYInterval;

            chatTexts.emplace_back(std::move(receviedText));
        }

        // 给服务器发送消息
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) && !inputBuffer.empty()) {
            auto sendText = std::make_unique<sf::Text>();
            sendText->setFont(font);
            sendText->setCharacterSize(fontSize);
            if (inputBuffer == L"exit") {
                sendText->setFillColor(HEX_TO_COLOR(0x0000ff, 255));
                sendText->setString(L"即将退出程序……");
            } else if (!netWorkManager.sendMessageToServer(inputBuffer)) {
                sendText->setFillColor(HEX_TO_COLOR(0xff0000, 255));
                sendText->setString(L"发送失败，可能与服务器已断开联系！(输入'exit'退出)");
            } else {
                sendText->setFillColor(HEX_TO_COLOR(0x000000, 255));
                std::wstring sendString = L"[你]: " + inputBuffer;
                sendText->setString(sendString);
            }

            if (chatTextNowPosY >= window.getSize().y - 2 * chatTextYInterval - chatTextPosX) {
                chatTextNowPosY = chatTextYInterval;
                chatTexts.clear();
            }
            sendText->setPosition(sf::Vector2f(chatTextPosX, chatTextNowPosY));
            chatTextNowPosY += chatTextYInterval;

            chatTexts.emplace_back(std::move(sendText));

            if (inputBuffer == L"exit") {
                for (auto &chatText : chatTexts) {
                    window.draw(*chatText);
                }
                window.display();
                sf::sleep(sf::milliseconds(2000)); // 等两秒再结束
                exit(0);
            }

            inputBuffer.clear();
        }

        for (auto &chatText : chatTexts) {
            window.draw(*chatText);
        }
        window.display();
    }
}
