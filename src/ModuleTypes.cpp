#include "ModuleTypes.hpp"

Motor::Motor(bool _horizontal, float _maxSpeed, float shipScale, float sizeX, float sizeY)
{
	
	fullHealth = 20;
	health = fullHealth;
	horizontal = _horizontal;
	if (horizontal)
		type = Module::MOTORX;
	else
		type = Module::MOTORY;
	maxSpeed = _maxSpeed;
	speed = maxSpeed;
	if (horizontal)
	{
		shape1.setSize({0.25, 0.5});
		shape2.setSize({0.25, 0.5});
		shape1.setOrigin((sizeX*0.5)+0.125, -sizeY*0.25);
		shape2.setOrigin(-(sizeX*0.5)+0.125, -sizeY*0.25);
	} else
	{
		shape1.setSize({0.25, 0.20});
		shape2.setSize({0.25, 0.20});
		shape1.setOrigin((sizeX*0.25)+0.125, -sizeY*0.5);
		shape2.setOrigin(-(sizeX*0.25)+0.125, -sizeY*0.5);
	}
	shape1.setScale(shipScale, shipScale);
	shape1.setFillColor(Palette::Tech1);
	shape2.setScale(shipScale, shipScale);
	shape2.setFillColor(Palette::Tech1);
}
Motor::Motor(bool _horizontal, float _maxSpeed, float shipScale, Vec2 dimensions)
{
	fullHealth = 20;
	health = fullHealth;
	horizontal = _horizontal;
	if (horizontal)
		type = Module::MOTORX;
	else
		type = Module::MOTORY;
	maxSpeed = _maxSpeed;
	speed = maxSpeed;
	if (horizontal)
	{
		shape1.setSize({0.25, 0.5});
		shape2.setSize({0.25, 0.5});
		shape1.setOrigin(Vec2(-0.5*dimensions.x+0.125, -0.25*dimensions.y+0.25));
		shape2.setOrigin(Vec2(0.5*dimensions.x+0.125, -0.25*dimensions.y+0.25));
	} else
	{
		shape1.setSize({0.25, 0.20});
		shape2.setSize({0.25, 0.20});
		shape1.setOrigin(Vec2(-0.5*dimensions.x+0.125, -0.5*dimensions.y+0.2));
		shape2.setOrigin(Vec2(0.5*dimensions.x+0.125, -0.5*dimensions.y+0.2));
	}
	shape1.setScale(shipScale, shipScale);
	shape1.setFillColor(sf::Color(200, 200, 200));
	shape2.setScale(shipScale, shipScale);
	shape2.setFillColor(sf::Color(200, 200, 200));
}
void Motor::Effect(float& pos, float& vel, float& acc, float& energy, float deltaTime, float mass)
{
	if (energy > 0)
	{
		float actualSpeed = speed/mass;
		if (energy <= speed)
		{
			actualSpeed = energy;
			moving = false;
			stopping = true;
		}
		currentDistance = pos-targetPos;
		if (std::abs(prevDistance) < std::abs(currentDistance))
		{
			moving = false;
			currentDistance = 0;
			// Drift planned.
			vel = 0;
			return;
		}
		if (currentDistance > 0)
		{
			if (currentDistance > targetDistance/2)
	    		acc = -actualSpeed;
	    	else
	    		acc = actualSpeed;
		} else
		{
			if (currentDistance < targetDistance/2)
	    		acc = actualSpeed;
	    	else
	    		acc = -actualSpeed;
		}
		prevDistance = currentDistance;
		energy -= speed*deltaTime;
	}
}
void Motor::Effect2(float& pos, float& vel, float& acc, float& energy, float deltaTime, float mass)
{
	if (energy > 0)
	{
		float actualSpeed = speed/mass;
		if (energy <= speed)
			actualSpeed = energy;
		float currentLastDistance = pos-lastTargetPos;
		if (currentLastDistance>0)
			acc = +actualSpeed;
		else
			acc = -actualSpeed;
		if (std::abs(lastPrevDistance) < std::abs(currentLastDistance))
	    {
	        vel = 0;
	        stopping = false;
	    }
	    lastPrevDistance = currentLastDistance;
	    energy -= speed*deltaTime;
	}
}
void Motor::EffectUpdate(Vec2& pos, Vec2& vel, Vec2& acc, float& energy, float deltaTime, short unsigned int& isMoving, float mass)
{
	if (health > 0)
    {
    	if (horizontal)
    	{
    		if (stopping)
	    	{
	    		Effect2(pos.x, vel.x, acc.x, energy, deltaTime, mass);
	    	} else if (moving)
	    	{
	    		Effect(pos.x, vel.x, acc.x, energy, deltaTime, mass);
	    		isMoving += 1;
	    	}
    	} else
    	{
    		if (stopping)
	    	{
	    		Effect2(pos.y, vel.y, acc.y, energy, deltaTime, mass);
	    	} else if (moving)
	    	{
	    		Effect(pos.y, vel.y, acc.y, energy, deltaTime, mass);
	    		isMoving += 1;
	    	}
    	}
    	
    }
}
void Motor::Draw(sf::RenderWindow* window)
{
	window->draw(shape1);
	window->draw(shape2);
}
sf::RectangleShape* Motor::GetShape1()
{
	return &shape1;
}
sf::RectangleShape* Motor::GetShape2()
{
	return &shape2;
}
Module* Motor::CreateCopy()
{
	Motor* mot = new Motor();
	mot->type = type;
	mot->fullHealth = fullHealth;
	mot->health = fullHealth;
	mot->horizontal = horizontal;
	mot->maxSpeed = maxSpeed;
	mot->speed = maxSpeed;
	mot->shape1.setSize(shape1.getSize());
	mot->shape2.setSize(shape2.getSize());
	mot->shape1.setPosition(shape1.getPosition());
	mot->shape2.setPosition(shape2.getPosition());
	mot->shape1.setOrigin(shape1.getOrigin());
	mot->shape2.setOrigin(shape2.getOrigin());
	mot->shape1.setScale(shape1.getScale());
	mot->shape2.setScale(shape2.getScale());
	mot->shape1.setFillColor(shape1.getFillColor());
	mot->shape2.setFillColor(shape2.getFillColor());
	return mot;
}	
MassShield::MassShield(float extraHealth)
{
	visible=false;
	fullHealth=extraHealth;
	health=fullHealth;
}
Module* MassShield::CreateCopy()
{
	MassShield* sh = new MassShield();
	sh->type = type;
	sh->fullHealth = fullHealth;
	sh->health = fullHealth;
	return sh;
}