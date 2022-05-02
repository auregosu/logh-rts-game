	#ifndef MODULETYPES_HPP
#define MODULETYPES_HPP
#include "Module.hpp"

class Motor : public Module
{
public:
	sf::RectangleShape shape1, shape2;
	float maxSpeed, speed=maxSpeed, targetPos, targetDistance, prevDistance, lastTargetPos, lastPrevDistance, currentDistance;
	bool moving = false, stopping = false, horizontal;

	inline Motor() {};
	Motor(bool _horizontal, float _maxSpeed, float shipScale, float sizeX, float sizeY);
	Motor(bool _horizontal, float _maxSpeed, float shipScale, Vec2 dimensions);
	void Effect(float& pos, float& vel, float& acc, float& energy, float deltaTime, float mass);
	void Effect2(float& pos, float& vel, float& acc, float& energy, float deltaTime, float mass);
	void EffectUpdate(Vec2& pos, Vec2& vel, Vec2& acc, float& energy, float deltaTime, short unsigned int& isMoving, float mass);
	void Draw(sf::RenderWindow *window);
	sf::RectangleShape* GetShape1();
	sf::RectangleShape* GetShape2();
	Module* CreateCopy();
};

class MassShield : public Module
{
public:
	inline MassShield() {visible=false;};
	MassShield(float extraHealth);
	inline sf::RectangleShape* GetShape1() {return nullptr;}
	inline sf::RectangleShape* GetShape2() {return nullptr;}
	Module* CreateCopy();
};

#endif