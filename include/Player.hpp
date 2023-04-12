#ifndef PLAYER_HPP
#define PLAYER_HPP
#define _USE_MATH_DEFINES
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
    enum PlayerAction{
        NONE,
        MOVING_MODULE,
        COMMAND_SHIP_MOVEMENT_START,
        COMMAND_SHIP_MOVEMENT_ROTATE,
        SELECTING_SQUARE,
        CHANGING_SUPPLIES} action = NONE;
    sf::Color color = sf::Color::White;
    sf::RectangleShape selectionSquare;
    Player(std::string _name, sf::Color _color);
    Vec2 shipAveragePos;
    float prevShipRotation;
};

#endif
