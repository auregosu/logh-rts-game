#include "WeaponTypes.hpp"

Cannon::Cannon(float shipScale, Vec2 dimensions)
{
	type = Module::Type::CANNON;
	fullHealth = 10;
	health = fullHealth;
	power = 5;
	range = 300;
	recoil = 1;
	shape.setSize({0.5, 0.5});
	shape.setOrigin({0.25, 0.5*dimensions.y+0.25});
	shape.setScale(shipScale, shipScale);
	shape.setFillColor(Palette::Tech1);
}

void Cannon::Effect(float& energy, bool& shipAttacking)
{
	sf::Color newColor = beam.getFillColor();
	newColor.a = 0;
	if (health > 0)
	{
		float actualPower = power;
		if (energy < power)
			actualPower = energy;
		beam.setPosition(*targetPos);
		float cannonPosX = shape.getTransform().transformPoint(shape.getSize().x*0.5, 0).x;
		float cannonPosY = shape.getTransform().transformPoint(shape.getSize().y*0.5, 0).y;
		float difX = cannonPosX-targetPos->x;
		float difY = cannonPosY-targetPos->y;
		targetDistance = sqrtf((difX*difX)+(difY*difY));
		if (targetDistance <= range+1 && energy > 0)
		{
			if (accumulator >= recoil)
			{
				newColor.a = 255;
				beam.setSize({targetDistance, recoil});
	        	float nX = difX/targetDistance;
	        	float nY = difY/targetDistance;
	            float tangent = atan2(nX, nY)*180/M_PI;
				beam.setRotation(-tangent+90);
				accumulator = 0;
				float damage = actualPower+(range/targetDistance);
				*targetHealth -= damage;
				energy -= actualPower;
			}
		}
		if (*targetHealth <= 0)
		{
			attacking = false;
		}
		shipAttacking = true;
	}
	beam.setFillColor(newColor);
}
void Cannon::Draw(sf::RenderWindow* window)
{
	window->draw(shape);
}
sf::RectangleShape* Cannon::GetShape1()
	{return &shape;}
sf::RectangleShape* Cannon::GetShape2()
	{return &shape;}

Module* Cannon::CreateCopy()
{
	Cannon* can = new Cannon();
	can->type = type;
	can->fullHealth = fullHealth;
	can->health = fullHealth;
	can->power = power;
	can->range = range;
	can->recoil = recoil;
	can->shape.setSize(shape.getSize());
	can->shape.setPosition(shape.getPosition());
	can->shape.setOrigin(shape.getOrigin());
	can->shape.setScale(shape.getScale());
	can->shape.setFillColor(shape.getFillColor());
	return can;
}