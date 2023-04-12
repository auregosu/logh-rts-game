#include "GameManager.hpp"

void ButtonAction(Button* button, GameManager* game) {
    if (button == nullptr)
        return;

    switch (button->action)
    {
        case Button::MAKE_SHAPE:
            button->shape.setFillColor(button->color);
            if (game->shipMaker->grid.pointCount > 2) {
                button->value = 0;
                game->shipMaker->grid.MakeShape();
                // esoteric calculations
                game->shipMaker->selector.points = std::floor(sqrt(2*game->shipMaker->grid.pointCount))-1+std::floor(sqrt(0.5f*game->shipMaker->grid.dimensions.x*game->shipMaker->grid.dimensions.y));
                game->shipMaker->selector.FitForShip(game->shipMaker->grid.fullShape->getScale().x, 
                    game->shipMaker->grid.fullShape->getOrigin(), game->shipMaker->grid.fullShape->getPosition(), 
                    game->shipMaker->grid.dimensions);
                Ship::shipShapes.insert(std::make_pair("custom", *(game->shipMaker->grid.fullShape)));
                game->shipMaker->selector.MakeButtons();
                Widget::widgets["MODULE_POINTS"]->ChangeText(sf::String(std::to_string(game->shipMaker->selector.points)));
                game->menuScreen = MenuScreens::BUILD;
                game->guiDisplay->Erase(button);
                Button* buildShip = new Button("BUILD_SHIP", Button::BUILD_SHIP, 
                    game->resolution*0.85f, game->resolution*0.2f, 
                    Player::mainPlayer->color, "BUILD");

            } else {
                button->value = 0;
            }
            break;

        case Button::BUILD_SHIP:
        {
            if (game->shipMaker->selector.points < game->shipMaker->grid.pointCount && game->shipMaker->selector.shapesInPlace)
            {
                game->guiDisplay->Erase(Widget::widgets["MODULE_POINTS"]);
                for (auto& button : game->shipMaker->selector.buttons)
                {
                    game->guiDisplay->Erase(button.second);
                }
                Ship blueprintShip;
                for (auto& m : game->shipMaker->selector.modules)
                {
                    if (game->shipMaker->selector.bools[m.first])
                    {
                        Vec2 scale = game->shipMaker->grid.fullShape->getScale();
                        Vec2 shapePos = game->shipMaker->grid.fullShape->getPosition();
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
                game->shipMaker->grid.fullShape->setScale(1, 1);
                float shipVolume = game->shipMaker->grid.dimensions.x*game->shipMaker->grid.dimensions.y;
                std::cout << "pointsOnBorder: " << game->shipMaker->grid.pointsOnBorder << std::endl;
                blueprintShip.mass = 0.125*shipVolume/(float(game->shipMaker->grid.pointCount)/float(game->shipMaker->grid.pointsOnBorder*2));
                //blueprintShip.mass = 0.25*game->shipMaker->grid.pointCount+0.75*game->shipMaker->grid.dimensions.x*game->shipMaker->grid.dimensions.y;
                std::cout << blueprintShip.mass << std::endl;
                blueprintShip.localShape = &(Ship::shipShapes["custom"]);
                for (int i = 0; i < 5; i++)
                {
                    Ship *ship = new Ship(Player::mainPlayer, Vec2(i*200, 500), blueprintShip);
                }
                blueprintShip.Kill();
                std::cout << "killed" << std::endl;
                game->gameState = GAME;
                game->guiDisplay->Erase(button);
            } else
            {
                button->shape.setFillColor(button->color);
                button->value = 0;
            }
            break;
        }
        case Button::SELECTOR_MODULE:
            if (button->shape.getGlobalBounds().contains(game->controller->mousePos))
            {
                if (game->shipMaker->selector.points >= 0)
                {
                    if (game->shipMaker->selector.bools[button->name]) {
                        button->shape.setFillColor(button->color);
                        game->shipMaker->selector.bools[button->name] = false;
                        game->shipMaker->selector.points += 1;
                    } else if (game->shipMaker->selector.points > 0){
                        button->shape.setFillColor(button->clickColor);
                        game->shipMaker->selector.bools[button->name] = true;
                        game->shipMaker->selector.points -= 1;
                    }
                }
                Widget::widgets["MODULE_POINTS"]->ChangeText(sf::String(std::to_string(game->shipMaker->selector.points)));
            }
            break;
    }
}
