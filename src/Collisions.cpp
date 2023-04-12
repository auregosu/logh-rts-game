#include "Collisions.hpp"
/*
std::vector<Vec2> Collisions::GetShapePoints(sf::Shape shape)
{
    std::vector<Vec2> points;
    for (int i = 0; i < shape.getPointCount(); i++)
    {
        sf::Transform t= shape.getTransform();
        points.push_back(t.transformPoint(shape.getPoint(i)));
    }
    return points;
}
*/
bool Collisions::areOverlapping(std::vector<Vec2> s1, std::vector<Vec2> s2)
{
    std::vector<Vec2>* shape1 = &s1;
    std::vector<Vec2>* shape2 = &s2;
    for (int ent = 0; ent < 2; ent++)
    {
        if (ent == 1)
        {
            shape1 = &s2;
            shape2 = &s1;
        }
        std::vector<Vec2> points1 = *shape1;
        std::vector<Vec2> points2 = *shape2;
        for (int a = 0; a < points1.size(); a++)
        {
            int b = (a + 1) % points1.size();
            Vec2 projVec = {points1[a].y - points1[b].y, points1[b].x - points1[a].x};
            float min1 = std::numeric_limits<float>::max(), 
                  max1 = std::numeric_limits<float>::min(), 
                  min2 = std::numeric_limits<float>::max(), 
                  max2 = std::numeric_limits<float>::min();
            for (auto& p : points1)
            {
                float q = (p.x * projVec.x + p.y * projVec.y);
                if (q < min1)
                    min1 = q;
                if (q > max1)
                    max1 = q;
            }
            for (auto& p : points2)
            {
                float q = (p.x * projVec.x + p.y * projVec.y);
                if (q < min2)
                    min2 = q;
                if (q > max2)
                    max2 = q;
            }
            if (!(max2 >= min1 && max1 >= min2))
                return false;
        }
    }
    return true;
}

bool Collisions::Contains(std::vector<Vec2> s1, Vec2 p)
{
    int N = s1.size();
    int counter = 0;
    int i;
    double xinters;
    Vec2 p1, p2;

    p1 = s1[0];
    for (i=1;i<=N;i++) {
        p2 = s1[i % N];
        if (p.y > std::min(p1.y,p2.y)) {
            if (p.y <= std::max(p1.y,p2.y)) {
                if (p.x <= std::max(p1.x,p2.x)) {
                    if (p1.y != p2.y) {
                        xinters = (p.y-p1.y)*(p2.x-p1.x)/(p2.y-p1.y)+p1.x;
                        if (p1.x == p2.x || p.x <= xinters)
                        counter++;
                    }
                }
            }
        }
        p1 = p2;
    }

    if (counter % 2 == 0)
        return false;
    else
        return true;
}
