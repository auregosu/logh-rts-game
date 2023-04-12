#include "ShipMaker.hpp"

void BuildGrid::Init(int _size, int _blockSize, int _offset)
{
	size = _size;
	offset = _offset;
	blockSize = _blockSize;
	float spacing = blockSize/size;
	fullShape = new sf::ConvexShape(1);
	blockPos = {
		offset,
		0.5f*(resolution->y-blockSize)
	};
	for (int x = 0; x < size; x++)
	{
		sf::VertexArray line(sf::Lines, 2);
		line[0].position = Vec2(blockPos.x, blockPos.y+x*spacing);
		line[1].position = Vec2(blockPos.x+blockSize, blockPos.y+x*spacing);
		lines.push_back(line);
	}
	for (int y = 0; y < size; y++)
	{
		sf::VertexArray line(sf::Lines, 2);
		line[0].position = Vec2(blockPos.x+y*spacing, blockPos.y);
		line[1].position = Vec2(blockPos.x+y*spacing, blockPos.y+blockSize);
		lines.push_back(line);
	}
	for (auto& line : lines)
	{
		line[0].color = Palette::Color1;
		line[1].color = Palette::Color1;
	}
	background.setPosition(blockPos);
	background.setSize(Vec2(blockSize, blockSize));
	emptyColor = sf::Color(48, 60, 60);
	background.setFillColor(emptyColor);
	tempShape.setFillColor(Palette::Highlight1);
	tempShape.setScale(Vec2(blockSize/size, blockSize/size));
	tempShape.setPosition(blockPos);
	startPoint.setFillColor(sf::Color::White);
	startPoint.setRadius(5);
	startPoint.setOrigin(5, 5);
	startPoint.setPointCount(10);
	startPoint.setPosition(-5, -5);
}

void BuildGrid::AddPoint(Vec2 mouse)
{
	Vec2 clicked = screenToCord(mouse);
	if (background.getGlobalBounds().contains(mouse))
	{
		bool isDifferent = true;
		bool isFirst = false;
		if (pointCount < size && !closed)
		{
			bool first = true;
			for (auto& point : points)
			{
				if (clicked.x == point.x && clicked.y == point.y)
				{
					isDifferent = false;
					if (first)
						isFirst = true;
					break;
				}
				first = false;
			}
			if (isDifferent)
			{
				points.push_back(clicked);
				pointCount++;
			} else if (isFirst && pointCount > 2)
			{
				closed = true;
				tempShape.setFillColor(Palette::Player1);
				startPoint.setFillColor(sf::Color::Transparent);
			}
		}
		if (pointCount == size && !closed)
		{
			closed = true;
			shapeBorder.push_back(sf::Vertex((coordToScreen(points[0])), sf::Color::White));
			tempShape.setFillColor(Palette::Player1);
			startPoint.setFillColor(sf::Color::Transparent);
		}
	}
}

void BuildGrid::RemovePoint(Vec2 mouse)
{
	if (background.getGlobalBounds().contains(mouse))
	{
		if (pointCount > 1 && !closed)
		{
			points.pop_back();
			shapeBorder.pop_back();
			pointCount--;
		} else if (closed || pointCount == 1)
			Reset();
	}
}

void BuildGrid::Reset()
{
	closed = false;
	points.clear();
	pointCount = 0;
	tempShape.setFillColor(Palette::Highlight1);
	startPoint.setFillColor(sf::Color::White);
	startPoint.setPosition(-5, -5);
	shapeBorder.clear();
}

void BuildGrid::UpdateShape(Vec2 mouse)
{
	tempShape.setPointCount(pointCount+1);
	shapeBorder.resize(pointCount+1);
	int i = 0;
	for (auto& point : points)
	{
		if (i == 0)
			startPoint.setPosition(coordToScreen(point));
		tempShape.setPoint(i, point);
		i++;
	}
	tempShape.setPoint(i, screenToCord(mouse));
	shapeBorder[i] = sf::Vertex(coordToScreen(screenToCord(mouse)), sf::Color::White);
	
}

void BuildGrid::MakeShape()
{
	float averageH = 0;
	float averageV = 0;
	for (auto& point : points)
	{
		averageH  += point.x;
		averageV += point.y;
	}
	Vec2 centroid = {averageH/pointCount, averageV/pointCount};
	Vec2 min = centroid, max = centroid;
	for (auto& point : points)
	{
		if (point.x > max.x)
			max.x = point.x;
		else if (point.x < min.x)
			min.x = point.x;
		if (point.y > max.y)
			max.y = point.y;
		else if (point.y < min.y)
			min.y = point.y;
	}
	dimensions = {max.x-min.x, max.y-min.y};
	fullShape->setPointCount(pointCount);
	int i = 0;
	for (auto& point : points) {
		fullShape->setPoint(i, point);
		if (point.x == min.x || point.y == min.y ||
			point.x == dimensions.x+min.x || point.y == dimensions.y+min.y) {
			// If NOT corner
			if ((point.x == min.x || point.x == dimensions.x+min.x) 
				&& point.y != min.y && point.y != dimensions.y+min.y)
				pointsOnBorder += 1;
			else if ((point.y == min.y || point.y == dimensions.y+min.y) 
				&& point.x != min.x && point.x != dimensions.x+min.x)
				pointsOnBorder += 1;
			// Else if corner
			else
				pointsOnBorder += 4;
		}
		i++;
	}
	fullShape->setOrigin(centroid.x, centroid.y);
	fullShape->setPosition(resolution->x*0.33f, resolution->y*0.5f);
	fullShape->setFillColor(Player::mainPlayer->color);
	fullShape->setRotation(0);
	float scale = (dimensions.x+dimensions.y)/2;
	fullShape->setScale(300/scale, 300/scale);
}

void ModuleSelector::Init(std::vector<Module::Type> types)
{
    availableTypes = types;

	for (auto& type : availableTypes)
	{
		if (type == Module::Type::MOTORX)
		{
			bools.insert(std::make_pair("SELECTOR_MOTORX", false));
		}
		if (type == Module::Type::MOTORY)
		{
			bools.insert(std::make_pair("SELECTOR_MOTORY", false));
		}
		if (type == Module::Type::CANNON)
		{
			bools.insert(std::make_pair("SELECTOR_CANNON", false));
		}
		
	}
}

void ModuleSelector::FitForShip(float shipScale, Vec2 centroid, Vec2 pos, Vec2 dimensions)
{
	for (auto& key : bools)
	{
		if (key.first == "SELECTOR_MOTORX")
		{
			Motor* motor = new Motor(true, 1, shipScale, dimensions);
			motor->shape1.setPosition(pos);
			motor->shape2.setPosition(pos);
			modules.insert(std::make_pair(key.first, motor));
		}
		else if (key.first == "SELECTOR_MOTORY")
		{
			Motor* motor = new Motor(false, 1, shipScale, dimensions);
			motor->shape1.setPosition(pos);
			motor->shape2.setPosition(pos);
			modules.insert(std::make_pair(key.first, motor));
		}
		else if (key.first == "SELECTOR_CANNON")
		{
			Cannon* cannon = new Cannon(shipScale, dimensions);
			cannon->shape.setPosition(pos);
			modules.insert(std::make_pair(key.first, cannon));
		}
	}
}

void ModuleSelector::MakeButtons()
{
	Vec2 buttonSize = *(resolution)*0.1f;
    for (auto& type : availableTypes) {
		if (type == Module::Type::MOTORX)
		{
			Button* b = new Button("SELECTOR_MOTORX", Button::SELECTOR_MODULE, 
			Vec2(resolution->x*0.85, resolution->y*0.5), buttonSize, Palette::Color2, "Motor-X");
			b->clickColor = Palette::Tech1;
			buttons.insert(std::make_pair("SELECTOR_MOTORX", b));
		}
		if (type == Module::Type::MOTORY)
		{
			Button* b = new Button("SELECTOR_MOTORY", Button::SELECTOR_MODULE, 
			Vec2(resolution->x*0.85, resolution->y*0.6), buttonSize, Palette::Color2, "Motor-Y");
			b->clickColor = Palette::Tech1;
			buttons.insert(std::make_pair("SELECTOR_MOTORY", b));
		}
		if (type == Module::Type::CANNON)
		{
			Button* b = new Button("SELECTOR_CANNON", Button::SELECTOR_MODULE, 
			Vec2(resolution->x*0.85, resolution->y*0.7), buttonSize, Palette::Color2, "Cannon");
			b->clickColor = Palette::Tech1;
			buttons.insert(std::make_pair("SELECTOR_CANNON", b));
		}
    }
}

void ShipMaker::Init(int size, int blockSize, int offset, std::vector<Module::Type> availableModules) {
	grid.resolution = resolution;
	selector.resolution = resolution;
    grid.Init(size, blockSize, offset);
    selector.Init(availableModules);
}