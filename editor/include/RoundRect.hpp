#pragma once

#include <SFML/Graphics.hpp>

class RoundRect : public sf::Drawable {
    sf::Vector2f m_pos;
    sf::Vector2f m_size;
    sf::ConvexShape m_shape;
    float m_roundingPercent;

    void update();

public:
    RoundRect() = default;
    RoundRect(sf::Vector2f const &pos, sf::Vector2f const &size, float rp);
    
    void setPosition(sf::Vector2f const &pos);
    const sf::Vector2f &getPosition() const;
    
    void setSize(sf::Vector2f const &size);
    const sf::Vector2f &getSize() const;
    
    void setRoundingPercent(float rp);

    void setFillColor(sf::Color const &col);

    void setOutlineColor(sf::Color const &col);

    void setOutlineThickness(float thickness);

    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
};