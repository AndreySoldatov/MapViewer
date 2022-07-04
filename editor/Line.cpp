#include <Line.hpp>

void Line::update() {
    sf::Vector2f normal = mult(norm({-(m_p2.y - m_p1.y), m_p2.x - m_p1.x}), m_thickness * 0.5);
    
    m_shape.setPoint(0, add(m_p1, normal));
    m_shape.setPoint(1, add(m_p2, normal));
    m_shape.setPoint(2, sub(m_p2, normal));
    m_shape.setPoint(3, sub(m_p1, normal));
}

Line::Line(sf::Vector2f const &p1, sf::Vector2f const &p2, float thickness) : m_p1(p1), m_p2(p2), m_thickness(thickness) {
    m_shape.setPointCount(4);
    m_shape.setFillColor(sf::Color::White);

    update();
}

void Line::setPoint1(sf::Vector2f const &vec) {
    m_p1 = vec;

    update();
}

void Line::setPoint2(sf::Vector2f const &vec) {
    m_p2 = vec;

    update();
}

const sf::Vector2f &Line::getPoint1() const {
    return m_p1;
}

const sf::Vector2f &Line::getPoint2() const {
    return m_p2;
}

const float &Line::getThickness() const {
    return m_thickness;
}

void Line::setThickness(float thickness) {
    m_thickness = thickness;

    update();
}

const sf::ConvexShape &Line::getShape() const {
    return m_shape;
}

void Line::setFillColor(sf::Color const &col) {
    m_shape.setFillColor(col);
}

void Line::setOutlineColor(sf::Color const &col) {
    m_shape.setOutlineColor(col);
}

void Line::setOutlineThickness(float thickness) {
    m_shape.setOutlineThickness(thickness);
}

void Line::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(m_shape, states);
}