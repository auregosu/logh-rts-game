#include <Player.hpp>

Player::Player(std::string _name, sf::Color _color)
{
	playerCount+=1;
	name = _name;
	color = _color;
	playerID = playerCount;
	sf::Color selectionSquareColor = sf::Color::White;
	selectionSquareColor.a = 100;
	selectionSquare.setFillColor(selectionSquareColor);
}

unsigned int Player::playerCount = 0;
Player* Player::mainPlayer;