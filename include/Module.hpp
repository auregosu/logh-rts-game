#ifndef MODULE_HPP
#define MODULE_HPP
#include "Player.hpp"

class Module
{
private:

public:
	enum Type {
		NONE=0,
		MOTORX=1,
		MOTORY=2,
		CANNON=3
	};
	Type type = NONE;
	float health, fullHealth;
	bool weapon, visible;
	inline Module() {weapon=false; visible=true;};

	inline virtual void Effect() {};
	inline virtual void Effect2() {};
	inline virtual void EffectUpdate() {};
	inline virtual void Draw(sf::RenderWindow* window) {};
	inline virtual sf::RectangleShape* GetShape1() {};
	inline virtual sf::RectangleShape* GetShape2() {};
	virtual Module* CreateCopy();
};

#endif