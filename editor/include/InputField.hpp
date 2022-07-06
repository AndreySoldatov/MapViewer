#pragma once

#include <SFML/Graphics.hpp>
#include <RoundRect.hpp>

class InputField : public sf::Drawable {
    RoundRect m_padding;
    sf::Text m_text;
    std::wstring m_defaultString{L"Дефолт"};
    std::wstring m_string{};
    bool m_active{};

public:
    InputField(sf::Font const &font, sf::Vector2f pos, std::wstring const &defstr = L"Дефолт");

    void update(sf::Event const &e, sf::RenderWindow const &window);

    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

    const std::wstring &getString() const;

};