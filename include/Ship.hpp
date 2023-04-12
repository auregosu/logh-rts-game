#ifndef SHIP_HPP
#define SHIP_HPP
#include "ModuleTypes.hpp"
#include "WeaponTypes.hpp"

typedef sf::ConvexShape ShipShape;

extern float deltaTime;

class Entity
{
public:
    static std::unordered_set<Entity*> entities, deadEntities;
    Player* player = nullptr;
    sf::FloatRect globalBounds;
    Vec2 pos;
    float health, fullHealth;
    bool dead;

    inline Entity()
    {
        dead = false;
        entities.insert(this);
    }
    Entity* isColliding();
    inline virtual std::vector<Vec2> GetPoints() {}
    inline static Entity* Click(Vec2 mousePos)
    {
        for (auto& entity : entities)
        {   
            if (entity->globalBounds.contains(mousePos.x, mousePos.y))
                return entity;
        }
        return nullptr;
    }
    inline virtual void Draw(sf::RenderWindow* window, GameStates gameState) {}
    inline virtual int GetPointNum() {}
    virtual void Kill();
    static void KillDeadEntities();
};

class Ship : public Entity
{
public:
    static float shipScale;
    static std::map<std::string, ShipShape> shipShapes;
    static std::unordered_set<Ship*> ships, selectedShips;
    std::vector<Module*> modules;
    std::vector<Weapon*> weapons;
    Weapon* selectedWeapon = nullptr;
    Motor* motorX = nullptr;
    Motor* motorY = nullptr;
    ShipShape* localShape = nullptr;
    ShipShape shape;
    ShipShape ghost;
    sf::RectangleShape lifeBar, energyBar;
    Vec2 vel, acc, prevGhostPos;
    float prevRotation, targetRotation, energy, fullEnergy=25, mass=1;
    bool selected = false, wannaMove = false, moving = false, attacking = false, canRotate = false;

    Ship() {};
    Ship(Player* _player, Vec2 _pos, std::string _localShape);
    Ship(Player* _player, Vec2 _pos, Ship blueprintShip);
    inline void Kill()
    {
        ships.erase(this);
        entities.erase(this);
        dead = true;
        deadEntities.insert(this);
    }

    std::vector<Vec2> GetPoints();

    void Move(Vec2 destination, float destinatedRotation, Entity* destinationEntity, float range);

    void Stop();

    inline void Attack(Entity* entity, Module* module, bool move)
    {
        if (entity->player != player)
        {
            Weapon* weapon = dynamic_cast<Weapon*>(module);
            weapon->targetPos = &(entity->pos);
            weapon->targetHealth = &(entity->health);
            if (!moving && move)
                Move({0, 0}, 0, entity, weapon->range);
            weapon->attacking = true;
        }
    }

    inline Module* GetModule(Module::Type _type)
    {
        for (auto& m : modules)
        {   
            if (m->type == _type)
                return m;
        }
        return nullptr;
    }

    inline int GetPointNum() {return shape.getPointCount();}

    void SelectWeapon(long unsigned int j);
    
    void Draw(sf::RenderWindow* window, GameStates gameState);

    static void Update();

    inline static Ship* Click(Vec2 mousePos)
    {
        for (auto& ship : ships)
        {   
            if (ship->shape.getGlobalBounds().contains(mousePos.x, mousePos.y))
                return ship;
        }
        return nullptr;
    }
    static bool ClickSelect(Player* clickPlayer, Vec2 mousePos, bool isShiftPressed);

    static void SquareSelect(Player* clickPlayer, bool isShiftPressed);
};

class Supply : public Entity
{
public:
    static std::unordered_set<Supply*> supplies;
    Ship* ship = nullptr;
    sf::RectangleShape shape;
    Vec2 vel = {0,0}, acc, lastTargetPos, targetPos;
    float fullHealth=10, energy, fullEnergy=100, prevDistance, lastPrevDistance, targetDistance, currentDistance, speed, maxSpeed=2;
    bool moving = false, stopping = false, startingMovement = false;

    std::vector<Vec2> GetPoints();

    Supply(Vec2 _pos, Ship* _ship, Player* _player);

    void Move(Vec2 destination, Ship* destinationShip = nullptr);

    void Draw(sf::RenderWindow* window, GameStates gameState);

    inline int GetPointNum() {return shape.getPointCount();}
    static void Update();
};

class Settlement : public Entity
{
public:
    static std::unordered_set<Settlement*> settlements;
    static std::unordered_set<Settlement*> selectedSettlements;
    std::unordered_set<Ship*> ships;
    std::map<Ship*, std::array<sf::Vertex, 2>> mouseLines;
    sf::CircleShape shape;
    float selected=false, fullHealth=100, accumulator=0, time=5;

    void Kill()
    {
        settlements.erase(this);
        entities.erase(this);
        dead = true;
        deadEntities.insert(this);
    }
    std::vector<Vec2> GetPoints();

    Settlement(Vec2 _pos, Player* _player);

    void Insert(Ship* shipClicked)
    {
        if (shipClicked-> player == player)
        {
            ships.insert(shipClicked);
            std::array<sf::Vertex, 2> line = {
                sf::Vertex(sf::Vector2f(pos)),
                sf::Vertex(sf::Vector2f(shipClicked->pos))
            };
            mouseLines.insert(std::make_pair(shipClicked, line));
        }
    }

    void Remove(Ship* shipClicked)
    {
        if (shipClicked-> player == player)
        {
            ships.erase(shipClicked);
            mouseLines.erase(shipClicked);
        }
    }

    void Draw(sf::RenderWindow* window, GameStates gameState);

    inline int GetPointNum() {return shape.getPointCount();}
    static bool ClickSelect(Player* clickPlayer, Vec2 mousePos, bool isShiftPressed);

    static void ClearSelect();

    static void Update();
}; 
#endif
