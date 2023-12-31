// SFMLGame.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <SFML/Graphics.hpp>

int main()
{
	sf::RenderWindow window(sf::VideoMode(300, 300), "SFML works!");
	sf::CircleShape shape(100.f);
	shape.setFillColor(sf::Color::Yellow);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear();
		window.draw(shape);
		window.display();
	}

	return 0;
}
