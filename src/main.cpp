#include "Timestep.hpp"
#include "GUI.hpp"

using namespace std::literals::chrono_literals;

float camSpeed = 8;

MenuScreens menuScreen = SHAPEMAKE;
GameStates gameState = MENU;

int main()
{
    sf::RenderWindow window(sf::VideoMode(SCREENWIDTH, SCREENHEIGHT), "Pizza");
    window.setFramerateLimit(int(idealFramerate));
    sf::View view = window.getDefaultView();
    window.setView(view);
    Palette::Initialize();

    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
    sf::Vector2f mousePos = window.mapPixelToCoords(pixelPos);

    sf::Vertex mouseLine[] =
    {
        sf::Vertex(sf::Vector2f(0.f, 0.f)),
        sf::Vertex(sf::Vector2f(0.f, 0.f))
    };

    ShipShape *onebytwo = new ShipShape();
    onebytwo->setPointCount(4);
    onebytwo->setPoint(0, Vec2(0, 0));
    onebytwo->setPoint(1, Vec2(0, -2));
    onebytwo->setPoint(2, Vec2(1, -2));
    onebytwo->setPoint(3, Vec2(1, 0));
    onebytwo->setOrigin(0.5, -1);
    Ship::shipShapes.insert(std::make_pair("1x2", onebytwo));

    Player *player1 = new Player("aure", Palette::Player1);
    Player *player2 = new Player("juan", Palette::Player2);
    Player::mainPlayer = player1;
    for (int i = 0; i <= 8; i+=1)
    {
        Ship *ship = new Ship(player2, Vec2(i*100, 0), "1x2");
        ship->shape.rotate(180);
    }
    Settlement* settle = new Settlement({SCREENWIDTH/2, 900}, Player::mainPlayer);
    Settlement* settle2 = new Settlement({SCREENWIDTH/2, -300}, player2);

    BuildGrid grid(8);
    std::vector<Module::Type> availableModules;
    availableModules.push_back(Module::Type::MOTORX);
    availableModules.push_back(Module::Type::MOTORY);
    availableModules.push_back(Module::Type::CANNON);
    ModuleSelector moduleSelector(availableModules);

    Widget modulePoints = Widget(true, Vec2(SCREENWIDTH*0.85, SCREENHEIGHT*0.2), Vec2(100, 100), Palette::Color2, "0/"+std::to_string(grid.size));
    Button shapeMakeButton = Button(true, Vec2(SCREENWIDTH*0.85, SCREENHEIGHT*0.85), Vec2(200, 126), Player::mainPlayer->color, "DONE");
    Button buildButton = Button(false, Vec2(SCREENWIDTH*0.85, SCREENHEIGHT*0.85), Vec2(200, 126), Player::mainPlayer->color, "BUILD");

    while (window.isOpen())
    {
        auto updateStart = std::chrono::steady_clock::now();

        pixelPos = sf::Mouse::getPosition(window);
        mousePos = window.mapPixelToCoords(pixelPos);

        sf::Event event;
        while (window.pollEvent(event))
        {
// INPUT.
            if (event.type == sf::Event::Closed)
                window.close();
            switch (gameState)
            {
                case GAME:
                    switch (event.type)
                    {
                        case sf::Event::MouseButtonPressed:
// Mouse press.
                            if (event.mouseButton.button == sf::Mouse::Left)
                            {
                                if (Player::mainPlayer->action == PlayerAction::NONE)
                                {
                                    bool wasShipClicked = Ship::ClickSelect(Player::mainPlayer, mousePos, sf::Keyboard::isKeyPressed(sf::Keyboard::LShift));
                                    bool wasSettleClicked = Settlement::ClickSelect(Player::mainPlayer, mousePos, sf::Keyboard::isKeyPressed(sf::Keyboard::LShift));
                                    if (!wasShipClicked && !wasSettleClicked)
                                    {
                                        Player::mainPlayer->pivot1 = sf::Vector2f(mousePos);
                                        Player::mainPlayer->selectionSquare.setPosition(Player::mainPlayer->pivot1);
                                        Player::mainPlayer->action = PlayerAction::SELECTING_SQUARE;
                                    } else if (wasSettleClicked)
                                    {
                                        for (auto& settle : Settlement::selectedSettlements)
                                        {
                                            mouseLine[0].position = settle->pos;
                                        }
                                        mouseLine[1].position = sf::Vector2f(mousePos);    
                                        Player::mainPlayer->action = PlayerAction::CHANGING_SUPPLIES;
                                    }
                                } else if (Player::mainPlayer->action == PlayerAction::CHANGING_SUPPLIES)
                                {
                                    Ship* shipClicked = Ship::Click(mousePos);
                                    if (shipClicked != nullptr)
                                    {
                                        for (auto& settle : Settlement::selectedSettlements)
                                        {
                                            settle->Insert(shipClicked);
                                        }
                                    }
                                }
                            } else
                            if (event.mouseButton.button == sf::Mouse::Right)
                            {
                                if (Player::mainPlayer->action == PlayerAction::NONE)
                                {
                                    Player::mainPlayer->shipAveragePos = {0, 0};
                                    if (Ship::selectedShips.size() > 1)
                                    {
                                        for (auto& ship : Ship::selectedShips)
                                        {
                                            Player::mainPlayer->shipAveragePos.x += ship->pos.x;
                                            Player::mainPlayer->shipAveragePos.y += ship->pos.y;
                                        }
                                        Player::mainPlayer->shipAveragePos.x /= Ship::selectedShips.size();
                                        Player::mainPlayer->shipAveragePos.y /= Ship::selectedShips.size();
                                    }
                                    for (auto& ship : Ship::selectedShips)
                                    {
                                        Entity* touchedEntity = Entity::Click(mousePos);
                                        if (touchedEntity != ship)
                                        {
                                            if (touchedEntity != nullptr) 
                                            {
                                                if (ship->selectedWeapon != nullptr)
                                                    ship->Attack(touchedEntity, ship->selectedWeapon, !sf::Keyboard::isKeyPressed(sf::Keyboard::LShift));
                                                else
                                                    ship->Move({0, 0}, 0, touchedEntity, Ship::shipScale*5);
                                            } else
                                            {
                                                Player::mainPlayer->pivot1 = sf::Vector2f(mousePos);
                                                Player::mainPlayer->prevShipRotation = ship->shape.getRotation();
                                                Vec2 ghostPos;
                                                if (Ship::selectedShips.size() == 1)
                                                    ghostPos = Player::mainPlayer->pivot1;
                                                else
                                                {
                                                    ghostPos = {
                                                    Player::mainPlayer->pivot1.x+ship->pos.x-Player::mainPlayer->shipAveragePos.x,
                                                    Player::mainPlayer->pivot1.y+ship->pos.y-Player::mainPlayer->shipAveragePos.y};
                                                }
                                                ship->ghost.setPosition(ghostPos);
                                                ship->prevGhostPos = ghostPos;
                                                mouseLine[0].position = Player::mainPlayer->pivot1;
                                                Player::mainPlayer->action = PlayerAction::COMMAND_SHIP_MOVEMENT_START;
                                            }
                                        }
                                    }
                                } else if (Player::mainPlayer->action == PlayerAction::CHANGING_SUPPLIES)
                                {
                                    Ship* shipClicked = Ship::Click(mousePos);
                                    if (shipClicked != nullptr)
                                    {
                                        for (auto& settle : Settlement::selectedSettlements)
                                        {
                                            settle->Remove(shipClicked);
                                        }
                                    } else
                                    {
                                        Settlement::ClearSelect();
                                        Player::mainPlayer->action = PlayerAction::NONE;
                                    }
                                }
                                
                            }
                            break;
                        case sf::Event::MouseButtonReleased:
// Mouse release.           
                            if (event.mouseButton.button == sf::Mouse::Right)
                            {
                                if (Player::mainPlayer->action == PlayerAction::COMMAND_SHIP_MOVEMENT_START ||
                                    Player::mainPlayer->action == PlayerAction::COMMAND_SHIP_MOVEMENT_ROTATE)
                                {
                                    for (auto& ship : Ship::selectedShips)
                                    {
                                        ship->Move(ship->ghost.getPosition(), ship->ghost.getRotation(), Ship::Click(Player::mainPlayer->pivot1), Ship::shipScale*5);
                                    }
                                    Player::mainPlayer->action = PlayerAction::NONE;
                                }
                            } else if (event.mouseButton.button == sf::Mouse::Left)
                            {
                                if (Player::mainPlayer->action == PlayerAction::SELECTING_SQUARE)
                                {
                                    Ship::SquareSelect(Player::mainPlayer, sf::Keyboard::isKeyPressed(sf::Keyboard::LShift));
                                    Player::mainPlayer->selectionSquare.setSize({0, 0});
                                    Player::mainPlayer->action = PlayerAction::NONE;
                                }
                            }
                            break;
                        case sf::Event::MouseMoved:
// Mouse move.
                            if (Player::mainPlayer->action == PlayerAction::CHANGING_SUPPLIES)
                            {
                                mouseLine[1].position = sf::Vector2f(mousePos);
                                if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
                                {
                                    Ship* shipClicked = Ship::Click(mousePos);
                                    if (shipClicked != nullptr)
                                    {
                                        for (auto& settle : Settlement::selectedSettlements)
                                        {
                                            settle->Insert(shipClicked);
                                        }
                                    }
                                }
                            }
                            if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
                            {
                                if (Player::mainPlayer->action == PlayerAction::COMMAND_SHIP_MOVEMENT_START)
                                    Player::mainPlayer->action = PlayerAction::COMMAND_SHIP_MOVEMENT_ROTATE;
                                for (auto& ship : Ship::selectedShips)
                                {
                                    if (Player::mainPlayer->action == PlayerAction::COMMAND_SHIP_MOVEMENT_ROTATE)
                                    {
                                        if (Ship::selectedShips.size() == 1)
                                        {
                                            mouseLine[1].position = ship->pos;
                                            float difX = ship->ghost.getPosition().x-mousePos.x;
                                            float difY = ship->ghost.getPosition().y-mousePos.y;
                                            float distance = sqrtf(difX*difX+difY*difY);
                                            float nX = difX/distance;
                                            float nY = difY/distance;
                                            float angle = (atan2(nX, nY)*180/M_PI);
                                            ship->ghost.setRotation(-angle);
                                        }
                                        else
                                        {
                                            Player::mainPlayer->shipAveragePos = {0, 0};
                                            if (Ship::selectedShips.size() > 1)
                                            {
                                                for (auto& ship : Ship::selectedShips)
                                                {
                                                    Player::mainPlayer->shipAveragePos.x += ship->pos.x;
                                                    Player::mainPlayer->shipAveragePos.y += ship->pos.y;
                                                }
                                                Player::mainPlayer->shipAveragePos.x /= Ship::selectedShips.size();
                                                Player::mainPlayer->shipAveragePos.y /= Ship::selectedShips.size();
                                            }
                                            mouseLine[1].position = Player::mainPlayer->shipAveragePos;
                                            if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                                            {
                                                float difX = ship->ghost.getPosition().x-mousePos.x;
                                                float difY = ship->ghost.getPosition().y-mousePos.y;
                                                float distance = sqrtf(difX*difX+difY*difY);
                                                float nX = difX/distance;
                                                float nY = difY/distance;
                                                float angle = (atan2(nX, nY)*180/M_PI);
                                                ship->ghost.setRotation(-angle);
                                            } else
                                            {
                                                float difX = Player::mainPlayer->pivot1.x-ship->prevGhostPos.x;
                                                float difY = Player::mainPlayer->pivot1.y-ship->prevGhostPos.y;
                                                float distance = sqrtf(difX*difX+difY*difY);
                                                float nX = difX/distance;
                                                float nY = difY/distance;
                                                float prevAngle = atan2(nX, nY);
                                                float difX2 = Player::mainPlayer->pivot1.x-mousePos.x;
                                                float difY2 = Player::mainPlayer->pivot1.y-mousePos.y;
                                                float distance2 = sqrtf(difX2*difX2+difY2*difY2);
                                                float nX2 = difX2/distance2;
                                                float nY2 = difY2/distance2;
                                                float angle = atan2(nX2, nY2);
                                                Vec2 newPos;
                                                newPos = {Player::mainPlayer->pivot1.x+cos(-angle+prevAngle+M_PI/2)*distance,
                                                        Player::mainPlayer->pivot1.y+sin(-angle+prevAngle+M_PI/2)*distance,
                                                };
                                                ship->ghost.setPosition(newPos);
                                                ship->ghost.setRotation(-Player::mainPlayer->prevShipRotation-(angle*180/M_PI));
                                            }
                                        }
                                    }
                                }
                            } else if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
                            {
                                if (Player::mainPlayer->action == PlayerAction::SELECTING_SQUARE)
                                {
                                    Vec2 selectionSize = {
                                        float(mousePos.x)-Player::mainPlayer->pivot1.x, 
                                        float(mousePos.y)-Player::mainPlayer->pivot1.y};
                                    Player::mainPlayer->selectionSquare.setSize(selectionSize);
                                }
                            }
                            break;
                        case sf::Event::KeyPressed:
// Key press.
                            switch(event.key.code)
                            {
                                case sf::Keyboard::Q:
                                    for (auto& ship : Ship::selectedShips)
                                        ship->SelectWeapon(0);
                                    break;
                                case sf::Keyboard::W:
                                    for (auto& ship : Ship::selectedShips)
                                        ship->SelectWeapon(1);
                                    break;
                                case sf::Keyboard::E:
                                    for (auto& ship : Ship::selectedShips)        
                                        ship->SelectWeapon(2);
                                case sf::Keyboard::R:
                                    for (auto& ship : Ship::selectedShips)
                                        ship->SelectWeapon(3);
                                    break;
                                case sf::Keyboard::P:
                                    //gameState = GameStates::MENU;
                                    break;
                                default:
                                    break;
                            }
                            break;
                        case sf::Event::MouseWheelScrolled:
// Mouse scroll.            
                            if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
                                view.zoom(1-event.mouseWheelScroll.delta*0.1);
                            break;
                    }
                    break;
                case MENU:
                    switch (event.type)
                    {
                        case sf::Event::MouseButtonPressed:
                            if (event.mouseButton.button == sf::Mouse::Left)
                            {
                                for (auto& widget : Widget::widgets)
                                {
                                    Button* button = (Button*)widget;
                                    if (button->shape.getGlobalBounds().contains(mousePos) && button->active &&
                                        button->value > -1 && button->value < 2)
                                    {
                                        button->shape.setFillColor(button->clickColor);
                                        if (button->value == 0)
                                            button->value = 1;
                                        else
                                            button->value = 0;
                                    }
                                }
                                switch (menuScreen)
                                {
                                    case SHAPEMAKE:
                                        grid.Click(mousePos);
                                        modulePoints.ChangeText(sf::String(std::to_string(grid.blockCount)+"/"+std::to_string(grid.size)));
                                        break;
                                    case BUILD:
                                        for (auto& b : moduleSelector.bools)
                                        {
                                            if (b.second)
                                            {
                                                if (moduleSelector.modules[b.first]->GetShape1()->getGlobalBounds().contains(mousePos))
                                                    Player::mainPlayer->pivot1 = mousePos-moduleSelector.modules[b.first]->GetShape1()->getPosition();
                                                if (moduleSelector.modules[b.first]->GetShape2()->getGlobalBounds().contains(mousePos))
                                                    Player::mainPlayer->pivot1 = mousePos-moduleSelector.modules[b.first]->GetShape2()->getPosition();
                                            }
                                        }
                                        break;
                                }       
                            }
                            break;
                        case sf::Event::MouseButtonReleased:
                            if (event.mouseButton.button == sf::Mouse::Left)
                            {
                                if (shapeMakeButton.value == 1)
                                {
                                    if (grid.blockCount > 0)
                                    {
                                        shapeMakeButton.shape.setFillColor(shapeMakeButton.color);
                                        shapeMakeButton.value = 0;
                                        shapeMakeButton.active = false;
                                        buildButton.active = true;
                                        grid.MakeShape();
                                        moduleSelector.points = grid.blockCount;
                                        moduleSelector.FitForShip(grid.fullShape->getScale().x, grid.fullShape->getOrigin(), grid.fullShape->getPosition(), grid.dimensions);
                                        moduleSelector.ActivateButtons();
                                        modulePoints.ChangeText(sf::String(std::to_string(moduleSelector.points)));
                                        menuScreen = MenuScreens::BUILD;
                                    } else
                                    {
                                        shapeMakeButton.shape.setFillColor(shapeMakeButton.color);
                                        shapeMakeButton.value = 0;
                                    }
                                }
                                for (auto& button : moduleSelector.buttons)
                                {
                                    if (button.second->shape.getGlobalBounds().contains(mousePos) && button.second->active)
                                    {
                                        if (button.second->value == 1 && moduleSelector.points > 0)
                                        {
                                            button.second->shape.setFillColor(button.second->clickColor);
                                            moduleSelector.bools[button.first] = true;
                                            moduleSelector.good2go[button.first] = false;
                                            moduleSelector.points -= 1;
                                        } else
                                        {
                                            button.second->shape.setFillColor(button.second->color);
                                            if (moduleSelector.points > 0 || ( moduleSelector.bools[button.first] == true && moduleSelector.points == 0))
                                            {
                                                moduleSelector.bools[button.first] = false;
                                                moduleSelector.good2go[button.first] = true;
                                                moduleSelector.points += 1;
                                            }

                                        }
                                        modulePoints.ChangeText(sf::String(std::to_string(moduleSelector.points)));
                                    }
                                    
                                }
                                if (buildButton.value == 1)
                                {
                                    bool canBuild = true;
                                    for (auto& check : moduleSelector.good2go)
                                    {
                                        if (!check.second)
                                        {
                                            canBuild = false;
                                            break;
                                        }
                                    }
                                    if (moduleSelector.points == grid.blockCount)
                                        canBuild = false;
                                    if (canBuild)
                                    {
                                        buildButton.shape.setFillColor(buildButton.color);
                                        buildButton.value = 0;
                                        buildButton.active = false;
                                        modulePoints.active = false;
                                        moduleSelector.DeactivateButtons();
                                        Ship blueprintShip;
                                        for (auto& m : moduleSelector.modules)
                                        {
                                            if (moduleSelector.bools[m.first])
                                            {
                                                Vec2 scale = grid.fullShape->getScale();
                                                Vec2 shapePos = grid.fullShape->getPosition();
                                                Vec2 mPos1 = m.second->GetShape1()->getPosition();
                                                Vec2 newOrigin1 = (shapePos-mPos1)/scale;
                                                Vec2 oldOrigin1 = m.second->GetShape1()->getOrigin();
                                                Vec2 mPos2 = m.second->GetShape2()->getPosition();
                                                Vec2 newOrigin2 = (shapePos-mPos2)/scale;
                                                Vec2 oldOrigin2 = m.second->GetShape2()->getOrigin();
                                                m.second->GetShape1()->setOrigin(oldOrigin1+newOrigin1);
                                                m.second->GetShape2()->setOrigin(oldOrigin2+newOrigin2);
                                                blueprintShip.modules.push_back(m.second);
                                            }
                                        }
                                        grid.fullShape->setScale(1, 1);
                                        blueprintShip.mass = (grid.blockCount+grid.dimensions.x*grid.dimensions.y)/2;
                                        //std::cout << blueprintShip.mass << std::endl;
                                        Ship::shipShapes.insert(std::make_pair("custom", grid.fullShape));
                                        blueprintShip.localShape = Ship::shipShapes["custom"];
                                        for (int i = 0; i < 5; i+=1)
                                        {
                                            Ship *ship = new Ship(player1, Vec2(i*200, 500), blueprintShip);
                                            //std::cout << ship->motorX << std::endl;
                                        }
                                        gameState = GameStates::GAME;
                                    } else
                                    {
                                        buildButton.shape.setFillColor(buildButton.color);
                                        buildButton.value = 0;
                                    }
                                }
                            }
                            break;
                        case sf::Event::MouseMoved:
                            if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
                            {
                                switch (menuScreen)
                                {
                                    case SHAPEMAKE:
                                        grid.ClickMove(mousePos);
                                        modulePoints.ChangeText(sf::String(std::to_string(grid.blockCount)+"/"+std::to_string(grid.size)));
                                        break;
                                    case BUILD:
                                        for (auto& b : moduleSelector.bools)
                                        {
                                            if (b.second)
                                            {
                                                if (moduleSelector.modules[b.first]->GetShape1()->getGlobalBounds().contains(mousePos))
                                                    moduleSelector.modules[b.first]->GetShape1()->setPosition(mousePos-Player::mainPlayer->pivot1);
                                                if (moduleSelector.modules[b.first]->GetShape2()->getGlobalBounds().contains(mousePos))
                                                    moduleSelector.modules[b.first]->GetShape2()->setPosition(mousePos-Player::mainPlayer->pivot1);
                                            }
                                        }
                                        break;
                                }       
                            }
                            break;
                        case sf::Event::KeyPressed:
                            switch(event.key.code)
                            {
                                case sf::Keyboard::P:
                                    //gameState = GameStates::GAME;
                                    break;
                            }
                            break;
                    }
                    break;
            }
        }
// LOGIC.
        switch (gameState)
            {
                case GAME:
                    if (pixelPos.y < 30)
                        view.move(0, -camSpeed);
                    else if (pixelPos.y > (int)window.getSize().y-30)
                        view.move(0, camSpeed);
                    if (pixelPos.x < 30)
                        view.move(-camSpeed, 0);
                    else if (pixelPos.x > (int)window.getSize().x-30)
                        view.move(camSpeed, 0);
                    break;
                case MENU:
                    switch (menuScreen)
                    {
                        case BUILD:
                            for (auto& b : moduleSelector.bools)
                            {
                                if (b.second)
                                {
                                    int allShapes = 0;
                                    //if (false)
                                    if (Collisions::areOverlapping(ShapeToVector(*(grid.fullShape)), ShapeToVector(*(moduleSelector.modules[b.first]->GetShape1()))))
                                    {
                                       moduleSelector.modules[b.first]->GetShape1()->setFillColor(Palette::Tech1);
                                        allShapes += 1;
                                    }
                                    else
                                    {
                                        std::cout << "shape1\n";
                                        moduleSelector.modules[b.first]->GetShape1()->setFillColor(Palette::Highlight2);
                                    }
                                    if (Collisions::areOverlapping(ShapeToVector(*(grid.fullShape)), ShapeToVector(*(moduleSelector.modules[b.first]->GetShape2()))))
                                    {
                                        moduleSelector.modules[b.first]->GetShape2()->setFillColor(Palette::Tech1);
                                        allShapes += 1;
                                    }
                                    else
                                    {
                                        moduleSelector.modules[b.first]->GetShape2()->setFillColor(Palette::Highlight2);
                                    }
                                    if (allShapes > 1)
                                        moduleSelector.good2go[b.first] = true;
                                    else
                                        moduleSelector.good2go[b.first] = false;
                                }
                            }
                            break;
                    }
                    break;
            }
        Ship::Update();
        Settlement::Update();
        Supply::Update();
        Entity::KillAllEntities();
// DISPLAY.
        window.setView(view);
        window.clear(Palette::Color1);

        for (auto& e : Settlement::settlements)
            e->Draw(&window, gameState, Player::mainPlayer->action, mouseLine);
        for (auto& e : Supply::supplies)
            e->Draw(&window, gameState, Player::mainPlayer->action, mouseLine);
        for (auto& e : Ship::ships)
            e->Draw(&window, gameState, Player::mainPlayer->action, mouseLine);

        switch (gameState)
        {
            case GAME:
            if (Player::mainPlayer->action == PlayerAction::SELECTING_SQUARE)
                    window.draw(Player::mainPlayer->selectionSquare);
                break;
            case MENU:
                switch(menuScreen)
                {
                    case SHAPEMAKE:
                        for (auto& block : grid.shapes)
                            window.draw(block.second);
                        break;
                    case BUILD:
                            window.draw(*grid.fullShape);
                            for (auto& m : moduleSelector.modules)
                            {
                                if (moduleSelector.bools[m.first])
                                    m.second->Draw(&window);
                            }
                        break;
                }
                break;
        }
        for (auto& widget : Widget::widgets)
        {
            if (widget->active)
            {
                window.draw(widget->shape);
                window.draw(widget->text);
            }
        }

        window.display();
        auto updateEnd = std::chrono::steady_clock::now();
        std::chrono::duration<float> updateTimeUsed = updateEnd - updateStart;
        deltaTime = updateTimeUsed.count();
        framerate = 1/deltaTime;
        //std::cout << framerate << std::endl;
    }

    return 0;
}
