#include "GUI.hpp"

Widget::Widget(std::string _name, Vec2 _pos, Vec2 size, sf::Color _color, sf::String txtString) {
	name = _name;
	widgets.insert(std::make_pair(name, this));
	pos = _pos;
	shape.setPosition(Vec2(pos.x-size.x/2, pos.y-size.y/2));
	shape.setSize(size);
	shape.setFillColor(_color);
	color = _color;
	if (!font.loadFromFile("Arimo-Bold.ttf"))
	    std::cout << "Failed loading the font.\n";
	Vec2 textOffset = size*0.2f;
	text.setFont(font);
	text.setStyle(sf::Text::Bold);
	text.setPosition(pos);
	text.setString(txtString);
	text.setCharacterSize(sqrtf(size.x/txtString.getSize())*7);
	text.setOrigin(text.getLocalBounds().width*0.5f, text.getLocalBounds().height*0.9f);
	text.setFillColor(sf::Color::White);
}

void Widget::ChangeText(sf::String txtString) {
	text.setString(txtString);
	text.setCharacterSize(sqrtf(shape.getSize().x/txtString.getSize())*7);
	text.setOrigin(text.getLocalBounds().width*0.5f, text.getLocalBounds().height*0.9f);
}

Button::Button(std::string _name, Action _action, Vec2 pos, Vec2 size, sf::Color _color, sf::String txtString) {
	name = _name;
	buttons.insert(std::make_pair(name, this));
	action = _action;
	value = 0;
	shape.setPosition(Vec2(pos.x-size.x*0.5f, pos.y-size.y*0.5f));
	shape.setSize(size);
	shape.setFillColor(_color);
	color = _color;
	clickColor = Palette::Highlight1;
	backColor = color;
	if (!font.loadFromFile("Arimo-Bold.ttf"))
	    std::cout << "Failed loading the font.\n";
	text.setFont(font);
	text.setStyle(sf::Text::Bold);
	text.setPosition(pos);
	text.setString(txtString);
	text.setCharacterSize(sqrtf(size.x/txtString.getSize())*6);
	text.setOrigin(text.getLocalBounds().width*0.5f, text.getLocalBounds().height*0.9f);
	text.setFillColor(sf::Color::White);
}

void GUI::MakeLine(std::string name, Vec2 pos1, Vec2 pos2) {
	std::array<sf::Vertex, 2> line =
	{
		sf::Vertex(pos1, sf::Color::White),
		sf::Vertex(pos2, sf::Color::White)
	};
	lines.insert(std::make_pair(name, line));		
}

void GUI::UpdateLine(std::string name, Vec2 pos1, Vec2 pos2) {
	lines[name][0] = sf::Vertex(pos1);
	lines[name][1] = sf::Vertex(pos2);
}

void GUI::UpdateLine1(std::string name, Vec2 pos1) {
	lines[name][0] = sf::Vertex(pos1);
}

void GUI::UpdateLine2(std::string name, Vec2 pos2) {
	lines[name][1] = sf::Vertex(pos2);
}

void GUI::EraseLine(std::string name) {
	lines.erase(name);
}

void GUI::Erase(Widget* widget) {
	Widget::widgets.erase(widget->name);
	delete widget;
}

void GUI::Erase(Button* button) {
	Button::buttons.erase(button->name);
	delete button;
}

std::map<std::string, Widget*> Widget::widgets;
std::map<std::string, Button*> Button::buttons;

