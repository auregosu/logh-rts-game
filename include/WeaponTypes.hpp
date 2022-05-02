#ifndef WEAPONTYPES_HPP
#define WEAPONTYPES_HPP
#include "Weapon.hpp"

class Cannon : public Weapon
{
public:
	inline Cannon() {};
	Cannon(float shipScale, Vec2 dimensions);
	void Effect(float& energy, bool& shipAttacking);
	void Draw(sf::RenderWindow* window);
	sf::RectangleShape* GetShape1();
	sf::RectangleShape* GetShape2();
	Module* CreateCopy();
};

#endif