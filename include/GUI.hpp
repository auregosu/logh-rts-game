#ifndef GUI_HPP
#define GUI_HPP
#include "Ship.hpp"

class Widget
{
public:
	static std::map<std::string, Widget*> widgets;
	sf::RectangleShape shape;
	sf::Color color;
	sf::Text text;
	sf::Font font;
	Vec2 pos, textSize;
	std::string name;

	inline Widget() {}
	Widget(std::string, Vec2, Vec2, sf::Color, sf::String);

	void ChangeText(sf::String txtString);
};

class Button : public Widget
{
public:
	static std::map<std::string, Button*> buttons;
	sf::Color clickColor = sf::Color::Cyan;
	sf::Color backColor = sf::Color::Cyan;
	std::string name = "NONE";
	int value = -1;
	enum Action {
		MAKE_SHAPE,
		BUILD_SHIP,
		SELECTOR_MODULE
	} action;

	Button(std::string, Action, Vec2, Vec2, sf::Color, sf::String);
};

struct GUI {
	std::map<std::string, std::array<sf::Vertex, 2>> lines;

	void MakeLine(std::string, Vec2, Vec2);
	void UpdateLine(std::string, Vec2, Vec2);
	void UpdateLine1(std::string, Vec2);
	void UpdateLine2(std::string, Vec2);

	void EraseLine(std::string name);
	void Erase(Widget*);
	void Erase(Button*);

};
#endif
