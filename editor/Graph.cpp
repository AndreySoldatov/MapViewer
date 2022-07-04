#include <Graph.hpp>

#include <Line.hpp>
#include <iomanip>

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

void Graph::update(sf::RenderWindow const &window) {
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

void Graph::draw(
    sf::RenderTarget &target, 
    sf::RenderWindow const &window,
    sf::Font const &font, 
    sf::Color const &prim, 
    sf::Color const &sec
) const {
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