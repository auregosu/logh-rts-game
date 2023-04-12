#include "Tools.hpp"

Vec2 operator/(const Vec2 &v1, const Vec2 &v2) {
	return Vec2(v1.x/v2.x, v1.y/v2.y);
}

std::ostream& operator<<(std::ostream& stream, const Vec2& vec)
{
	stream << "(" << vec.x << ", " << vec.y << ")";
	return stream;
}

std::vector<Vec2> ShapeToVector(sf::RectangleShape shape)
{
    std::vector<Vec2> points;
    sf::Transform t = shape.getTransform();
    for (int i = 0; i < shape.getPointCount(); i++)
    {
        points.push_back(t.transformPoint(shape.getPoint(i)));
    }
    return points;
}

std::vector<Vec2> ShapeToVector(sf::ConvexShape shape)
{
    std::vector<Vec2> points;
    sf::Transform t = shape.getTransform();
    for (int i = 0; i < shape.getPointCount(); i++)
    {
        points.push_back(t.transformPoint(shape.getPoint(i)));
        std::cout << points[i] << std::endl;
    }
    return points;
}

std::vector<Vec2> ShapeToVector(sf::CircleShape shape)
{
    std::vector<Vec2> points;
    sf::Transform t = shape.getTransform();
    for (int i = 0; i < shape.getPointCount(); i++)
    {
        points.push_back(t.transformPoint(shape.getPoint(i)));
    }
    return points;
}
