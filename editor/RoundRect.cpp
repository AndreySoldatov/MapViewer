#include <RoundRect.hpp>

#include <utils.hpp>

void RoundRect::update() {
    float radius = std::min(m_size.x, m_size.y) * 0.5f * (m_roundingPercent * 0.01f);
    int cornerPointCount = ((int)(m_roundingPercent * 0.3f) + 2);
    m_shape.setPointCount(cornerPointCount * 4);

    for (int i = 0; i < cornerPointCount; i++) {
        float a = (float)(i) / (float)(cornerPointCount - 1);
        a = a * PI * 0.5f;
        float cosVal = radius * std::cos(a);
        float sinVal = radius * std::sin(a);
        m_shape.setPoint(i, add(m_pos, {-cosVal + radius, -sinVal + radius}));
        m_shape.setPoint((cornerPointCount - i - 1) + cornerPointCount, add(m_pos, {cosVal + m_size.x - radius, -sinVal + radius}));
        m_shape.setPoint(i + cornerPointCount * 2, add(m_pos, {cosVal + m_size.x - radius, sinVal - radius + m_size.y}));
        m_shape.setPoint((cornerPointCount - i - 1) + cornerPointCount * 3, add(m_pos, {-cosVal + radius, sinVal - radius + m_size.y}));
    }
}

RoundRect::RoundRect(sf::Vector2f const &pos, sf::Vector2f const &size, float rp)
    : m_pos(pos), m_size(size), m_roundingPercent(std::max(std::min(rp, 100.0f), 0.0f)) {
    update();
}

void RoundRect::setPosition(sf::Vector2f const &pos) {
    m_pos = pos;
    update();
}

void RoundRect::setSize(sf::Vector2f const &size) {
    m_size = size;
    update();
}

void RoundRect::setRoundingPercent(float rp) {
    m_roundingPercent = std::max(std::min(rp, 100.0f), 0.0f);
    update();
}

void RoundRect::setFillColor(sf::Color const &col) {
    m_shape.setFillColor(col);
}

void RoundRect::setOutlineColor(sf::Color const &col) {
    m_shape.setOutlineColor(col);
}

void RoundRect::setOutlineThickness(float thickness) {
    m_shape.setOutlineThickness(thickness);
}

void RoundRect::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    target.draw(m_shape, states);
    
    // DEBUG
    // sf::RectangleShape rs;
    // rs.setSize(m_size);
    // rs.setPosition(m_pos);
    // rs.setOutlineColor(sf::Color::Red);
    // rs.setOutlineThickness(2);
    // rs.setFillColor(sf::Color::Transparent);
    // target.draw(rs);
}

const sf::Vector2f &RoundRect::getPosition() const {
    return m_pos;
}

const sf::Vector2f &RoundRect::getSize() const {
    return m_size;
}