#ifndef SHIP_MAKER_HPP
#define SHIP_MAKER_HPP
#include "GUI.hpp"

struct BuildGrid
{
	std::vector<Vec2> points;
	std::vector<sf::VertexArray> lines;
	sf::RectangleShape background;
	sf::ConvexShape *fullShape;
	sf::ConvexShape tempShape;
	sf::CircleShape startPoint;
	std::vector<sf::Vertex> shapeBorder;
	Vec2 dimensions, blockPos;
	Vec2 *resolution;
	int size, blockSize, offset;
	int pointCount = 0, pointsOnBorder = 0;
	bool closed = false;
	sf::Color emptyColor;

	void Init(int _size, int _blockSize, int _offset);

	inline Vec2 coordToScreen(Vec2 vC)
	{
		return {
			vC.x * (blockSize / size) + blockPos.x,
			vC.y * (blockSize / size) + blockPos.y};
	}
	inline Vec2 screenToCord(Vec2 vS)
	{
		return {
			std::round((vS.x - blockPos.x) / (blockSize / size)),
			std::round((vS.y - blockPos.y) / (blockSize / size))};
	}
	void AddPoint(Vec2 mouse);
	void RemovePoint(Vec2 mouse);
	void UpdateShape(Vec2 mouse);
	void Reset();
	void MakeShape();
};

struct ModuleSelector
{
	std::map<std::string, Button *> buttons;
	std::map<std::string, Module *> modules;
	std::map<std::string, bool> bools;
	std::vector<Module::Type> availableTypes;
	Vec2 *resolution;
	int points;
	float shapesInPlace;

	void Init(std::vector<Module::Type>);
	void FitForShip(float, Vec2, Vec2, Vec2);
	void MakeButtons();
};

struct ShipMaker
{
	BuildGrid grid;
	ModuleSelector selector;
	Vec2 *resolution;

	void Init(int, int, int, std::vector<Module::Type>);
};

#endif