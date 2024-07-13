#pragma once
#include <memory>
#include <iostream>

#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Shape.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

class AShape
{
public:
    std::shared_ptr<sf::Shape> shape;
    // name in the shape
    sf::Text shape_name;
    // speed of the shape
    sf::Vector2f speed;

    AShape(const sf::Vector2f pos, const sf::Vector2f speed,
           const sf::Color color,
           const sf::Vector2f size)
    {
        shape = std::make_shared<sf::RectangleShape>(size);
        shape->setPosition(pos);
        shape->setFillColor(color);
        this->speed = speed;
    }

    AShape(const sf::Vector2f pos, const sf::Vector2f speed,
           const sf::Color color,
           const float radius)
    {
        shape = std::make_shared<sf::CircleShape>(radius);
        shape->setPosition(pos);
        shape->setFillColor(color);
        this->speed = speed;
    }

    void setText(const std::string& name, const sf::Font& font, const sf::Color& color, const int font_size)
    {
        shape_name.setFont(font);
        shape_name.setString(name);
        shape_name.setFillColor(color);
        shape_name.setCharacterSize(font_size);
        shape_name.setOrigin(shape_name.getLocalBounds().left + shape_name.getLocalBounds().width * 0.5f,
                             shape_name.getLocalBounds().top + shape_name.getLocalBounds().height * 0.5f);
        shape_name.setPosition(
            shape->getPosition().x + shape->getLocalBounds().width * 0.5f,
            shape->getPosition().y + shape->getLocalBounds().height * 0.5f);
    }

    void move()
    {
        shape->move(speed);
        shape_name.move(speed);
    }

    void draw(sf::RenderWindow* window) const
    {
        window->draw(*shape);
        window->draw(shape_name);
    }
};
