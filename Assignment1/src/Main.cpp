#include <fstream>
#include <iostream>
#include <SFML/Graphics.hpp>

#include "AShape.h"

std::vector<std::shared_ptr<AShape>> shapes;
// window config
unsigned int window_w, window_h;
sf::Font font;

void readConfigFile(const std::string& file_name)
{
    std::ifstream fin(file_name);
    std::string mode;

    // font configs
    std::string font_path;
    sf::Color font_color;
    int font_size = 30;


    // shape configs
    std::string name;
    float px, py, sx, sy, w, h, radius;

    // color configs
    int r, g, b;

    while (fin >> mode)
    {
        if (mode == "Window")
        {
            fin >> window_w >> window_h;
        }
        else if (mode == "Font")
        {
            fin >> font_path >> font_size >> r >> g >> b;
            if (!font.loadFromFile(font_path))
            {
                std::cerr << "Could not load the font!" << '\n';
                return;
            }
            font_color = sf::Color(r, g, b);
        }
        else if (mode == "Circle")
        {
            fin >> name >> px >> py >> sx >> sy >> r >> g >> b >> radius;
            AShape as(sf::Vector2f(px, py), sf::Vector2f(sx, sy),
                      sf::Color(r, g, b), radius);
            as.setText(name, font, font_color, font_size);
            shapes.push_back(std::make_shared<AShape>(as));
        }
        else if (mode == "Rectangle")
        {
            fin >> name >> px >> py >> sx >> sy >> r >> g >> b >> w >> h;
            AShape as(sf::Vector2f(px, py), sf::Vector2f(sx, sy),
                      sf::Color(r, g, b), sf::Vector2f(w, h));
            as.setText(name, font, font_color, font_size);
            shapes.push_back(std::make_shared<AShape>(as));
        }
    }
}

int main()
{
    readConfigFile("config.txt");
    sf::RenderWindow window(sf::VideoMode(window_w, window_h), "SFML Test");
    // set this to avoid speed too fast
    // window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();

        for (auto& s : shapes)
        {
            if (s->shape->getPosition().x < 0 ||
                s->shape->getPosition().x + s->shape->getLocalBounds().width > window_w)
            {
                s->speed.x *= -1;
            }
            if (s->shape->getPosition().y < 0 ||
                s->shape->getPosition().y + s->shape->getLocalBounds().height > window_h)
            {
                s->speed.y *= -1;
            }

            s->draw(&window);
            s->move();
        }
        window.display();
    }

    return 0;
}
