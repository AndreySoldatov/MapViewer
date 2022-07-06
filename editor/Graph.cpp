#include <Graph.hpp>

#include <Line.hpp>
#include <iomanip>
#include <codecvt>

void Graph::pushVertex(Vertex const &v) {
    m_vertecies.push_back(v);
}

void Graph::removeVertexByIndex(size_t index) {
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

void Graph::insertEdge(size_t v1, size_t v2, Edge const &e) {
    if(v1 >= m_vertecies.size() || v2 >= m_vertecies.size())
        throw std::runtime_error("Cannot insert edge to non existing verticies");
    bool canPush{true};
    for (auto edge : m_edges) {
        canPush = canPush && 
        !((std::get<0>(edge) == v1 && std::get<1>(edge) == v2) || (std::get<1>(edge) == v1 && std::get<0>(edge) == v2));
    }
    if(canPush)
        m_edges.push_back({v1, v2, e});
}

void Graph::deleteEdge(size_t v1, size_t v2) {
    if(v1 >= m_vertecies.size() || v2 >= m_vertecies.size())
        return;
    
    m_edges.erase(
        std::remove_if(m_edges.begin(), m_edges.end(), [&](std::tuple<size_t, size_t, Edge> &val) -> bool {
            return std::get<0>(val) == v1 && std::get<1>(val) == v2;
        }),
        m_edges.end()
    );
}

void Graph::deleteEdgeByIndex(size_t index) {
    if(index >= m_edges.size()) return;
    m_edges.erase(m_edges.begin() + index);
}

const std::vector<Vertex> &Graph::getVertecies() const {
    return m_vertecies;
}

const std::vector<std::tuple<size_t, size_t, Edge>> &Graph::getEdges() const {
    return m_edges;
}

void Graph::updateInput(sf::Event const &e) {
    if(e.type == sf::Event::TextEntered) {
        if(m_activeVertex < m_vertecies.size()) {
            if(e.text.unicode >= 0x20) {
                m_vertecies[m_activeVertex].id += e.text.unicode;
            }
            if(e.text.unicode == 8 && m_vertecies[m_activeVertex].id.length() >= 2) {
                m_vertecies[m_activeVertex].id.erase(m_vertecies[m_activeVertex].id.end() - 1);
            }
        } else if(m_activeEdge < m_vertecies.size()) {
            if(e.text.unicode >= 0x20) {
                std::get<2>(m_edges[m_activeEdge]).id += e.text.unicode;
            }
            if(e.text.unicode == 8 && std::get<2>(m_edges[m_activeEdge]).id.length() >= 2) {
                std::get<2>(m_edges[m_activeEdge]).id.erase(std::get<2>(m_edges[m_activeEdge]).id.end() - 1);
            }
        }
    }
}

void Graph::update(sf::RenderWindow const &window, float scale, sf::Vector2f const &offset) {
    sf::Vector2f mousePos = sub(mult({(float)sf::Mouse::getPosition(window).x, (float)sf::Mouse::getPosition(window).y}, 1.0f / scale), mult(offset, 1.0f / scale));

    size_t index{};
    size_t removeIndex{1000000};

    for (auto & vertex : m_vertecies) {
        if(dist(mousePos, vertex.pos) <= 30.0f / scale && sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
            removeIndex = index;
        }
        index++;
    }

    removeVertexByIndex(removeIndex);

    size_t movingIndex{};
    for (auto & vertex : m_vertecies) {
        if(
            dist(mousePos, vertex.pos) <= 30.0f / scale && 
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

    if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        m_activeVertex = 1000000;
        for (size_t i = 0; i < m_vertecies.size(); i++)
        {
            if(dist(mousePos, m_vertecies[i].pos) <= 30.0f / scale) {
                m_activeVertex = i;
                break;
            }
        }
        m_activeEdge = 1000000;
        for (size_t i = 0; i < m_edges.size(); i++)
        {
            float minX = std::min(m_vertecies[std::get<0>(m_edges[i])].pos.x, m_vertecies[std::get<1>(m_edges[i])].pos.x);
            float maxX = std::max(m_vertecies[std::get<0>(m_edges[i])].pos.x, m_vertecies[std::get<1>(m_edges[i])].pos.x);

            float minY = std::min(m_vertecies[std::get<0>(m_edges[i])].pos.y, m_vertecies[std::get<1>(m_edges[i])].pos.y);
            float maxY = std::max(m_vertecies[std::get<0>(m_edges[i])].pos.y, m_vertecies[std::get<1>(m_edges[i])].pos.y);

            if(
                distToLine(mousePos, m_vertecies[std::get<0>(m_edges[i])].pos, m_vertecies[std::get<1>(m_edges[i])].pos) <= 30 &&
                mousePos.x >= minX && mousePos.x <= maxX &&
                mousePos.y >= minY && mousePos.y <= maxY
            ) {
                m_activeEdge = i;
                break;
            }
        }
        if(m_activeVertex < m_vertecies.size() && m_activeEdge < m_edges.size()) {
            m_activeEdge = 1000000;
        }
    }

    index = 0;
    removeIndex = 1000000;
    if(sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
        for (auto &edge : m_edges)
        {
            float minX = std::min(m_vertecies[std::get<0>(edge)].pos.x, m_vertecies[std::get<1>(edge)].pos.x);
            float maxX = std::max(m_vertecies[std::get<0>(edge)].pos.x, m_vertecies[std::get<1>(edge)].pos.x);

            float minY = std::min(m_vertecies[std::get<0>(edge)].pos.y, m_vertecies[std::get<1>(edge)].pos.y);
            float maxY = std::max(m_vertecies[std::get<0>(edge)].pos.y, m_vertecies[std::get<1>(edge)].pos.y);

            if(
                distToLine(mousePos, m_vertecies[std::get<0>(edge)].pos, m_vertecies[std::get<1>(edge)].pos) <= 30 &&
                mousePos.x >= minX && mousePos.x <= maxX &&
                mousePos.y >= minY && mousePos.y <= maxY
            ) {
                removeIndex = index;
                break;
            }
            index++;
        }
    }

    deleteEdgeByIndex(removeIndex);
}

void Graph::draw(
    sf::RenderTarget &target, 
    sf::RenderWindow const &window,
    sf::Font const &font, 
    sf::Color const &prim, 
    sf::Color const &sec,
    float scale,
    sf::Vector2f const &offset
) const {
    sf::Vector2f mousePos = {(float)sf::Mouse::getPosition(window).x, (float)sf::Mouse::getPosition(window).y};

    float maxWeight{};
    for (auto &edge : m_edges)
    {
        maxWeight = std::max(std::get<2>(edge).weight, maxWeight);
    }

    for(auto &edge : m_edges) {
        Line tmpLine(
            add(mult(m_vertecies[std::get<0>(edge)].pos, scale), offset),
            add(mult(m_vertecies[std::get<1>(edge)].pos, scale), offset),
            6.0 * (std::get<2>(edge).weight / maxWeight) + 4.0
        );

        tmpLine.setFillColor(std::get<2>(edge).color);
        tmpLine.setOutlineThickness(3);

        float minX = std::min(tmpLine.getPoint1().x, tmpLine.getPoint2().x);
        float maxX = std::max(tmpLine.getPoint1().x, tmpLine.getPoint2().x);

        float minY = std::min(tmpLine.getPoint1().y, tmpLine.getPoint2().y);
        float maxY = std::max(tmpLine.getPoint1().y, tmpLine.getPoint2().y);

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
                add(mult(
                    add(m_vertecies[std::get<0>(edge)].pos, m_vertecies[std::get<1>(edge)].pos), 
                    0.5f * scale
                ), offset), 
                mult(
                    {weight.getGlobalBounds().width, weight.getGlobalBounds().height}, 
                    0.5f
                )));

        target.draw(weight);
    }

    if(m_activeEdge < m_edges.size()) {
        sf::Text id;
        id.setString(std::get<2>(m_edges[m_activeEdge]).id);
        id.setFillColor(sf::Color(255, 150, 0));
        id.setCharacterSize(28.0);
        id.setOutlineColor(prim);
        id.setOutlineThickness(2);
        id.setFont(font);
        id.setPosition(
            add(
                add(mult(
                    add(m_vertecies[std::get<0>(m_edges[m_activeEdge])].pos, m_vertecies[std::get<1>(m_edges[m_activeEdge])].pos), 
                    0.5f * scale
                ), offset), {-id.getGlobalBounds().width * 0.5f, 20}));

        target.draw(id);
    }

    size_t index{};

    for (auto &vertex : m_vertecies) {
        sf::CircleShape circle;
        circle.setRadius(10);
        if(index == m_movingIndex) {
            circle.setOutlineThickness(5);
            circle.setFillColor(sf::Color(255, 255, 230));
            circle.setOutlineColor(sf::Color(255, 150, 0));   
        }
        else {
            circle.setOutlineThickness(5);
            circle.setFillColor(prim);
            circle.setOutlineColor(sec);
        }
        circle.setPosition(sub(add(mult(vertex.pos, scale), offset), {10, 10}));

        target.draw(circle);

        sf::Text text;
        text.setString(vertex.id);
        if(index == m_activeVertex) {
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
        text.setPosition(add(add(mult(vertex.pos, scale), offset), {12, 12}));

        target.draw(text);
        index++;
    }
}

nl::json Graph::packToJson(float normFactor) {
    nl::json js;
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    
    size_t index{};
    for (auto &vert : m_vertecies) {
        nl::json tmpVertex;
        tmpVertex["id"] = converter.to_bytes(vert.id);
        tmpVertex["pos"]["x"] = vert.pos.x / normFactor;
        tmpVertex["pos"]["y"] = vert.pos.y / normFactor;

        js["vert"][index] = tmpVertex;
        index++;
    }

    index = {};
    for (auto &edge : m_edges) {
        nl::json tmpEdge;

        tmpEdge["vert"]["v1"] = std::get<0>(edge);
        tmpEdge["vert"]["v2"] = std::get<1>(edge);
        tmpEdge["info"]["id"] = converter.to_bytes(std::get<2>(edge).id);
        tmpEdge["info"]["color"] = {std::get<2>(edge).color.r, std::get<2>(edge).color.g, std::get<2>(edge).color.b};
        tmpEdge["info"]["weight"] = std::get<2>(edge).weight;

        js["edge"][index] = tmpEdge;
        index++;
    }
    
    
    return js;
}