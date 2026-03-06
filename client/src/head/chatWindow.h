#pragma once

#include "netWorkManager.h"
#include <SFML/Graphics.hpp>
#include <memory>

class ChatWindow {
private:
    sf::RenderWindow window;
    float chatTextPosX;
    float chatTextYInterval;
    float chatTextNowPosY;

    sf::Font font;
    int fontSize;

    sf::RectangleShape inputBox;
    sf::Text inputBoxText;
    std::wstring inputBuffer;

    std::vector<std::unique_ptr<sf::Text>> chatTexts;

public:
    ChatWindow(int windowWidth, int windowHeight, std::wstring windowTitle, float chatTextPosX,
               float chatTextYInterval);
    void clientInterface(NetWorkManager &netWorkManager);
};
