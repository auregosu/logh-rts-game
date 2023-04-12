#include "Timestep.hpp"
#include "GameManager.hpp"

using namespace std::literals::chrono_literals;

int main()
{
    GameManager game;

    sf::RenderWindow window(sf::VideoMode(1618*0.75, 1000*0.75), "Pizza");
    Palette::Initialize();
    InputController controller;
    GUI guiDisplay;
    ShipMaker shipMaker;

    game.window = &window;
    game.controller = &controller;
    game.guiDisplay = &guiDisplay;
    game.shipMaker = &shipMaker;

    std::vector<Module::Type> availableModules;
    availableModules.push_back(Module::Type::MOTORX);
    availableModules.push_back(Module::Type::MOTORY);
    availableModules.push_back(Module::Type::CANNON);

    game.Init(idealFramerate, availableModules);

    Widget *modulePoints = new Widget("MODULE_POINTS",
        Vec2(game.resolution.x * 0.85f, game.resolution.y * 0.2f), game.resolution * 0.1f,
        Palette::Color2, "0/" + std::to_string(shipMaker.grid.size));

    Button *makeShape = new Button("MAKE_SHAPE", Button::MAKE_SHAPE,
        game.resolution*0.85f, game.resolution * 0.2f,
        Palette::Player1, "DONE");

/*
    ShipShape onebytwo;
    onebytwo.setPointCount(4);
    onebytwo.setPoint(0, Vec2(0, 0));
    onebytwo.setPoint(1, Vec2(0, -2));
    onebytwo.setPoint(2, Vec2(1, -2));
    onebytwo.setPoint(3, Vec2(1, 0));
    onebytwo.setOrigin(0.5, -1);
    Ship::shipShapes.insert(std::make_pair("1x2", onebytwo));
*/
    Player *player1 = new Player("aure", Palette::Player1);
    Player *player2 = new Player("juan", Palette::Player2);
    Player::mainPlayer = player1;
/*
    for (int i = 0; i <= 8; i += 1)
    {
        Ship *ship = new Ship(player2, Vec2(i * 100, 0), "1x2");
        ship->shape.rotate(180);
    }
*/
    Settlement *settle = new Settlement({game.resolution.x * 0.5, 900}, Player::mainPlayer);
    Settlement *settle2 = new Settlement({game.resolution.x * 0.5, -300}, player2);

    while (window.isOpen())
    {
        auto updateStart = std::chrono::steady_clock::now();

        controller.pixelPos = sf::Mouse::getPosition(window);
        controller.mousePos = window.mapPixelToCoords(controller.pixelPos);

        game.Input();

        game.Logic();

        game.Display();

        auto updateEnd = std::chrono::steady_clock::now();
        std::chrono::duration<float> updateTimeUsed = updateEnd - updateStart;
        deltaTime = updateTimeUsed.count();
        framerate = 1 / deltaTime;
        // std::cout << framerate << std::endl;
    }

    return 0;
}
