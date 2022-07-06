#pragma once

#include <RoundRect.hpp>
#include <SFML/Graphics.hpp>
#include <functional>

class Button : public sf::Drawable {
    RoundRect m_padding;
    sf::Text m_text;

    std::function<void(void)> m_onClick;

public:

    Button(sf::Font const &font, sf::Vector2f const &pos, std::wstring const &text, std::function<void(void)> onClick);

    void update(sf::Event const &e, sf::RenderWindow const &window);

    void draw(sf::RenderTarget &target, sf::RenderStates states) const; 
};
