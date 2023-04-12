#ifndef GAME_MANAGER_HPP
#define GAME_MANAGER_HPP
#include "ShipMaker.hpp"

class InputController {
public:
    sf::Vector2i pixelPos;
    sf::Vector2f mousePos;

    Vec2 pivot1, pivot2;

    Button* clickedButton = nullptr;

    InputController() {
    }
};

struct GameManager {
    sf::RenderWindow* window;
    InputController* controller;
    GUI* guiDisplay;
    ShipMaker* shipMaker;
    
    sf::View view;
    MenuScreens menuScreen = SHAPEMAKE;
    GameStates gameState = MENU;
    Vec2 resolution;
    float camSpeed = 8;

    void Init(float, std::vector<Module::Type>);

    void Input();
    void Logic();
    void Display();

private:
    void InputGame(sf::Event);
    void InputMenu(sf::Event);

    void LogicGame();
    void LogicMenu();

    void DisplayGame();
    void DisplayMenu();

};

void ButtonAction(Button*, GameManager*);

#endif