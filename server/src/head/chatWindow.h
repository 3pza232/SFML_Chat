#pragma once
#include "netWorkManager.h"
#include <SFML/Graphics.hpp>

class ChatWindow {
private:
    sf::RenderWindow window;
    float chatTextPosX;
    float nowChatTextPosY;
    float chatTextYInterval; // 文本之间的纵轴间隙
    int fontSize;

    // 字体
    sf::Font font;

    // 聊天输入框
    sf::RectangleShape inputBox;

    // 输入框文字设定
    sf::Text inputText;

    // 输入缓冲区
    std::wstring inputBuffer;

    // 聊天区记录
    std::vector<std::unique_ptr<sf::Text>> chatTexts;

public:
    ChatWindow(int windowWidth, int windowHeight, std::wstring windowTitle, float chatTextPosX,
               float chatTextYInterval);
    void serverInterface(NetWorkManager &NetWorkManager);
};
