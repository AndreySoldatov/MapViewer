#pragma once

#include <SFML/Graphics.hpp>

#include <utils.hpp>

class Line : public sf::Drawable {
    sf::Vector2f m_p1{};
    sf::Vector2f m_p2{};
    float m_thickness{5};
    sf::ConvexShape m_shape{};

    void update();

public:

    Line(sf::Vector2f const &p1, sf::Vector2f const &p2, float thickness = 5);

    void setPoint1(sf::Vector2f const &vec);

    void setPoint2(sf::Vector2f const &vec);

    const sf::Vector2f &getPoint1() const;

    const sf::Vector2f &getPoint2() const;

    const float &getThickness() const;

    void setThickness(float thickness);

    const sf::ConvexShape &getShape() const;

    void setFillColor(sf::Color const &col);

    void setOutlineColor(sf::Color const &col);

    void setOutlineThickness(float thickness);

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};