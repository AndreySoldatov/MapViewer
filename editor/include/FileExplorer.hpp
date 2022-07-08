#pragma once

#include <SFML/Graphics.hpp>
#include <filesystem>

namespace fs = std::filesystem;

class FileExplorer : public sf::Drawable {
    const sf::Font &m_font;
    fs::path m_active_dir{};
    fs::path m_active_file{};

    float m_verticalOffsetSpeed{};
    float m_verticalOffset{};

    sf::Vector2f m_pos;

public:
    FileExplorer(std::string const &initPath, sf::Font const &font, sf::Vector2f const &pos);

    void updateInput(sf::Event const &event, float dt);

    void update(float dt);

    std::wstring getActivePath() const;

    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
};