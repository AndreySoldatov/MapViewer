#include <InputField.hpp>

InputField::InputField(sf::Font const &font, sf::Vector2f pos, std::wstring const &defstr) {
    m_padding.setSize({400, 60});
    m_padding.setPosition(pos);
    m_padding.setFillColor(sf::Color(200, 200, 200));

    m_text.setFont(font);
    m_text.setString(defstr);
    m_text.setCharacterSize(30);
    m_text.setFillColor(sf::Color(100, 100, 100));
    m_text.setPosition({pos.x + 20, pos.y + 12});

    m_defaultString = defstr;
}

void InputField::update(sf::Event const &e, sf::RenderWindow const &window) {
    sf::Vector2f mousePos = {(float)sf::Mouse::getPosition(window).x, (float)sf::Mouse::getPosition(window).y};
    if (e.type == sf::Event::MouseButtonReleased && e.mouseButton.button == sf::Mouse::Left) {
        if(
            mousePos.x >= m_padding.getPosition().x && mousePos.x <= m_padding.getPosition().x + m_padding.getSize().x &&
            mousePos.y >= m_padding.getPosition().y && mousePos.y <= m_padding.getPosition().y + m_padding.getSize().y
        ) {
            m_active = true;
            m_text.setString(m_string);

            m_padding.setFillColor(sf::Color(230, 230, 230));
        } else {
            m_active = false;
            if(m_string.empty()) {
                m_text.setString(m_defaultString);
            }
            m_padding.setFillColor(sf::Color(200, 200, 200));
        }
    }

    if(m_active && e.type == sf::Event::TextEntered) {
        if(e.text.unicode >= 0x30 && m_text.getGlobalBounds().width < m_padding.getSize().x - 60) {
            m_string += e.text.unicode;
        }
        if(e.text.unicode == 8 && !m_string.empty()) {
            m_string.erase(m_string.end() - 1);
        }
        m_text.setString(m_string);
    }
}

void InputField::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    target.draw(m_padding, states);
    target.draw(m_text, states);
}

const std::wstring &InputField::getString() const {
    return m_string;
}