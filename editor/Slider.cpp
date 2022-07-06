#include <Slider.hpp>

Slider::Slider(sf::Vector2f pos) : m_line(pos, add(pos, {300, 0}), 5) {
    m_line.setFillColor(sf::Color(200, 200, 200));

    m_cursor.setRadius(10);
    m_cursor.setFillColor(sf::Color(100, 100, 100));
    m_cursor.setPosition(sub(pos, {10, 10}));

    m_pos = pos;
}

void Slider::update(sf::RenderWindow const &window) {
    sf::Vector2f mousePos = {(float)sf::Mouse::getPosition(window).x, (float)sf::Mouse::getPosition(window).y};

    if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        if(
            mousePos.x >= m_line.getPoint1().x && mousePos.x <= m_line.getPoint2().x &&
            mousePos.y >= m_cursor.getPosition().y - 10 && mousePos.y <= m_cursor.getPosition().y + 2.0f * m_cursor.getRadius() + 10
        ) {
            m_cursor.setPosition({mousePos.x - 10, m_cursor.getPosition().y});
        }
    }
}

void Slider::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    target.draw(m_line, states);
    target.draw(m_cursor, states);
}

float Slider::getValue() const {
    return (m_cursor.getPosition().x - m_pos.x + 10) / 300.0f;
}