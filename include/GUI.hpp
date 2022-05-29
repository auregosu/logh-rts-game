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
	std::vector<Vec2> points;
	std::vector<sf::VertexArray> lines;
	sf::RectangleShape background;
	sf::ConvexShape* fullShape;
	sf::ConvexShape tempShape;
	sf::CircleShape startPoint;
	Vec2 dimensions;
	int size, blockSize, offset;
	int pointCount = 0;
	bool closed = false;
	sf::Color emptyColor;

	BuildGrid(int _size, int _blockSize, int _offset);
	inline Vec2 coordToScreen(Vec2 vC)
	{
		Vec2 vS = {
			vC.x*(blockSize/size)+offset,
			vC.y*(blockSize/size)+offset,
		};
		return vS;
	}
	inline Vec2 screenToCord(Vec2 vS)
	{
		Vec2 vC = {
		std::round((vS.x-offset)/(blockSize/size)),
		std::round((vS.y-offset)/(blockSize/size)),
		};
		return vC;
	}
	void AddPoint(Vec2 mouse);
	void RemovePoint(Vec2 mouse);
	void UpdateShape(Vec2 mouse);
	void Reset();
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
