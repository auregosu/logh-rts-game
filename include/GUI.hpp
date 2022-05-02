#ifndef GUI_HPP
#define GUI_HPP
#include "Ship.hpp"

class Widget
{
public:
	static std::unordered_set<Widget*> widgets;
	sf::RectangleShape shape;
	sf::Color color;
	sf::Text text;
	sf::Font font;
	Vec2 pos, textSize;
	bool active = false;

	Widget();
	Widget(bool _active, Vec2 pos, Vec2 size, sf::Color _color, sf::String txtString);
	void ChangeText(sf::String txtString);
};

class Button : public Widget
{
public:
	sf::Color clickColor = sf::Color::Cyan;
	sf::Color backColor = sf::Color::Cyan;
	int value = -1;
	Button(bool _active, Vec2 pos, Vec2 size, sf::Color _color, sf::String txtString);
};

class BuildGrid
{
public:
	std::map<coord, bool> values;
	std::map<coord, sf::RectangleShape> shapes;
	sf::ConvexShape* fullShape;
	Vec2 dimensions;
	int size;
	int blockCount = 0;
	bool trueing = true;
	sf::Color emptyColor;

	BuildGrid(int _size);
	inline void TrueBlock(coord blockFirst)
	{
		if (blockCount < size)
		{
	    	values[blockFirst] = true;
	    	shapes[blockFirst].setFillColor(Player::mainPlayer->color);
            blockCount+=1;
      	}
	}
	void Click(Vec2 mouse);
	void ClickMove(Vec2 mouse);
	void MakeShape();
};

class ModuleSelector
{
public:
	std::map<std::string, Button*> buttons;
	std::map<std::string, Module*> modules;
	std::map<std::string, bool> bools;
	std::map<std::string, bool> good2go;
	int points;

	ModuleSelector(std::vector<Module::Type> strings);
	void FitForShip(float shipScale, Vec2 centroid, Vec2 pos, Vec2 dimensions);
	void ActivateButtons();
	void DeactivateButtons();
};

#endif