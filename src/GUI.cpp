#include "GUI.hpp"

Widget::Widget()
{
	widgets.insert(this);
}
Widget::Widget(bool _active, Vec2 _pos, Vec2 size, sf::Color _color, sf::String txtString)
{
	widgets.insert(this);
	pos = _pos;
	active = _active;
	shape.setPosition(Vec2(pos.x-size.x/2, pos.y-size.y/2));
	shape.setSize(size);
	shape.setFillColor(_color);
	color = _color;
	if (!font.loadFromFile("Arimo-Bold.ttf"))
	    std::cout << "Failed loading the font.\n";
	textSize = {size.x*1.3, 1.3*size.x/txtString.getSize()};
	text.setPosition(Vec2(pos.x-textSize.x/3, pos.y-textSize.y/1.7));
	text.setString(txtString);
	text.setCharacterSize(textSize.y);
	text.setFillColor(sf::Color::White);
	text.setFont(font);
	text.setStyle(sf::Text::Bold);
}
void Widget::ChangeText(sf::String txtString)
{
	text.setString(txtString);
	textSize = Vec2(shape.getSize().x*1.3, 1.3*shape.getSize().x/txtString.getSize());
	text.setPosition(Vec2(pos.x-textSize.x/3, pos.y-textSize.y/1.7));
	text.setCharacterSize(textSize.y);
	
}

Button::Button(bool _active, Vec2 pos, Vec2 size, sf::Color _color, sf::String txtString)
{
	value = 0;
	active = _active;
	shape.setPosition(Vec2(pos.x-size.x/2, pos.y-size.y/2));
	shape.setSize(size);
	shape.setFillColor(_color);
	color = _color;
	clickColor = Palette::Highlight1;
	backColor = color;
	if (!font.loadFromFile("Arimo-Bold.ttf"))
	    std::cout << "Failed loading the font.\n";
	Vec2 textSize = {size.x*1.3, 1.3*size.x/txtString.getSize()};
	text.setPosition(Vec2(pos.x-textSize.x/3, pos.y-textSize.y/1.7));
	text.setString(txtString);
	text.setCharacterSize(textSize.y);
	text.setFillColor(sf::Color::White);
	text.setFont(font);
	text.setStyle(sf::Text::Bold);
}

BuildGrid::BuildGrid(int _size, int _blockSize, int _offset)
{
	size = _size;
	offset = _offset;
	blockSize = _blockSize;
	int spacing = blockSize/size;
	fullShape = new sf::ConvexShape(1);
	for (int x = 0; x < size; x++)
	{
		sf::VertexArray line(sf::Lines, 2);
		line[0].position = Vec2(offset, offset+x*spacing);
		line[1].position = Vec2(offset+blockSize, offset+x*spacing);
		lines.push_back(line);
	}
	for (int y = 0; y < size; y++)
	{
		sf::VertexArray line(sf::Lines, 2);
		line[0].position = Vec2(offset+y*spacing, offset);
		line[1].position = Vec2(offset+y*spacing, offset+blockSize);
		lines.push_back(line);
	}
	for (auto& line : lines)
	{
		line[0].color = Palette::Color1;
		line[1].color = Palette::Color1;
	}
	background.setPosition(Vec2(offset, offset));
	background.setSize(Vec2(blockSize, blockSize));
	emptyColor = sf::Color(48, 60, 60);
	background.setFillColor(emptyColor);
	tempShape.setFillColor(Palette::Highlight1);
	tempShape.setScale(Vec2(blockSize/size, blockSize/size));
	tempShape.setPosition(100, 100);
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
			int first = true;
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
				points.push_back(Vec2(clicked.x, clicked.y));
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
			startPoint.setFillColor(sf::Color::Transparent);
			tempShape.setFillColor(Palette::Player1);
		}
	}
}

void BuildGrid::RemovePoint(Vec2 mouse)
{
	if (background.getGlobalBounds().contains(mouse))
	{
		if (pointCount > 0 && !closed)
		{
			points.pop_back();
			pointCount--;
		} else if (closed)
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
}

void BuildGrid::UpdateShape(Vec2 mouse)
{
	tempShape.setPointCount(pointCount+1);
	int i = 0;
	for (auto& point : points)
	{
		if (i == 0)
			startPoint.setPosition(coordToScreen(points[i]));
		tempShape.setPoint(i, point);
		i++;
	}
	tempShape.setPoint(i, screenToCord(mouse));
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
	float minX = centroid.x, maxX = centroid.x;
	float minY = centroid.y, maxY = centroid.y;
	for (auto& point : points)
	{
		if (point.x > maxX)
			maxX = point.x;
		else if (point.x < minX)
			minX = point.x;
		if (point.y > maxY)
			maxY = point.y;
		else if (point.y < minY)
			minY = point.y;
	}
	dimensions = {maxX-minX, maxY-minY};
	fullShape->setPointCount(pointCount);
	int i = 0;
	for (auto& point : points)
	{
		fullShape->setPoint(i, point);
		i++;
	}
	fullShape->setOrigin(centroid.x, centroid.y);
	fullShape->setPosition(300, 300);
	fullShape->setFillColor(Player::mainPlayer->color);
	fullShape->setRotation(0);
	float scale = (dimensions.x+dimensions.y)/2;
	fullShape->setScale(300/scale, 300/scale);
}
ModuleSelector::ModuleSelector(std::vector<Module::Type> types)
{
	for (auto& type : types)
	{
		if (type == Module::Type::MOTORX)
		{
			Button* b = new Button(false, Vec2(SCREENWIDTH*0.8, SCREENHEIGHT*0.5), Vec2(100, 62), Palette::Color2, "Motor-X");
			b->clickColor = Palette::Tech1;
			buttons.insert(std::make_pair("Motor-X", b));
			bools.insert(std::make_pair("Motor-X", false));
			good2go.insert(std::make_pair("Motor-X", true));
		}
		if (type == Module::Type::MOTORY)
		{
			Button* b = new Button(false, Vec2(SCREENWIDTH*0.8, SCREENHEIGHT*0.6), Vec2(100, 62), Palette::Color2, "Motor-Y");
			b->clickColor = Palette::Tech1;
			buttons.insert(std::make_pair("Motor-Y", b));
			bools.insert(std::make_pair("Motor-Y", false));
			good2go.insert(std::make_pair("Motor-Y", true));
		}
		if (type == Module::Type::CANNON)
		{
			Button* b = new Button(false, Vec2(SCREENWIDTH*0.8, SCREENHEIGHT*0.7), Vec2(100, 62), Palette::Color2, "Cannon");
			b->clickColor = Palette::Tech1;
			buttons.insert(std::make_pair("Cannon", b));
			bools.insert(std::make_pair("Cannon", false));
			good2go.insert(std::make_pair("Cannon", true));
		}
		
	}
}
void ModuleSelector::FitForShip(float shipScale, Vec2 centroid, Vec2 pos, Vec2 dimensions)
{
	for (auto& key : buttons)
	{
		if (key.first == "Motor-X")
		{
			Motor* motor = new Motor(true, 1, shipScale, dimensions);
			motor->shape1.setPosition(pos);
			motor->shape2.setPosition(pos);
			modules.insert(std::make_pair(key.first, motor));
		}
		else if (key.first == "Motor-Y")
		{
			Motor* motor = new Motor(false, 1, shipScale, dimensions);
			motor->shape1.setPosition(pos);
			motor->shape2.setPosition(pos);
			modules.insert(std::make_pair(key.first, motor));
		}
		else if (key.first == "Cannon")
		{
			Cannon* cannon = new Cannon(shipScale, dimensions);
			cannon->shape.setPosition(pos);
			modules.insert(std::make_pair(key.first, cannon));
		}
	}
}
void ModuleSelector::ActivateButtons()
{
	for (auto& button : buttons)
	{
		button.second->active = true;
	}
}
void ModuleSelector::DeactivateButtons()
{
	for (auto& button : buttons)
	{
		button.second->active = false;
	}
}

std::unordered_set<Widget*> Widget::widgets;
