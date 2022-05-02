#include "Ship.hpp"

Ship::Ship(Player* _player, Vec2 _pos, std::string _localShape)
{
	player = _player;
	ships.insert(this);
	pos = _pos;
	localShape = shipShapes[_localShape];
	shape = *localShape;
	shape.setOrigin(localShape->getOrigin().x, localShape->getOrigin().y);
	shape.setFillColor(player->color);
	shape.setRotation(0);
    MassShield* shield = new MassShield(mass*5);
    modules.push_back(shield);
	Cannon* cannon = new Cannon(shipScale, {1, 2});
	weapons.push_back(std::move(cannon));
    cannon->shape.setPosition(pos);
	for (auto& w : weapons)
	{
		modules.push_back(w);
	}
	motorX = new Motor(true, 1, shipScale, shape.getGlobalBounds().width, shape.getGlobalBounds().height);
	modules.push_back(motorX);
	motorY = new Motor(false, 1, shipScale, shape.getGlobalBounds().width, shape.getGlobalBounds().height);
	modules.push_back(motorY);
	shape.setScale(shipScale, shipScale);
	ghost = shape;
	sf::Color ghostColor = shape.getFillColor();
	ghostColor.a = 100;
	ghost.setFillColor(ghostColor);
    mass = 2;
	fullHealth = 0;
    
	for (auto& m : modules)
	{
		fullHealth += m->fullHealth;
	}
	health = fullHealth;
	energy = fullEnergy;
	energyBar.setFillColor(sf::Color::Blue);
}
Ship::Ship(Player* _player, Vec2 _pos, Ship blueprintShip)
{
    player = _player;
    ships.insert(this);
    pos = _pos;
    localShape = blueprintShip.localShape;
    shape = *localShape;
    shape.setOrigin(localShape->getOrigin().x, localShape->getOrigin().y);
    shape.setFillColor(player->color);
    shape.setRotation(0);
    MassShield* shield = new MassShield(mass*5);
    modules.push_back(shield);
    for (auto& m : blueprintShip.modules)
    {
        if (m->visible)
        {
            m->GetShape1()->setScale(Vec2(shipScale, shipScale));
            m->GetShape2()->setScale(Vec2(shipScale, shipScale));
        }
        Module* mod = m->CreateCopy();
        if (mod->weapon)
            weapons.push_back(dynamic_cast<Weapon*>(mod));
        modules.push_back(mod);
    }

    for (auto& m : modules)
    {
        if (m->type == Module::Type::MOTORX || m->type == Module::Type::MOTORY)
        {
            Motor* mot = dynamic_cast<Motor*>(m);
            if (mot->horizontal)
                motorX = mot;
            else
                motorY = mot;
        }
    }
    shape.setScale(shipScale, shipScale);
    ghost = shape;
    sf::Color ghostColor = shape.getFillColor();
    ghostColor.a = 100;
    ghost.setFillColor(ghostColor);
    mass = blueprintShip.mass;
    fullHealth = 0;
    for (auto& m : modules)
    {
        fullHealth += m->fullHealth;
    }

    health = fullHealth;
    energy = fullEnergy;
    energyBar.setFillColor(sf::Color::Blue);
}
void Ship::Move(Vec2 destination, float destinatedRotation, Entity* destinationEntity = nullptr, float range = 0)
{
	unsigned short int willRotate = 0;
	if (destinationEntity != nullptr)
	{
		float difX = pos.x-destinationEntity->pos.x;
        float difY = pos.y-destinationEntity->pos.y;
        float distance = sqrtf((difX*difX)+(difY*difY));
    	float nx = difX/distance;
        float ny = difY/distance;
        float newX = nx*range;
        float newY = ny*range;
        destination = {destinationEntity->pos.x+newX, destinationEntity->pos.y+newY};
        difX = destination.x-destinationEntity->pos.x;
        difY = destination.y-destinationEntity->pos.y;
        distance = sqrtf((difX*difX)+(difY*difY));
    	nx = difX/distance;
        ny = difY/distance;
        float currentRotation = shape.getRotation();
        float tangent = (atan2(nx, ny)*180/M_PI);
        destinatedRotation = -tangent;
        if (destinatedRotation < 0)
        	destinatedRotation += 360;
       	prevRotation = currentRotation;
	}
	if (motorX != nullptr)
	{
		motorX->lastTargetPos = motorX->targetPos;
		motorX->targetPos = destination.x;
		motorX->targetDistance = pos.x-destination.x;
		motorX->prevDistance = motorX->targetDistance; 
		willRotate += 1;
		if (motorX->moving)
			motorX->stopping = true;
		else
			motorX->moving = true;
	}
	if (motorY != nullptr)
	{
		vel.y = 0;
		motorY->targetPos = destination.y;
		motorY->targetDistance = pos.y-destination.y;
		motorY->prevDistance = motorY->targetDistance;
		motorY->moving = true;
		willRotate += 1;
	}
	if (willRotate > 1)
	{
		targetRotation = destinatedRotation;
        prevRotation = shape.getRotation();
	}
}

void Ship::SelectWeapon(long unsigned int j)
{
    for (long unsigned int i = 0; i < weapons.size(); i++)
    {
        sf::Color color = sf::Color(200, 200, 200);
        if (i == j)
        {
            selectedWeapon = weapons[j];
            if (selectedWeapon->selected)
            {
                selectedWeapon->selected = false;
                selectedWeapon = nullptr;
            } else
            {
                selectedWeapon->selected = true;
                color.r += 20;
                color.g += 20;
                color.b += 20;
            }
        } else
            weapons[i]->selected = false;
        weapons[i]->shape.setFillColor(color);
    }
}

bool Ship::ClickSelect(Player* clickPlayer, Vec2 mousePos, bool isShiftPressed)
{
    bool returnValue = false;
    selectedShips.clear();
    for (auto& ship : ships)
    {   
        if (ship->shape.getGlobalBounds().contains(mousePos.x, mousePos.y) && ship->player == clickPlayer)
        {
            ship->selected = true;
            ship->shape.setFillColor(Palette::Highlight1);
            selectedShips.insert(ship);
            returnValue = true;
        } else 
        {
            if (ship->selected && isShiftPressed)
            {
                ship->selected = true;
                ship->shape.setFillColor(Palette::Highlight1);
                selectedShips.insert(ship);
            } else
            {
                ship->selected = false;
                ship->shape.setFillColor(ship->player->color);
            }
        }
    }
    return returnValue;
}
void Ship::SquareSelect(Player* clickPlayer, bool isShiftPressed)
{
    selectedShips.clear();
    for (auto& ship : ships)
    {   
        if (ship->shape.getGlobalBounds().intersects(clickPlayer->selectionSquare.getGlobalBounds()) && ship->player == clickPlayer)
        {
            ship->selected = true;
            ship->shape.setFillColor(Palette::Highlight1);
            selectedShips.insert(ship);
        } else 
        {
            if (ship->selected && isShiftPressed)
            {
                ship->selected = true;
                ship->shape.setFillColor(Palette::Highlight1);
                selectedShips.insert(ship);
            } else
            {
                ship->selected = false;
                ship->shape.setFillColor(ship->player->color);
            }
        }
    }
}

void Ship::Update()
{
    for (auto& p : ships)
    {   
    	if (p->health < p->fullHealth)
    	{
    		float prevModuleHealth = 0;
    		for (auto& m : p->modules)
    		{
    			prevModuleHealth += m->fullHealth;
    			if (m->health > 0)
    			{
    				float dif = p->health-p->fullHealth;
    				m->health = dif+prevModuleHealth;
    				if (m->health > m->fullHealth)
    					m->health = m->fullHealth;
    					
    			}
    		}
    	}
    	if (p->health <= 0)
    	{
    		//ships.erase(p);
    		continue;
    	}
    	if (p->energy <= 0)
    	{
    		p->energy = 0;
    	}
        p->acc.x = 0; p->acc.y = 0;
        unsigned short int isMoving = 0;
        // Movement
        for (auto& m : p->modules)
        {
        	if (m->type == Module::Type::MOTORX || m->type == Module::Type::MOTORY)
        	{
        		Motor* mot = dynamic_cast<Motor*>(m);
        		mot->EffectUpdate(p->pos, p->vel, p->acc, p->energy, deltaTime, isMoving, p->mass);
        	}
            if (m->visible)
            {
                m->GetShape1()->setPosition(p->pos);
                m->GetShape2()->setPosition(p->pos);
                m->GetShape1()->setRotation(p->shape.getRotation());
                m->GetShape2()->setRotation(p->shape.getRotation());
            }
        }
        if (isMoving > 0)
        {
        	p->moving = true;
        } else
        	p->moving = false;
        if (p->motorX != nullptr && p->motorY != nullptr && p->motorX->health > 0 && p->motorY->health > 0)
        	p->canRotate = true;
        else
            p->canRotate = false;
       	if (p->energy > 0 && p->canRotate && (p->moving || p->attacking))
    	{
    		//std::cout << "currentRotation: " << p->shape.getRotation() << std::endl;
    		//std::cout << "targetRotation: " << p->targetRotation << std::endl;
    		float rotationSpeed = 10*(p->motorX->speed+p->motorY->speed)*deltaTime/p->mass;
    		float dif = p->targetRotation-p->shape.getRotation();
    		short unsigned int q = 0;
    		if (dif > 0 && dif < 180)
    			q = 1;
    		else if (dif > 0 && dif > 180)
    			q = 2;
    		else if (dif < 0 && dif > -180)
    			q = 3;
    		else if (dif < 0 && dif < -180)
    			q = 4;
    		if (std::abs(dif) <= rotationSpeed)
    			q = 0;
    		switch (q)
    		{
    			case 1:
    				if (p->shape.getRotation() < p->targetRotation)
	    				p->shape.setRotation(p->shape.getRotation()+rotationSpeed);
    				break;
    			case 2:
    				if (p->shape.getRotation() < p->targetRotation)
	    				p->shape.setRotation(p->shape.getRotation()-rotationSpeed);
    				break;
    			case 3:
    				if (p->shape.getRotation() > p->targetRotation)
	    				p->shape.setRotation(p->shape.getRotation()-rotationSpeed);
    				break;
    			case 4:
    				if (p->shape.getRotation() > p->targetRotation)
	    				p->shape.setRotation(p->shape.getRotation()+rotationSpeed);
    				break;
    			default:
    			p->shape.setRotation(p->targetRotation);
    		}
    	}
        // Collision.
        for (auto& p2 : ships)
        {
            if (p2 != p)
            {
                if (p2->shape.getGlobalBounds().intersects(p->shape.getGlobalBounds()))
                {
                    //p->speed = p->maxSpeed*0.1;
                } else
                {
                    //p->speed = p->maxSpeed;
                }

            }
        }
        // POSITIONING.
        // Weapon position.
        p->attacking = false;
        for (auto& w : p->weapons)
        {
            w->accumulator += deltaTime;
        	if (w->attacking == true)
        	{
        		w->Effect(p->energy, p->attacking);
        	}
        }
        // Velocity and acceleration.
        p->vel.x += p->acc.x*deltaTime; p->vel.y += p->acc.y*deltaTime;
        p->pos.x += p->vel.x; p->pos.y += p->vel.y;
        p->shape.setPosition(p->pos);
        p->globalBounds = p->shape.getGlobalBounds();
        // UI
        p->lifeBar.setSize({p->health/p->fullHealth*shipScale, shipScale/5});
        p->lifeBar.setPosition(p->pos);
        p->energyBar.setSize({p->energy/p->fullEnergy*shipScale, shipScale/10});
        p->energyBar.setPosition({p->pos.x, p->pos.y+shipScale/5});
    }
}

Supply::Supply(Vec2 _pos, Ship* _ship, Player* _player)
{
    supplies.insert(this);
    ship = _ship;
    player = _player;
    pos = _pos;
    energy = fullEnergy;
    speed = maxSpeed;
    health = fullHealth;
    shape.setSize({Ship::shipScale*0.5f, Ship::shipScale*0.5f});
    shape.setFillColor(player->color);
    Move({0, 0}, ship);
}

void Supply::Move(Vec2 destination, Ship* destinationShip)
{
    if (destinationShip != nullptr)
    {
        float difX = pos.x-destinationShip->pos.x;
        float difY = pos.y-destinationShip->pos.y;
        targetDistance = sqrtf((difX*difX)+(difY*difY));
        prevDistance = targetDistance;
        currentDistance = targetDistance;
        destination = destinationShip->pos;
    }
    lastTargetPos = targetPos;
    targetPos = destination;
    if (moving)
    {
        stopping = true;
        moving = false;
        startingMovement = false;
    }
    else
    {
        moving = true;
        startingMovement = true;
        stopping = false;
    }
}
void Supply::Update()
{
    for (auto& s : supplies)
    {
        if (s->shape.getGlobalBounds().intersects(s->ship->shape.getGlobalBounds()))
        {
            if (s->health > 0)
            	s->ship->energy += s->energy;
            if (s->ship->energy > s->ship->fullEnergy)
                s->ship->energy = s->ship->fullEnergy;
            s->health = 0;
        }
        if (s->health <= 0)
        {
            //supplies.erase(s);
            continue;
        }
        s->acc.x = 0; s->acc.y = 0;
        if (s->moving)
        {
            if (s->energy > 0)
            {
                float actualSpeed = s->speed;
                if (s->energy <= s->speed)
                {
                    s->moving = false;
                    s->stopping = true;
                }
                float difX = s->pos.x-s->ship->pos.x;
                float difY = s->pos.y-s->ship->pos.y;
                s->currentDistance = sqrtf((difX*difX)+(difY*difY));
                float nX = difX/s->currentDistance;
                float nY = difY/s->currentDistance;
                if (s->prevDistance < s->currentDistance && !s->startingMovement)
                {
                    s->moving = false;
                    s->stopping = true;
                    s->currentDistance = 0;
                    s->startingMovement = true;
                    return;
                }
                if (s->currentDistance > s->targetDistance*0.5f)
                {
                    s->acc.x = -actualSpeed*nX;
                    s->acc.y = -actualSpeed*nY;
                } else
                {
                    s->acc.x = actualSpeed*nX;
                    s->acc.y = actualSpeed*nY;
                    s->startingMovement = false;
                }
                s->prevDistance = s->currentDistance;
                s->energy -= s->speed*deltaTime;
            }
        } else if (s->stopping)
        {
            float actualSpeed = s->speed;
            float difX = s->pos.x-s->lastTargetPos.x;
            float difY = s->pos.y-s->lastTargetPos.y;
            float currentLastDistance = sqrtf((difX*difX)+(difY*difY));
            float nX = difX/currentLastDistance;
            float nY = difY/currentLastDistance;
            s->acc.x = actualSpeed*nX;
            s->acc.y = actualSpeed*nY;
            if (s->lastPrevDistance < currentLastDistance && !s->startingMovement)
            {
                s->vel = {0, 0};
                s->Move({0, 0}, s->ship);
                return;
            }
            s->lastPrevDistance = currentLastDistance;
            s->startingMovement = false;
            s->energy -= s->speed*deltaTime;
        }
        // Velocity, acceleration and position.
        s->vel.x += s->acc.x*deltaTime; s->vel.y += s->acc.y*deltaTime;
        s->pos.x += s->vel.x; s->pos.y += s->vel.y;
        s->shape.setPosition(s->pos);
        s->globalBounds = s->shape.getGlobalBounds();
    }
}

Settlement::Settlement(Vec2 _pos, Player* _player)
{
    settlements.insert(this);
    player = _player;
    pos = _pos;
    shape.setRadius(2);
    shape.setOrigin({2, 2});
    shape.setPointCount(6);
    shape.setFillColor(player->color);
    shape.setScale({Ship::shipScale, Ship::shipScale});
    health = fullHealth;
}

bool Settlement::ClickSelect(Player* clickPlayer, Vec2 mousePos, bool isShiftPressed)
{
    bool returnValue = false;
    selectedSettlements.clear();
    for (auto& settle : settlements)
    {   
        if (settle->shape.getGlobalBounds().contains(mousePos.x, mousePos.y) && settle->player == clickPlayer)
        {
            settle->selected = true;
            settle->shape.setFillColor(Palette::Highlight1);
            selectedSettlements.insert(settle);
            returnValue = true;
        } else 
        {
            if (settle->selected && isShiftPressed)
            {
                settle->selected = true;
                settle->shape.setFillColor(Palette::Highlight1);
                selectedSettlements.insert(settle);
            } else
            {
                settle->selected = false;
                settle->shape.setFillColor(settle->player->color);
            }
        }
    }
    return returnValue;
}

void Settlement::ClearSelect()
{
    for (auto& settle : selectedSettlements)
    {
        settle->selected = false;
        settle->shape.setFillColor(settle->player->color);
    }
    selectedSettlements.clear();
}

void Settlement::Update()
{
    for (auto& s : settlements)
    {
        if (s->player->action == PlayerAction::CHANGING_SUPPLIES)
        {
            for (auto& ship : s->ships)
            {
                std::array<sf::Vertex, 2> line = {
                sf::Vertex(sf::Vector2f(s->pos)),
                sf::Vertex(sf::Vector2f(ship->pos))
                };
                s->mouseLines[ship] = line;
            }
        }
        s->accumulator += deltaTime;
        if (s->accumulator >= s->time)
        {
            for (auto& ship : s->ships)
            {
                if (ship->health > 0)
                    Supply* sup = new Supply(s->pos, ship, s->player);
            }
            s->accumulator = 0;
        }
        s->shape.setPosition(s->pos);
        s->globalBounds = s->shape.getGlobalBounds();
    }
}

std::unordered_set<Entity*> Entity::entities;
std::map<std::string, ShipShape*> Ship::shipShapes;
std::unordered_set<Ship*> Ship::ships;
std::unordered_set<Ship*> Ship::selectedShips;
float Ship::shipScale = 60;
std::unordered_set<Supply*> Supply::supplies;
std::unordered_set<Settlement*> Settlement::settlements;
std::unordered_set<Settlement*> Settlement::selectedSettlements;