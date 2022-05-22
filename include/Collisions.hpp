#ifndef COLLISIONS_HPP
#define COLLISIONS_HPP
#include "Player.hpp"

class CollisionType
{
public:
    bool col;
    
    CollisionType() { col = false;}
    CollisionType(bool _col) : col(_col) {}
    inline void Init(bool _col)
    {
        col = _col;
    }
};

class Collisions
{
public:
    //std::vector<Vec2> GetShapePoints(sf::Shape shape);
    static bool areOverlapping(std::vector<Vec2> s1, std::vector<Vec2> s2);
};

#endif
