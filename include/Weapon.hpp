#ifndef WEAPON_HPP
#define WEAPON_HPP
#include "Module.hpp"

class Weapon : public Module
{
	public:
	Weapon(float shipScale, Vec2 offset);
	sf::RectangleShape shape, beam;
	Vec2* targetPos=nullptr;
	float power, range, recoil, accumulator=0, targetDistance;
	float* targetHealth=nullptr;
	bool attacking=false, selected=false;
	inline Weapon() {weapon=true;};
	inline virtual void Effect(float& energy, bool& shipAttacking) {};
};

#endif