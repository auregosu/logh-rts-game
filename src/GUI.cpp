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

BuildGrid::BuildGrid(int _size)
{
	size = _size;
	int blockSize = 400/size;
	fullShape = new sf::ConvexShape(1); 
	for (int x = 0; x < size; x++)
	{
		for (int y = 0; y < size; y++)
		{
			float spacing = 2;
			coord coordinate = {x, y};
			sf::RectangleShape s(Vec2(0, 0));
			s.setPosition(Vec2(100+(blockSize*x)+x*spacing, 100+(blockSize*y)+y*spacing));
			s.setSize(Vec2(blockSize, blockSize));
			emptyColor = sf::Color(48, 60, 60);
			s.setFillColor(emptyColor);
			values.insert({coordinate, false});
			shapes.insert(std::make_pair(coordinate, s));
		}
	}
}

void BuildGrid::Click(Vec2 mouse)
{
	for (auto& block : shapes)
    {
        if (block.second.getGlobalBounds().contains(mouse))
        {
            if (!values[block.first])
            {
                TrueBlock(block.first);
               	trueing = true;
            } else
            {
                values[block.first] = false;
                block.second.setFillColor(emptyColor);
                trueing = false;
                blockCount-=1;
            }
        }
    }
}

void BuildGrid::ClickMove(Vec2 mouse)
{
	for (auto& block : shapes)
    {
        if (block.second.getGlobalBounds().contains(mouse))
        {
            if (!values[block.first] && trueing)
            {
            	TrueBlock(block.first);
            } else
            if (values[block.first] && !trueing)
            {
                values[block.first] = false;
                block.second.setFillColor(emptyColor);
                blockCount-=1;
            }
        }
    }
}

void BuildGrid::MakeShape()
{
	std::vector<sf::ConvexShape> convShapes;
	std::vector<Vec2> points, repeatedPoints;
	for (auto& block : values)
	{
		if (block.second)
		{
			sf::ConvexShape shape(4);
		    shape.setPoint(0, Vec2(0+block.first.x, 0+block.first.y));
		    shape.setPoint(1, Vec2(1+block.first.x, 0+block.first.y));
		    shape.setPoint(2, Vec2(1+block.first.x, 1+block.first.y));
		    shape.setPoint(3, Vec2(0+block.first.x, 1+block.first.y));
		    convShapes.push_back(shape);
		}
	}
	for (auto it = convShapes.begin(); it != convShapes.end(); it++)
	{
		for (auto it2 = convShapes.begin()+1; it2 != convShapes.end(); it2++)
		{
			if (it != it2)
			{
				//std::cout << "not same\n";
				for (int i = 0; i < 4; i++)
				{
					for (int j = 0; j < 4; j++)
					{
						if (((*it).getPoint(i).x == (*it2).getPoint(j).x &&
							(*it).getPoint(i).y == (*it2).getPoint(j).y) && (*it).getPoint(i).x != -1)
						{
							repeatedPoints.push_back((*it).getPoint(i));
							//std::cout << "(" << (*it).getPoint(i).x << ", " << (*it).getPoint(i).y << ") == ";
							//std::cout << "(" << (*it2).getPoint(j).x << ", " << (*it2).getPoint(j).y << ")\n";
							(*it).setPoint(i, Vec2(-1, -1));
							(*it2).setPoint(j, Vec2(-1, -1));
						}
					}
				}
			}
		}
	}
	for (auto &shape : convShapes)
	{
		for (auto &repPoint : repeatedPoints)
		{
			for (int i = 0; i < 4; i++)
			{
				if (shape.getPoint(i).x == repPoint.x &&
				shape.getPoint(i).y == repPoint.y && shape.getPoint(i).x != -1)
				{
					//std::cout << "(" << shape.getPoint(i).x << ", " << shape.getPoint(i).y << ") == ";
					//std::cout << "(" << repPoint.x << ", " << repPoint.y << ")\n";
					shape.setPoint(i, Vec2(-1, -1));
				}
			}
		}
		
	}
	float horizontal = 0;
	float vertical = 0;
	for (auto& shape : convShapes)
	{
		for (int i = 0; i < 4; i++)
		{
			if (shape.getPoint(i).x != -1)
			{
				horizontal += shape.getPoint(i).x;
				vertical += shape.getPoint(i).y;
				points.push_back(shape.getPoint(i));
			}
		}
	}
	int pointCount = points.size();
	std::map<float, Vec2> sortedPoints;
	Vec2 centroid = {horizontal/pointCount, vertical/pointCount};
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
		Vec2 vect = {centroid.x-point.x, centroid.y-point.y};
		float angle = atan2(vect.y, vect.x);
		if (angle < 0)
			angle += 2*M_PI;
		//std::cout << angle << std::endl;
		sortedPoints.insert(std::make_pair(angle, point));
	}
	dimensions = {maxX-minX, maxY-minY};
	sf::ConvexShape *returnShape = new sf::ConvexShape(pointCount);
	int i = 0;
	//std::cout << "gui.cpp\n";
	for (auto& point : sortedPoints)
	{
		returnShape->setPoint(i, point.second);
		//std::cout << "(" << point.second.x << ", " << point.second.y << ")\n";
		i++;
	}
	returnShape->setOrigin(centroid.x, centroid.y);
	fullShape = returnShape;
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
