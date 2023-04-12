#ifndef TOOLS_HPP
#define TOOLS_HPP

#include <iostream>
#include <array>
#include <limits>
#include <SFML/Graphics.hpp>

typedef sf::Vector2f Vec2;

Vec2 operator/(const Vec2 &v1, const Vec2 &v2);

std::ostream& operator<<(std::ostream& stream, const Vec2& vec);

std::vector<Vec2> ShapeToVector(sf::RectangleShape shape);
std::vector<Vec2> ShapeToVector(sf::ConvexShape shape);
std::vector<Vec2> ShapeToVector(sf::CircleShape shape);

struct coord { 
    int x, y; 
    inline bool operator==(const coord &o) const {
        return x == o.x && y == o.y;
    }
    inline bool operator<(const coord &o)  const {
        return x < o.x || (x == o.x && y < o.y);
    }
};


enum GameStates {MENU, GAME};
enum MenuScreens {SHAPEMAKE, BUILD};

class Palette
{
public:
	inline static sf::Color Color1;
	inline static sf::Color Color2;
	inline static sf::Color Player1;
	inline static sf::Color Player2;
	inline static sf::Color Highlight1;
	inline static sf::Color Highlight2;
	inline static sf::Color Tech1;

	inline static void Initialize()
	{
		Color1 = sf::Color(65, 80, 80);
		Color2 = sf::Color(48, 60, 60);
		Player1 = sf::Color(69, 163, 155);
		Player2 = sf::Color(163, 69, 69);
		Highlight1 = sf::Color(101, 240, 228);
		Highlight2 = sf::Color(240, 101, 101);
		Tech1 = sf::Color(200, 200, 200);
	}
};

#endif
