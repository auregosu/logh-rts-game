#ifndef PLAYER_HPP
#define PLAYER_HPP
#define _USE_MATH_DEFINES
#include <iostream>
#include <cmath>
#include <unordered_set>
#include <set>
#include <map>
#include "Tools.hpp"

struct Player
{
    static Player* mainPlayer;
    static unsigned int playerCount;
    unsigned int playerID;
    std::string name = "player";
    PlayerAction action = PlayerAction::NONE;
    sf::Color color = sf::Color::White;
    sf::RectangleShape selectionSquare;
    Player(std::string _name, sf::Color _color);
    Vec2 pivot1, pivot2, shipAveragePos;
    float prevShipRotation;
};

#endif