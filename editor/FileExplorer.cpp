#include <FileExplorer.hpp>

FileExplorer::FileExplorer(std::string const &initPath, sf::Font const &font, sf::Vector2f const &pos) : m_font(font) {
    m_pos = pos;
    m_active_dir = initPath;
    m_active_dir = m_active_dir.parent_path();

    for (auto &p : fs::directory_iterator(m_active_dir)) {
        if(!p.is_directory()) {
            m_active_file = p;
            break;
        }
    }
}

void FileExplorer::updateInput(sf::Event const &event, float dt) {
    if(event.type == sf::Event::MouseWheelScrolled) {
        float size{};
        for ([[maybe_unused]]auto &p : fs::directory_iterator(m_active_dir)) {
            size += 1.0f;
        }
        
        if(m_verticalOffset >= -70.0f * (size - 1.0f) && event.mouseWheelScroll.delta < 0) {
            m_verticalOffsetSpeed += event.mouseWheelScroll.delta * 128.0f * dt;
        }
        if(m_verticalOffset <= 70.f * (size - 1.0f) &&  event.mouseWheelScroll.delta > 0) {
            m_verticalOffsetSpeed += event.mouseWheelScroll.delta * 128.0f * dt;
        }
    }

    if(event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left &&
        event.mouseButton.x > m_pos.x && event.mouseButton.x < m_pos.x + 600.0f
    ) {
        if(event.mouseButton.y - m_verticalOffset > 0.0f && event.mouseButton.y - m_verticalOffset < 70.0f) {
            m_active_dir = m_active_dir.parent_path();
            for (auto &p : fs::directory_iterator(m_active_dir)) {
                if(!p.is_directory()) {
                    m_active_file = p;
                    break;
                }
            }
            m_verticalOffset = 0.0f;
        } else {
            float index{1};
            for (auto &path : fs::directory_iterator(m_active_dir)) {
                if(
                    event.mouseButton.y - m_verticalOffset > index * 70.0f && 
                    event.mouseButton.y - m_verticalOffset < (index + 1) * 70.0f
                ) {
                    if(path.is_directory()) {
                        m_active_dir = path;
                        for (auto &p : fs::directory_iterator(m_active_dir)) {
                            if(!p.is_directory()) {
                                m_active_file = p;
                                break;
                            }
                        }
                        m_verticalOffset = 0.0f;
                    } else {
                        m_active_file = path;
                    }
                }
                index++;
            }
        }
    }
}

void FileExplorer::update(float dt) {
    m_verticalOffsetSpeed *= (1.0f - 8.0f * dt);
    m_verticalOffset += m_verticalOffsetSpeed;
}

std::wstring FileExplorer::getActivePath() const {
    return m_active_file.wstring();
}

void FileExplorer::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    sf::RectangleShape border0;
    border0.setPosition({m_pos.x, m_pos.y + m_verticalOffset});
    border0.setSize({600, 70});
    border0.setFillColor(sf::Color(230, 230, 230));
    border0.setOutlineColor(sf::Color(100, 100, 100));
    border0.setOutlineThickness(1);
    target.draw(border0, states);

    sf::Text dots;
    dots.setCharacterSize(30.0f);
    dots.setFont(m_font);
    dots.setFillColor(sf::Color(100, 100, 100));
    dots.setPosition({m_pos.x + 20, m_pos.y + 16 + m_verticalOffset});
    dots.setString("...");
    target.draw(dots, states);

    float index{1};
    for (auto &entity : fs::directory_iterator(m_active_dir)) {
        sf::RectangleShape border;
        border.setPosition({m_pos.x, m_pos.y + index * 70.0f + m_verticalOffset});
        border.setSize({600, 70});
        if(entity == m_active_file) {
            border.setFillColor(sf::Color(200, 200, 200));
        } else {
            border.setFillColor(sf::Color(230, 230, 230));
        }
        border.setOutlineColor(sf::Color(100, 100, 100));
        border.setOutlineThickness(1);
        target.draw(border, states);

        sf::Text txt;
        txt.setCharacterSize(30.0f);
        txt.setFont(m_font);
        if(entity.path().filename().string() == "map.json") {
            txt.setFillColor(sf::Color(80, 160, 80));
        } else {
            txt.setFillColor(sf::Color(100, 100, 100));
        }
        if(entity.is_directory()) {
            txt.setPosition({m_pos.x + 90, m_pos.y + 16 + index * 70.0f + m_verticalOffset});
            
            sf::RectangleShape folderIconBase;
            folderIconBase.setPosition({m_pos.x + 20, m_pos.y + index * 70.0f + 20 + m_verticalOffset});
            folderIconBase.setSize({50, 30});
            folderIconBase.setFillColor(sf::Color(200, 200, 200));
            target.draw(folderIconBase, states);

            sf::RectangleShape folderIconCorner;
            folderIconCorner.setPosition({m_pos.x + 20, m_pos.y + index * 70.0f + 15 + m_verticalOffset});
            folderIconCorner.setSize({20, 5});
            folderIconCorner.setFillColor(sf::Color(200, 200, 200));
            target.draw(folderIconCorner, states);
        } else {
            txt.setPosition({m_pos.x + 20, m_pos.y + 16 + index * 70.0f + m_verticalOffset});
        }
        txt.setString(entity.path().filename().wstring());

        target.draw(txt, states);
        index += 1.0f;
    }
}