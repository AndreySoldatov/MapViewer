#pragma once

#include <SFML/Graphics.hpp>

#include <utils.hpp>

struct Edge {
    std::wstring id{};
    sf::Color color{};
    float weight{FLOAT_INF};
};

struct Vertex {
    std::wstring id{};
    sf::Vector2f pos{};
};

class Graph {
    std::vector<Vertex> m_vertecies{};
    std::vector<std::tuple<size_t, size_t, Edge>> m_edges;

    size_t m_movingIndex{100000};

public:
    void pushVertex(Vertex const &v);

    void removeVertexByIndex(size_t index);

    void insertEdge(size_t v1, size_t v2, Edge const &e);

    void deleteEdge(size_t v1, size_t v2);

    void deleteEdgeByIndex(size_t index);

    const std::vector<Vertex> &getVertecies() const;

    const std::vector<std::tuple<size_t, size_t, Edge>> &getEdges() const;

    void update(sf::RenderWindow const &window);

    void draw(
        sf::RenderTarget &target, 
        sf::RenderWindow const &window,
        sf::Font const &font, 
        sf::Color const &prim, 
        sf::Color const &sec
    ) const;
};