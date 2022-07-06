#pragma once

#include <SFML/Graphics.hpp>
#include <cmath>

#define PI 3.14159265f
#define FLOAT_INF 13371337.0

inline float length(sf::Vector2f const &vec) {
    return std::sqrt(vec.x * vec.x + vec.y * vec.y);
}

inline sf::Vector2f norm(sf::Vector2f const &vec) {
    return {vec.x / length(vec), vec.y / length(vec)};
}

inline sf::Vector2f add(sf::Vector2f const &vec1, sf::Vector2f const &vec2) {
    return {vec1.x + vec2.x, vec1.y + vec2.y};
}

inline sf::Vector2f sub(sf::Vector2f const &vec1, sf::Vector2f const &vec2) {
    return {vec1.x - vec2.x, vec1.y - vec2.y};
}

inline sf::Vector2f mult(sf::Vector2f const &vec, float scal) {
    return {vec.x * scal, vec.y * scal};
}

inline float dist(sf::Vector2f const &v1, sf::Vector2f const &v2) {
    return length({v2.x - v1.x, v2.y - v1.y});
}

inline sf::Color invert(sf::Color const & col) {
    return {(uint8_t)(255 - col.r), (uint8_t)(255 - col.g), (uint8_t)(255 - col.b), col.a};
}

inline float distToLine(sf::Vector2f const &vec, sf::Vector2f const &p1, sf::Vector2f const &p2) {
    float slope = (p2.y - p1.y) / (p2.x - p1.x);

    float a = -slope;
    float b = 1;
    float c = slope * p1.x - p1.y;

    return std::abs(a * vec.x + b * vec.y + c) / std::sqrt(a * a + b * b);
}

inline sf::Vector2f toVec2f(sf::Vector2i const &vec) {
    return {(float)vec.x, (float)vec.y};
}

inline sf::Vector2f delta(sf::Vector2f const &vec1, sf::Vector2f const &vec2) {
    return {vec1.x - vec2.x, vec1.y - vec2.y};
}