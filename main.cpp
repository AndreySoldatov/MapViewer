#include <iostream>
#include <vector>
#include <array>
#include <algorithm>
#include <SFML/Graphics.hpp>

#define FLOAT_INF 13371337.0

// Color data type
typedef std::array<uint8_t, 3> Color;

// Represents Edges of Graph
struct Edge {
    std::wstring id{};
    Color color{};
    float weight{FLOAT_INF};
};

struct Vertex {
    std::wstring id{};
    sf::Vector2f pos{};
};

// Represents weighted Graph data structure
class Graph {
    std::vector<Vertex> m_vertexNames{};     // Names of vertecies
    std::vector<std::vector<Edge>> m_adgMatrix{}; // Adjacency matrix of graph

public:
    void pushVertex(Vertex const &v) { // Adds Vertex to the end of the adj. matrix
        m_vertexNames.push_back(v);

        for (size_t i = 0; i < m_vertexNames.size() - 1; i++) {
            m_adgMatrix[i].push_back({});
        }

        m_adgMatrix.push_back(std::vector<Edge>(m_vertexNames.size(), Edge{}));
    }

    void removeVertexByIndex(size_t index) {
        if(index >= m_adgMatrix.size()) throw std::runtime_error("Cannot remove non existing vertex");

        m_vertexNames.erase(m_vertexNames.begin() + index);

        m_adgMatrix.erase(m_adgMatrix.begin() + index);

        for (size_t i = 0; i < m_adgMatrix.size(); i++) {
            m_adgMatrix[i].erase(m_adgMatrix[i].begin() + index);
        }
    }

    void insertEdge(size_t v1, size_t v2, Edge const &e) {
        if(v1 >= m_adgMatrix.size() || v2 >= m_adgMatrix.size())
            throw std::runtime_error("Cannot insert edge to non existing verticies");
        m_adgMatrix[v1][v2] = e;
        m_adgMatrix[v2][v1] = e;
    }

    void deleteEdge(size_t v1, size_t v2) {
        if(v1 >= m_adgMatrix.size() || v2 >= m_adgMatrix.size())
            throw std::runtime_error("Cannot delete edge of non existing verticies");
        
        m_adgMatrix[v1][v2] = {};
        m_adgMatrix[v2][v1] = {};
    }

    void printToConsole() {
        size_t index{};
        for (auto &row : m_adgMatrix) {
            std::wcout << m_vertexNames[index++].id << "\t";
            for (auto &edge : row) {
                std::cout << edge.weight << " ";
            }
            std::cout << "\n";
        }
    }
};

int main() {
    Graph g;
    g.pushVertex({L"first"});
    g.pushVertex({L"second"});
    g.pushVertex({L"third"});
    g.pushVertex({L"fourth"});

    g.removeVertexByIndex(1);

    g.insertEdge(1, 2, {{}, {}, 12});

    g.deleteEdge(1, 2);

    g.printToConsole();
    return 0;
}