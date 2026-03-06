#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Text.hpp>
#include <cstdint>
#include <cstring>
#include <string>

inline sf::Color HEX_TO_COLOR(uint32_t hex, uint8_t alpha) {
    return sf::Color(static_cast<uint8_t>((hex >> 16) & 0xFF),
                     static_cast<uint8_t>((hex >> 8) & 0xFF), static_cast<uint8_t>(hex & 0xFF),
                     alpha);
}

inline std::wstring charToWstring(const char *str) {
    if (str == nullptr) return L"";
    sf::String sfStr = sf::String::fromUtf8(str, str + std::strlen(str));
    return sfStr.toWideString();
}
