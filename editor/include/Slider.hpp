#pragma once

#include <SFML/Graphics.hpp>
#include <Line.hpp>

class Slider : public sf::Drawable {
    Line m_line;
    sf::CircleShape m_cursor;
    sf::Vector2f m_pos;

public:
    Slider(sf::Vector2f pos);

    void update(sf::RenderWindow const &window);

    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

    float getValue() const;
};