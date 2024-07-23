#pragma once

#include <SFML/Graphics.hpp>
#include "Vec2.h"

class CTransform
{
public:
    Vec2 position = {0.0f, 0.0f};
    Vec2 velocity = {0.0f, 0.0f};
    float angle = 0;

    CTransform(const Vec2& p, const Vec2& v, const float a): position(p), velocity(v), angle(a)
    {
    }
};

class CShape
{
public:
    sf::CircleShape shape;

    CShape(const float radius, const int points,
           const sf::Color& fillColor, const sf::Color outlineColor, const float thickness)
        : shape(radius, points)
    {
        shape.setFillColor(fillColor);
        shape.setOutlineColor(outlineColor);
        shape.setOutlineThickness(thickness);
        shape.setOrigin(radius, radius);
    }
};

class CCollision
{
public:
    float radius = 0.0f;

    CCollision(const float radius) : radius(radius)
    {
    }
};

class CScore
{
public:
    int score = 0;

    CScore(const int score) : score(score)
    {
    }
};

class CLifespan
{
public:
    int remaining = 0; // amount of remaining lifespan on the entity
    int total = 0; // the total initial amount of lifespan

    CLifespan(const int total) : remaining(total), total(total)
    {
    }
};

class CInput
{
public:
    bool up = false;
    bool left = false;
    bool right = false;
    bool down = false;
    bool shoot = false;

    CInput()
    {
    }
};
