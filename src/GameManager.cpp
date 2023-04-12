#include "GameManager.hpp"

void GameManager::Input() {
    sf::Event event;
    while (window->pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            window->close();
        switch (gameState)
        {
            case GAME:
                switch (event.type)
                {
                    case sf::Event::MouseButtonPressed:
// Mouse press.
                        if (event.mouseButton.button == sf::Mouse::Left)
                        {
                            if (Player::mainPlayer->action == Player::NONE)
                            {
                                bool wasShipClicked = Ship::ClickSelect(Player::mainPlayer, controller->mousePos, sf::Keyboard::isKeyPressed(sf::Keyboard::LShift));
                                bool wasSettleClicked = Settlement::ClickSelect(Player::mainPlayer, controller->mousePos, sf::Keyboard::isKeyPressed(sf::Keyboard::LShift));
                                if (!wasShipClicked && !wasSettleClicked)
                                {
                                    controller->pivot1 = sf::Vector2f(controller->mousePos);
                                    Player::mainPlayer->selectionSquare.setPosition(controller->pivot1);
                                    Player::mainPlayer->action = Player::SELECTING_SQUARE;
                                } else if (wasSettleClicked)
                                {
                                    for (auto& settle : Settlement::selectedSettlements)
                                    {
                                        guiDisplay->MakeLine("SELECTED_SETTLEMENT", settle->pos, controller->mousePos);
                                    }
                                    Player::mainPlayer->action = Player::CHANGING_SUPPLIES;
                                }
                            } else if (Player::mainPlayer->action == Player::CHANGING_SUPPLIES)
                            {
                                Ship* shipClicked = Ship::Click(controller->mousePos);
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
                            if (Player::mainPlayer->action == Player::NONE)
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
                                    Entity* touchedEntity = Entity::Click(controller->mousePos);
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
                                            controller->pivot1 = sf::Vector2f(controller->mousePos);
                                            Player::mainPlayer->prevShipRotation = ship->shape.getRotation();
                                            Vec2 ghostPos;
                                            if (Ship::selectedShips.size() == 1)
                                                ghostPos = controller->pivot1;
                                            else
                                            {
                                                ghostPos = {
                                                controller->pivot1.x+ship->pos.x-Player::mainPlayer->shipAveragePos.x,
                                                controller->pivot1.y+ship->pos.y-Player::mainPlayer->shipAveragePos.y};
                                            }
                                            ship->ghost.setPosition(ghostPos);
                                            ship->prevGhostPos = ghostPos;
                                            //mouseLine[0].position = controller->pivot1;
                                            guiDisplay->MakeLine("MOVE_SHIP", controller->pivot1, controller->mousePos);
                                            Player::mainPlayer->action = Player::COMMAND_SHIP_MOVEMENT_START;
                                        }
                                    }
                                }
                            } else if (Player::mainPlayer->action == Player::CHANGING_SUPPLIES)
                            {
                                Ship* shipClicked = Ship::Click(controller->mousePos);
                                if (shipClicked != nullptr)
                                {
                                    for (auto& settle : Settlement::selectedSettlements)
                                    {
                                        settle->Remove(shipClicked);
                                    }
                                } else
                                {
                                    Settlement::ClearSelect();
                                    Player::mainPlayer->action = Player::NONE;
                                    guiDisplay->EraseLine("SELECTED_SETTLEMENT");
                                }
                            }
                            
                        }
                        break;
                    case sf::Event::MouseButtonReleased:
// Mouse release.           
                        if (event.mouseButton.button == sf::Mouse::Right)
                        {
                            if (Player::mainPlayer->action == Player::COMMAND_SHIP_MOVEMENT_START ||
                                Player::mainPlayer->action == Player::COMMAND_SHIP_MOVEMENT_ROTATE)
                            {
                                for (auto& ship : Ship::selectedShips)
                                {
                                    ship->Move(ship->ghost.getPosition(), ship->ghost.getRotation(), Ship::Click(controller->pivot1), Ship::shipScale*5);
                                }
                                Player::mainPlayer->action = Player::NONE;
                                guiDisplay->EraseLine("MOVE_SHIP");
                            }
                        } else if (event.mouseButton.button == sf::Mouse::Left)
                        {
                            if (Player::mainPlayer->action == Player::SELECTING_SQUARE)
                            {
                                Ship::SquareSelect(Player::mainPlayer, sf::Keyboard::isKeyPressed(sf::Keyboard::LShift));
                                Player::mainPlayer->selectionSquare.setSize({0, 0});
                                Player::mainPlayer->action = Player::NONE;
                            }
                        }
                        break;
                    case sf::Event::MouseMoved:
// Mouse move.
                        if (Player::mainPlayer->action == Player::CHANGING_SUPPLIES)
                        {
                            //mouseLine[1].position = sf::Vector2f(controller->mousePos);
                            guiDisplay->UpdateLine2("SELECTED_SETTLEMENT",controller->mousePos);
                            if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
                            {
                                Ship* shipClicked = Ship::Click(controller->mousePos);
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
                            if (Player::mainPlayer->action == Player::COMMAND_SHIP_MOVEMENT_START)
                                Player::mainPlayer->action = Player::COMMAND_SHIP_MOVEMENT_ROTATE;
                            for (auto& ship : Ship::selectedShips)
                            {
                                if (Player::mainPlayer->action == Player::COMMAND_SHIP_MOVEMENT_ROTATE)
                                {
                                    if (Ship::selectedShips.size() == 1)
                                    {
                                        //mouseLine[1].position = ship->pos;
                                        guiDisplay->UpdateLine2("MOVE_SHIP", ship->pos);
                                        float difX = ship->ghost.getPosition().x-controller->mousePos.x;
                                        float difY = ship->ghost.getPosition().y-controller->mousePos.y;
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
                                        //mouseLine[1].position = Player::mainPlayer->shipAveragePos;
                                        guiDisplay->UpdateLine2("MOVE_SHIP", Player::mainPlayer->shipAveragePos);
                                        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                                        {
                                            float difX = ship->ghost.getPosition().x-controller->mousePos.x;
                                            float difY = ship->ghost.getPosition().y-controller->mousePos.y;
                                            float distance = sqrtf(difX*difX+difY*difY);
                                            float nX = difX/distance;
                                            float nY = difY/distance;
                                            float angle = (atan2(nX, nY)*180/M_PI);
                                            ship->ghost.setRotation(-angle);
                                        } else
                                        {
                                            float difX = controller->pivot1.x-ship->prevGhostPos.x;
                                            float difY = controller->pivot1.y-ship->prevGhostPos.y;
                                            float distance = sqrtf(difX*difX+difY*difY);
                                            float nX = difX/distance;
                                            float nY = difY/distance;
                                            float prevAngle = atan2(nX, nY);
                                            float difX2 = controller->pivot1.x-controller->mousePos.x;
                                            float difY2 = controller->pivot1.y-controller->mousePos.y;
                                            float distance2 = sqrtf(difX2*difX2+difY2*difY2);
                                            float nX2 = difX2/distance2;
                                            float nY2 = difY2/distance2;
                                            float angle = atan2(nX2, nY2);
                                            Vec2 newPos;
                                            newPos = {controller->pivot1.x+cos(-angle+prevAngle+M_PI/2)*distance,
                                                    controller->pivot1.y+sin(-angle+prevAngle+M_PI/2)*distance,
                                            };
                                            ship->ghost.setPosition(newPos);
                                            ship->ghost.setRotation(-Player::mainPlayer->prevShipRotation-(angle*180/M_PI));
                                        }
                                    }
                                }
                            }
                        } else if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
                        {
                            if (Player::mainPlayer->action == Player::SELECTING_SQUARE)
                            {
                                Vec2 selectionSize = {
                                    float(controller->mousePos.x)-controller->pivot1.x, 
                                    float(controller->mousePos.y)-controller->pivot1.y};
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
                            case sf::Keyboard::S:
                                for (auto& ship : Ship::selectedShips)
                                    ship->Stop();
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
                            for (auto& button : Button::buttons)
                            {
                                if (button.second->shape.getGlobalBounds().contains(controller->mousePos))
                                {
                                    button.second->shape.setFillColor(button.second->clickColor);
                                    controller->clickedButton = button.second; 
                                }
                            }
                            switch (menuScreen)
                            {
                                case SHAPEMAKE:
                                    shipMaker->grid.AddPoint(controller->mousePos);
                                    Widget::widgets["MODULE_POINTS"]->ChangeText(sf::String(std::to_string(shipMaker->grid.pointCount)+"/"+std::to_string(shipMaker->grid.size)));
                                    break;
                                case BUILD:
                                    for (auto& b : shipMaker->selector.bools)
                                    {
                                        if (b.second)
                                        {
                                            
                                            if (shipMaker->selector.modules[b.first]->GetShape1()->getGlobalBounds().contains(controller->mousePos)) {
                                                controller->pivot1 = controller->mousePos-shipMaker->selector.modules[b.first]->GetShape1()->getPosition();
                                                Player::mainPlayer->action = Player::MOVING_MODULE;
                                            }
                                            if (shipMaker->selector.modules[b.first]->GetShape2()->getGlobalBounds().contains(controller->mousePos)) {
                                                controller->pivot1 = controller->mousePos-shipMaker->selector.modules[b.first]->GetShape2()->getPosition();
                                                Player::mainPlayer->action = Player::MOVING_MODULE;
                                            }
                                        }
                                    }
                                    break;
                            }       
                        }
                        else // sf::Mouse::Right
                        {
                            switch (menuScreen)
                            {
                                case SHAPEMAKE:
                                    shipMaker->grid.RemovePoint(controller->mousePos);
                                    Widget::widgets["MODULE_POINTS"]->ChangeText(sf::String(std::to_string(shipMaker->grid.pointCount)+"/"+std::to_string(shipMaker->grid.size)));
                                    break;
                                default:
                                    break;
                            }
                        }
                        break;
                    case sf::Event::MouseButtonReleased:
                        if (event.mouseButton.button == sf::Mouse::Left)
                        {
                            if (controller->clickedButton) {
                                controller->clickedButton->shape.setFillColor(controller->clickedButton->color);
                                ButtonAction(controller->clickedButton, this);
                                controller->clickedButton = nullptr;
                            }

                            if (Player::mainPlayer->action == Player::MOVING_MODULE) {
                                controller->pivot1 = controller->mousePos;
                                Player::mainPlayer->action = Player::NONE;
                            }   
                        }
                        break;
                    case sf::Event::MouseMoved:
                        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
                        {
                            switch (menuScreen)
                            {
                                case SHAPEMAKE:
                                    break;
                                case BUILD:
                                    if (Player::mainPlayer->action == Player::MOVING_MODULE) {
                                        for (auto& b : shipMaker->selector.bools) {
                                            if (b.second) {
                                                if (shipMaker->selector.modules[b.first]->GetShape1()->getGlobalBounds().contains(controller->mousePos))
                                                    shipMaker->selector.modules[b.first]->GetShape1()->setPosition(controller->mousePos-controller->pivot1);
                                                if (shipMaker->selector.modules[b.first]->GetShape2()->getGlobalBounds().contains(controller->mousePos))
                                                    shipMaker->selector.modules[b.first]->GetShape2()->setPosition(controller->mousePos-controller->pivot1);
                                            }
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
}

void GameManager::Logic() {
    switch (gameState)
        {
            case GAME:
                if (controller->pixelPos.y < 30)
                    view.move(0, -camSpeed);
                else if (controller->pixelPos.y > (int)window->getSize().y-30)
                    view.move(0, camSpeed);
                if (controller->pixelPos.x < 30)
                    view.move(-camSpeed, 0);
                else if (controller->pixelPos.x > (int)window->getSize().x-30)
                    view.move(camSpeed, 0);
                break;
            case MENU:
                switch (menuScreen)
                {
                    case SHAPEMAKE:
                        if (!shipMaker->grid.closed)
                            shipMaker->grid.UpdateShape(controller->mousePos);
                        break;
                    case BUILD:
                        for (auto& b : shipMaker->selector.bools)
                        {
                            if (b.second)
                            {
                                int allShapes = 0;
                                //if (false)
                                if (Collisions::areOverlapping(ShapeToVector(*(shipMaker->grid.fullShape)), ShapeToVector(*(shipMaker->selector.modules[b.first]->GetShape1()))))
                                {
                                    shipMaker->selector.modules[b.first]->GetShape1()->setFillColor(Palette::Tech1);
                                    allShapes += 1;
                                }
                                else
                                {
                                    shipMaker->selector.modules[b.first]->GetShape1()->setFillColor(Palette::Highlight2);
                                }
                                if (Collisions::areOverlapping(ShapeToVector(*(shipMaker->grid.fullShape)), ShapeToVector(*(shipMaker->selector.modules[b.first]->GetShape2()))))
                                {
                                    shipMaker->selector.modules[b.first]->GetShape2()->setFillColor(Palette::Tech1);
                                    allShapes += 1;
                                }
                                else
                                {
                                    shipMaker->selector.modules[b.first]->GetShape2()->setFillColor(Palette::Highlight2);
                                }
                                if (allShapes > 1)
                                    shipMaker->selector.shapesInPlace = true;
                                else
                                    shipMaker->selector.shapesInPlace = false;
                            }
                        }
                        break;
                }
                break;
        }
        Ship::Update();
        Settlement::Update();
        Supply::Update();
        Entity::KillDeadEntities();
}

void GameManager::Display() {
    window->setView(view);
    window->clear(Palette::Color1);

    for (auto& e : Settlement::settlements)
        e->Draw(window, gameState);
    for (auto& e : Supply::supplies)
        e->Draw(window, gameState);
    for (auto& e : Ship::ships)
        e->Draw(window, gameState);

    switch (gameState)
    {
        case GAME:
        if (Player::mainPlayer->action == Player::SELECTING_SQUARE)
                window->draw(Player::mainPlayer->selectionSquare);
            break;
        case MENU:
            switch(menuScreen)
            {
                case SHAPEMAKE:
                    window->draw(shipMaker->grid.background);
                    for (auto& l : shipMaker->grid.lines)
                        window->draw(l);
                    window->draw(shipMaker->grid.tempShape);
                    window->draw(shipMaker->grid.startPoint);
                    window->draw(&(shipMaker->grid.shapeBorder)[0], shipMaker->grid.shapeBorder.size(), sf::LineStrip);
                    break;
                case BUILD:
                        window->draw(*(shipMaker->grid.fullShape));
                        for (auto& m : shipMaker->selector.modules)
                        {
                            if (shipMaker->selector.bools[m.first])
                                m.second->Draw(window);
                        }
                    break;
            }
            break;
    }
    for (auto& widget : Widget::widgets)
    {
        window->draw(widget.second->shape);
        window->draw(widget.second->text);
    }

    for (auto& button : Button::buttons)
    {
        window->draw(button.second->shape);
        window->draw(button.second->text);
    }

    for (auto& line : guiDisplay->lines)
    {
        window->draw(&(line.second)[0], 2, sf::Lines);
    }

    window->display();
}

void GameManager::Init(float idealFramerate, std::vector<Module::Type> availableModules)  {
    // Window
    window->setFramerateLimit(int(idealFramerate));
    view = window->getDefaultView();
    window->setView(view);
    resolution = Vec2(window->getSize());

    // Shape maker
    shipMaker->resolution = &resolution;
    shipMaker->Init(8, resolution.x*0.4f, resolution.y*0.15f, availableModules);
}
