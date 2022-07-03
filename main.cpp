#include <iostream>
#include <vector>
#include <array>
#include <algorithm>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <functional>
#include <sstream>
#include <iomanip>
#include <locale>
#include <codecvt>
#include <cwctype>

#define FLOAT_INF 13371337.0

float length(sf::Vector2f const &vec) {
    return std::sqrt(vec.x * vec.x + vec.y * vec.y);
}

sf::Vector2f norm(sf::Vector2f const &vec) {
    return {vec.x / length(vec), vec.y / length(vec)};
}

sf::Vector2f add(sf::Vector2f const &vec1, sf::Vector2f const &vec2) {
    return {vec1.x + vec2.x, vec1.y + vec2.y};
}

sf::Vector2f sub(sf::Vector2f const &vec1, sf::Vector2f const &vec2) {
    return {vec1.x - vec2.x, vec1.y - vec2.y};
}

sf::Vector2f mult(sf::Vector2f const &vec, float scal) {
    return {vec.x * scal, vec.y * scal};
}

float dist(sf::Vector2f const &v1, sf::Vector2f const &v2) {
    return length({v2.x - v1.x, v2.y - v1.y});
}

sf::Color invert(sf::Color const & col) {
    return {(uint8_t)(255 - col.r), (uint8_t)(255 - col.g), (uint8_t)(255 - col.b), col.a};
}

float distToLine(sf::Vector2f const &vec, sf::Vector2f const &p1, sf::Vector2f const &p2) {
    float slope = (p2.y - p1.y) / (p2.x - p1.x);

    float a = -slope;
    float b = 1;
    float c = slope * p1.x - p1.y;

    return std::abs(a * vec.x + b * vec.y + c) / std::sqrt(a * a + b * b);
}

class Line : public sf::Drawable {
    sf::Vector2f m_p1{};
    sf::Vector2f m_p2{};
    float m_thickness{5};
    sf::ConvexShape m_shape{};

    void update() {
        sf::Vector2f normal = mult(norm({-(m_p2.y - m_p1.y), m_p2.x - m_p1.x}), m_thickness * 0.5);
        
        m_shape.setPoint(0, add(m_p1, normal));
        m_shape.setPoint(1, add(m_p2, normal));
        m_shape.setPoint(2, sub(m_p2, normal));
        m_shape.setPoint(3, sub(m_p1, normal));
    }

public:

    Line(sf::Vector2f const &p1, sf::Vector2f const &p2, float thickness = 5) : m_p1(p1), m_p2(p2), m_thickness(thickness) {
        m_shape.setPointCount(4);
        m_shape.setFillColor(sf::Color::White);

        update();
    }

    void setPoint1(sf::Vector2f const &vec) {
        m_p1 = vec;

        update();
    }

    void setPoint2(sf::Vector2f const &vec) {
        m_p2 = vec;

        update();
    }

    const sf::Vector2f &getPoint1() const {
        return m_p1;
    }

    const sf::Vector2f &getPoint2() const {
        return m_p2;
    }

    const float &getThickness() const {
        return m_thickness;
    }

    void setThickness(float thickness) {
        m_thickness = thickness;

        update();
    }

    const sf::ConvexShape &getShape() const {
        return m_shape;
    }

    void setFillColor(sf::Color const &col) {
        m_shape.setFillColor(col);
    }

    void setOutlineColor(sf::Color const &col) {
        m_shape.setOutlineColor(col);
    }

    void setOutlineThickness(float thickness) {
        m_shape.setOutlineThickness(thickness);
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        target.draw(m_shape, states);
    }
};

// Represents Edges of Graph
struct Edge {
    std::wstring id{};
    sf::Color color{};
    float weight{FLOAT_INF};
};

struct Vertex {
    std::wstring id{};
    sf::Vector2f pos{};
};

// Represents weighted Graph data structure
class Graph {
    std::vector<Vertex> m_vertecies{};     // Names of vertecies
    std::vector<std::tuple<size_t, size_t, Edge>> m_edges;

    size_t m_movingIndex{100000};

public:
    void pushVertex(Vertex const &v) {
        m_vertecies.push_back(v);
    }

    void removeVertexByIndex(size_t index) {
        if(index >= m_vertecies.size()) return;

        m_vertecies.erase(m_vertecies.begin() + index);

        m_edges.erase(std::remove_if(m_edges.begin(), m_edges.end(), [&](auto edge) {
            return std::get<0>(edge) == index || std::get<1>(edge) == index;
        }), m_edges.end());

        for (auto &edge : m_edges) {
            if(std::get<0>(edge) > index) std::get<0>(edge)--;
            if(std::get<1>(edge) > index) std::get<1>(edge)--; 
        }
    }

    void insertEdge(size_t v1, size_t v2, Edge const &e) {
        if(v1 >= m_vertecies.size() || v2 >= m_vertecies.size())
            throw std::runtime_error("Cannot insert edge to non existing verticies");
        m_edges.push_back({v1, v2, e});
    }

    void deleteEdge(size_t v1, size_t v2) {
        if(v1 >= m_vertecies.size() || v2 >= m_vertecies.size())
            return;
        
        m_edges.erase(
            std::remove_if(m_edges.begin(), m_edges.end(), [&](std::tuple<size_t, size_t, Edge> &val) -> bool {
                return std::get<0>(val) == v1 && std::get<1>(val) == v2;
            }),
            m_edges.end()
        );
    }

    void deleteEdgeByIndex(size_t index) {
        if(index >= m_edges.size()) return;
        m_edges.erase(m_edges.begin() + index);
    }

    const std::vector<Vertex> &getVertecies() const {
        return m_vertecies;
    }

    const std::vector<std::tuple<size_t, size_t, Edge>> &getEdges() const {
        return m_edges;
    }

    void update(sf::RenderWindow const &window) {
        sf::Vector2f mousePos = {(float)sf::Mouse::getPosition(window).x, (float)sf::Mouse::getPosition(window).y};

        size_t index{};
        size_t removeIndex{1000000};

        for (auto & vertex : m_vertecies) {
            if(dist(mousePos, vertex.pos) <= 30 && sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
                removeIndex = index;
            }
            index++;
        }

        removeVertexByIndex(removeIndex);

        size_t movingIndex{};
        for (auto & vertex : m_vertecies) {
            if(
                dist(mousePos, vertex.pos) <= 30 && 
                sf::Mouse::isButtonPressed(sf::Mouse::Left) &&
                m_movingIndex > m_vertecies.size()
            ) {
                m_movingIndex = movingIndex;
            }
            movingIndex++;
        }

        if(m_movingIndex < m_vertecies.size()) {
            if( 
                sf::Mouse::isButtonPressed(sf::Mouse::Left)
            ) {
                m_vertecies[m_movingIndex].pos = mousePos;
            } else {
                m_movingIndex = 1000000;
            }
        }        

        index = 0;
        removeIndex = 1000000;
        for (auto &edge : m_edges)
        {
            float minX = std::min(m_vertecies[std::get<0>(edge)].pos.x, m_vertecies[std::get<1>(edge)].pos.x);
            float maxX = std::max(m_vertecies[std::get<0>(edge)].pos.x, m_vertecies[std::get<1>(edge)].pos.x);

            float minY = std::min(m_vertecies[std::get<0>(edge)].pos.y, m_vertecies[std::get<1>(edge)].pos.y);
            float maxY = std::max(m_vertecies[std::get<0>(edge)].pos.y, m_vertecies[std::get<1>(edge)].pos.y);

            if(
                distToLine(mousePos, m_vertecies[std::get<0>(edge)].pos, m_vertecies[std::get<1>(edge)].pos) <= 30 &&
                mousePos.x >= minX && mousePos.x <= maxX &&
                mousePos.y >= minY && mousePos.y <= maxY && sf::Mouse::isButtonPressed(sf::Mouse::Right)
            ) {
                removeIndex = index;
                break;
            }
            index++;
        }

        deleteEdgeByIndex(removeIndex);
    }

    void draw(
        sf::RenderTarget &target, 
        sf::RenderWindow const &window,
        sf::Font const &font, 
        sf::Color const &prim, 
        sf::Color const &sec
    ) {
        sf::Vector2f mousePos = {(float)sf::Mouse::getPosition(window).x, (float)sf::Mouse::getPosition(window).y};

        float maxWeight{};
        for (auto &edge : m_edges)
        {
            maxWeight = std::max(std::get<2>(edge).weight, maxWeight);
        }

        for(auto &edge : m_edges) {
            Line tmpLine(
                m_vertecies[std::get<0>(edge)].pos,
                m_vertecies[std::get<1>(edge)].pos,
                6.0 * (std::get<2>(edge).weight / maxWeight) + 4.0
            );

            tmpLine.setFillColor(std::get<2>(edge).color);
            tmpLine.setOutlineThickness(3);

            float minX = std::min(m_vertecies[std::get<0>(edge)].pos.x, m_vertecies[std::get<1>(edge)].pos.x);
            float maxX = std::max(m_vertecies[std::get<0>(edge)].pos.x, m_vertecies[std::get<1>(edge)].pos.x);

            float minY = std::min(m_vertecies[std::get<0>(edge)].pos.y, m_vertecies[std::get<1>(edge)].pos.y);
            float maxY = std::max(m_vertecies[std::get<0>(edge)].pos.y, m_vertecies[std::get<1>(edge)].pos.y);

            if(
                distToLine(mousePos, tmpLine.getPoint1(), tmpLine.getPoint2()) <= 30 &&
                mousePos.x >= minX && mousePos.x <= maxX &&
                mousePos.y >= minY && mousePos.y <= maxY 
            ) {
                tmpLine.setOutlineColor(std::get<2>(edge).color);
            } else {
                tmpLine.setOutlineColor(sf::Color::Transparent);
            }
            
            target.draw(tmpLine);

            sf::Text weight;
            std::stringstream ss;
            ss << std::fixed << std::setprecision(1) << std::get<2>(edge).weight;
            weight.setString(ss.str());
            weight.setFillColor(sec);
            weight.setCharacterSize(28.0);
            weight.setOutlineColor(prim);
            weight.setOutlineThickness(2);
            weight.setFont(font);
            weight.setPosition(
                sub(
                    mult(
                        add(m_vertecies[std::get<0>(edge)].pos, m_vertecies[std::get<1>(edge)].pos), 
                        0.5f
                    ), 
                    mult(
                        {weight.getGlobalBounds().width, weight.getGlobalBounds().height}, 
                        0.5f
                    )));

            target.draw(weight);
        }

        size_t index{};

        for (auto &vertex : m_vertecies) {
            sf::CircleShape circle;
            circle.setRadius(10);
            if(index == m_movingIndex) {
                circle.setOutlineThickness(6);
                circle.setFillColor(sf::Color(255, 255, 230));
                circle.setOutlineColor(sf::Color(255, 150, 0));   
            }
            else {
                circle.setOutlineThickness(6);
                circle.setFillColor(prim);
                circle.setOutlineColor(sec);
            }
            circle.setPosition(sub(vertex.pos, {10, 10}));

            target.draw(circle);

            sf::Text text;
            text.setString(vertex.id);
            if(index == m_movingIndex) {
                text.setFillColor(sf::Color(255, 150, 0));
                text.setCharacterSize(28.0);
            }
            else {
                text.setFillColor(sec);
                text.setCharacterSize(24.0);
            }
            text.setOutlineColor(prim);
            text.setOutlineThickness(2);
            text.setFont(font);
            text.setPosition(add(vertex.pos, {12, 12}));

            target.draw(text);
            index++;
        }
    }
};

class InputField : public sf::Drawable {
    sf::RectangleShape m_padding;
    sf::Text m_text;
    std::wstring m_defaultString{L"Дефолт"};
    std::wstring m_string{};
    bool m_active{};

public:
    InputField(sf::Font const &font, sf::Vector2f pos, std::wstring const &defstr = L"Дефолт") {
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

    void update(sf::Event const &e, sf::RenderWindow const &window) {
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

    void draw(sf::RenderTarget &target, sf::RenderStates states) const override {
        target.draw(m_padding, states);
        target.draw(m_text, states);
    }

    const std::wstring &getString() const {
        return m_string;
    }

};

class Button : public sf::Drawable {
    sf::RectangleShape m_padding;
    sf::Text m_text;

    std::function<void(void)> m_onClick;

public:

    Button(sf::Font const &font, sf::Vector2f const &pos, std::wstring const &text, std::function<void(void)> onClick) {
        m_padding.setFillColor(sf::Color(230, 230, 230));
        m_padding.setPosition(pos);

        m_text.setFont(font);
        m_text.setString(text);
        m_text.setCharacterSize(30);
        m_text.setFillColor(sf::Color(100, 100, 100));
        m_text.setPosition({pos.x + 20, pos.y + 12.0f});
        
        m_padding.setSize({m_text.getGlobalBounds().width + 40, 60});
        
        m_onClick = onClick;
    }

    void update(sf::Event const &e, sf::RenderWindow const &window) {
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

    void draw(sf::RenderTarget &target, sf::RenderStates states) const override {
        target.draw(m_padding, states);
        target.draw(m_text, states);
    } 
};

class Slider : public sf::Drawable {
    Line m_line;
    sf::CircleShape m_cursor;
    sf::Vector2f m_pos;

public:
    Slider(sf::Vector2f pos) : m_line(pos, add(pos, {300, 0}), 5) {
        m_line.setFillColor(sf::Color(200, 200, 200));

        m_cursor.setRadius(13);
        m_cursor.setFillColor(sf::Color(230, 230, 230));
        m_cursor.setPosition(sub(pos, {13, 13}));

        m_pos = pos;
    }

    void update(sf::RenderWindow const &window) {
        sf::Vector2f mousePos = {(float)sf::Mouse::getPosition(window).x, (float)sf::Mouse::getPosition(window).y};

        if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            if(
                mousePos.x >= m_line.getPoint1().x && mousePos.x <= m_line.getPoint2().x &&
                mousePos.y >= m_cursor.getPosition().y - 10 && mousePos.y <= m_cursor.getPosition().y + 2.0f * m_cursor.getRadius() + 10
            ) {
                m_cursor.setPosition({mousePos.x - 13, m_cursor.getPosition().y});
            }
        }
    }

    void draw(sf::RenderTarget &target, sf::RenderStates states) const override {
        target.draw(m_line, states);
        target.draw(m_cursor, states);
    }

    float getValue() {
        return (m_cursor.getPosition().x - m_pos.x + 13) / 300.0f;
    }
};

int main()
{
    sf::Texture bgImage;
    bgImage.loadFromFile("map.jpg");
    bgImage.setSmooth(true);

    sf::Sprite bgSprite;
    bgSprite.setTexture(bgImage);
    bgSprite.scale({1080.0f / bgImage.getSize().y, 1080.0f / bgImage.getSize().y});
    bgSprite.setColor({255, 255, 255, 100});

    sf::ContextSettings settings;
    settings.antialiasingLevel = 4;

    sf::RenderWindow window(sf::VideoMode(1920, 1080), "", sf::Style::Fullscreen, settings);
    window.setFramerateLimit(60);

    sf::Font font;
    if(!font.loadFromFile("font.ttf")) return 1;
    
    Graph g;
    g.pushVertex({L"Альдрун", {100, 100}});
    g.pushVertex({L"Балмора", {200, 400}});
    g.pushVertex({L"Кальдера", {600, 200}});

    g.insertEdge(0, 1, {L"", {255, 20, 20}, 20});
    g.insertEdge(1, 2, {L"", {255, 200, 0}, 10});
    g.insertEdge(2, 0, {L"", {0, 20, 255}, 5});

    sf::RectangleShape rect;
    rect.setFillColor(sf::Color::White);
    rect.setSize({600, 1080});
    rect.setPosition({1320, 0});
    rect.setOutlineColor({200, 200, 200});
    rect.setOutlineThickness(1);

    InputField vertexInput(font, {1400, 100}, L"Имя вершины");
    Button vertexButton(font, {1400, 200}, L"Добавить", [&]() {
        if(!vertexInput.getString().empty()) {
            g.pushVertex({vertexInput.getString(), {100, 900}});
        }
    });
    sf::Text vertexText;
    vertexText.setFont(font);
    vertexText.setString(L"Добавить точку на карту:");
    vertexText.setCharacterSize(30);
    vertexText.setFillColor(sf::Color(100, 100, 100));
    vertexText.setPosition({1360, 30});

    sf::Text edgeText;
    edgeText.setFont(font);
    edgeText.setString(L"Добавить ребро:");
    edgeText.setCharacterSize(30);
    edgeText.setFillColor(sf::Color(100, 100, 100));
    edgeText.setPosition({1360, 300});

    sf::Text edgeColorText;
    edgeColorText.setFont(font);
    edgeColorText.setString(L"Цвет ребра:");
    edgeColorText.setCharacterSize(30);
    edgeColorText.setFillColor(sf::Color(100, 100, 100));
    edgeColorText.setPosition({1400, 550});

    InputField from(font, {1400, 370}, L"Откуда");
    InputField to(font, {1400, 470}, L"Куда");

    InputField edgeNameInput(font, {1400, 780}, L"Имя ребра");
    InputField edgeWeightInput(font, {1400, 880}, L"Вес ребра (цифра)");

    sf::Color sliderColor;

    Button edgeButton(font, {1400, 980}, L"Добавить", [&]() {
        float weight{};
        try {
            std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
            weight = std::stof(converter.to_bytes(edgeWeightInput.getString()));
        } catch (std::invalid_argument const &e) {
            return;
        }

        if(!edgeNameInput.getString().empty() && !from.getString().empty() && !to.getString().empty()) {
            size_t vertexIndex1{1000000};
            size_t vertexIndex2{1000000};
            for (size_t i = 0; i < g.getVertecies().size(); i++)
            {
                if(g.getVertecies()[i].id == from.getString()) {
                    vertexIndex1 = i;
                }
                if(g.getVertecies()[i].id == to.getString()) {
                    vertexIndex2 = i;
                }
            }

            if(vertexIndex1 < g.getVertecies().size() && vertexIndex2 < g.getVertecies().size()) {
                g.insertEdge(
                    vertexIndex1,
                    vertexIndex2,
                    {
                        edgeNameInput.getString(),
                        sliderColor,
                        weight
                    }
                );
            }
        }
    });

    Slider redSlider({1400, 630});
    Slider greenSlider({1400, 680});
    Slider blueSlider({1400, 730});

    sf::RectangleShape colorRect;
    colorRect.setPosition({1770, 630});
    colorRect.setSize({100, 100});
    colorRect.setOutlineColor(sf::Color(230, 230, 230));
    colorRect.setOutlineThickness(6);

    sf::Clock cl;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (
                event.type == sf::Event::Closed || 
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
            ) {
                window.close();
            }

            vertexInput.update(event, window);
            vertexButton.update(event, window);

            from.update(event, window);
            to.update(event, window);

            edgeNameInput.update(event, window);
            edgeWeightInput.update(event, window);
            edgeButton.update(event, window);
        }

        g.update(window);

        redSlider.update(window);
        greenSlider.update(window);
        blueSlider.update(window);

        sliderColor = {
            (uint8_t)(redSlider.getValue() * 255),
            (uint8_t)(greenSlider.getValue() * 255),
            (uint8_t)(blueSlider.getValue() * 255),
        };

        colorRect.setFillColor(sliderColor);

        window.clear(sf::Color::White);

        window.draw(bgSprite);

        window.draw(rect);
        window.draw(vertexText);
        window.draw(vertexInput);
        window.draw(vertexButton);

        window.draw(edgeText);
        window.draw(edgeColorText);
        window.draw(edgeNameInput);
        window.draw(edgeWeightInput);
        window.draw(edgeButton);

        window.draw(from);
        window.draw(to);

        window.draw(redSlider);
        window.draw(greenSlider);
        window.draw(blueSlider);
        window.draw(colorRect);

        g.draw(window, window, font, sf::Color::White, {100, 100, 100});
        window.display();
    }

    return 0;
}