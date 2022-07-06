#include <Button.hpp>

Button::Button(sf::Font const &font, sf::Vector2f const &pos, std::wstring const &text, std::function<void(void)> onClick) {
    m_padding.setFillColor(sf::Color(230, 230, 230));
    m_padding.setPosition(pos);

    m_text.setFont(font);
    m_text.setString(text);
    m_text.setCharacterSize(30);
    m_text.setFillColor(sf::Color(100, 100, 100));
    m_text.setPosition({pos.x + 20, pos.y + 10.0f});
    
    m_padding.setSize({m_text.getGlobalBounds().width + 40, 60});
    m_padding.setRoundingPercent(40.0f);
    
    m_onClick = onClick;
}

void Button::update(sf::Event const &e, sf::RenderWindow const &window) {
    sf::Vector2f mousePos = {(float)sf::Mouse::getPosition(window).x, (float)sf::Mouse::getPosition(window).y};

    if(
            mousePos.x >= m_padding.getPosition().x && mousePos.x <= m_padding.getPosition().x + m_padding.getSize().x &&
            mousePos.y >= m_padding.getPosition().y && mousePos.y <= m_padding.getPosition().y + m_padding.getSize().y
    ) {
        if(e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
            m_padding.setFillColor(sf::Color(200, 200, 200));
        } else if(e.type == sf::Event::MouseButtonReleased && e.mouseButton.button == sf::Mouse::Left) {
            m_padding.setFillColor(sf::Color(230, 230, 230));
            m_onClick();
        }
    }
}

void Button::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    target.draw(m_padding, states);
    target.draw(m_text, states);
}